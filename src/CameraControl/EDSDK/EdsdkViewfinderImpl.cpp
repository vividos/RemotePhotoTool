//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file EdsdkViewfinderImpl.cpp EDSDK - Viewfinder impl
//

// includes
#include "stdafx.h"
#include "EdsdkViewfinderImpl.hpp"
#include "EdsdkPropertyAccess.hpp"
#include "BackgroundTimer.hpp"

using namespace EDSDK;

ViewfinderImpl::ViewfinderImpl(Handle hSourceDevice, boost::asio::io_service& ioService, std::shared_ptr<LightweightMutex> spMtxLock)
:m_hSourceDevice(hSourceDevice),
m_ioService(ioService),
m_spMtxLock(spMtxLock),
m_evtTimerStopped(false)
{
   // Get the output device for the live view image
   PropertyAccess p(hSourceDevice);

   Variant vDevice = p.Get(kEdsPropID_Evf_OutputDevice);
   ATLASSERT(vDevice.Type() == Variant::typeUInt32);

   unsigned int device = vDevice.Get<unsigned int>();

   // PC live view starts by setting the PC as the output device for the live view image.
   device |= kEdsEvfOutputDevice_PC;

   vDevice.Set(device);

   {
      LightweightMutex::LockType lock(*spMtxLock);
      p.Set(kEdsPropID_Evf_OutputDevice, vDevice);
   }

   // A property change event notification is issued from the camera if property settings are made successfully.
   // Start downloading of the live view image once the property change notification arrives.
}

ViewfinderImpl::~ViewfinderImpl()
{
   try
   {
      // don't stop background thread in dtor, since background thread would be stopped asynchronous,
      // and in the dtor it's too late to use shared_from_this() there. So SetAvailImageHandler() must
      // be called before reset()ing a shared ptr to this class.
      //StopBackgroundThread();

      Close();
   }
   catch (...)
   {
   }
}

void ViewfinderImpl::Close()
{
   // clear live view flag in property
   PropertyAccess p(m_hSourceDevice);

   Variant vDevice = p.Get(kEdsPropID_Evf_OutputDevice);
   ATLASSERT(vDevice.Type() == Variant::typeUInt32);

   unsigned int device = vDevice.Get<unsigned int>();
   device &= ~kEdsEvfOutputDevice_PC;

   vDevice.Set(device);

   p.Set(kEdsPropID_Evf_OutputDevice, vDevice);
}

bool ViewfinderImpl::GetCapability(T_enViewfinderCapability enViewfinderCapability) const
{
   switch (enViewfinderCapability)
   {
   case Viewfinder::capOutputTypeVideoOut:
      return false; // not supported in EDSDK

   case Viewfinder::capGetHistogram:
      return true; // supported in EDSDK

   default:
      ATLASSERT(false);
      break;
   }

   return false;
}

void ViewfinderImpl::SetOutputType(T_enOutputType enOutputType)
{
   unsigned int output = 0;

   switch (enOutputType)
   {
   case Viewfinder::outputTypeUndefined:
      return; // do nothing

   case Viewfinder::outputTypeLCD:
      // corresponds to "Camera and PC", to have viewfinder on camera LCD also
      output = kEdsEvfOutputDevice_TFT | kEdsEvfOutputDevice_PC;
      break;

   case Viewfinder::outputTypeVideoOut:
      return; // do nothing, since on EDSDK, video out can't be set

   case Viewfinder::outputTypeOff:
      // corresponds to "PC", not "Off", to not shutdown viewfinder at all
      output = kEdsEvfOutputDevice_PC;
      break;

   default:
      ATLASSERT(false);
      return;
   }

   Variant outputValue;
   outputValue.Set(output);
   outputValue.SetType(Variant::typeUInt32);

   PropertyAccess p(m_hSourceDevice);
   p.Set(kEdsPropID_Evf_OutputDevice, outputValue);
}

void ViewfinderImpl::SetAvailImageHandler(Viewfinder::T_fnOnAvailViewfinderImage fnOnAvailViewfinderImage)
{
   {
      LightweightMutex::LockType lock(m_mtxFnOnAvailViewfinderImage);

      m_fnOnAvailViewfinderImage = fnOnAvailViewfinderImage;
   }

   if (fnOnAvailViewfinderImage != nullptr)
      StartBackgroundThread();
   else
      StopBackgroundThread();
}

void ViewfinderImpl::StartBackgroundThread()
{
   m_spViewfinderImageTimer.reset(); // clear old timers

   m_spViewfinderImageTimer.reset(
      new BackgroundTimer(
         m_ioService,
         50, // 50 ms results in 20 fps
         std::bind(&ViewfinderImpl::OnGetViewfinderImage, this)
         ));

   m_spViewfinderImageTimer->Start();
}

void ViewfinderImpl::StopBackgroundThread()
{
   if (m_spViewfinderImageTimer == nullptr)
      return;

   m_evtTimerStopped.Reset();

   // Run in worker thread, since we must not block the window message processing, needed in
   // EDSDK functions called in GetImage(). Since caller might destroy the Viewfinder class right
   // away, use shared_from_this() to manage lifetime of this class.
   m_ioService.post(
      std::bind(&ViewfinderImpl::AsyncStopBackgroundThread, shared_from_this()));

   if (!m_hSourceDevice.IsValid())
      return;

   MutexTryLock<RecursiveMutex> tryLock(m_hSourceDevice.GetRef()->SdkFunctionMutex());

   while (!tryLock.Try(10))
      EDSDK::Ref::OnIdle();

   m_evtTimerStopped.Wait();
}

void ViewfinderImpl::AsyncStopBackgroundThread()
{
   // stop timer
   if (m_spViewfinderImageTimer != nullptr)
      m_spViewfinderImageTimer->Stop();

   m_spViewfinderImageTimer.reset();

   m_evtTimerStopped.Set();
}

void ViewfinderImpl::GetHistogram(T_enHistogramType enHistogramType, std::vector<unsigned int>& vecHistogramData)
{
   static_assert(sizeof(EdsUInt32) == sizeof(unsigned int), "types EdsUInt32 and unsigned int must match");

   LightweightMutex::LockType lock(m_mtxHistogram);

   std::reference_wrapper<const EdsUInt32[256]> histogram(m_histogramY);

   switch (enHistogramType)
   {
   case Viewfinder::histogramLuminance:
      histogram = std::cref(m_histogramY);
      break;

   case Viewfinder::histogramRed:
      histogram = std::cref(m_histogramR);
      break;

   case Viewfinder::histogramGreen:
      histogram = std::cref(m_histogramG);
      break;

   case Viewfinder::histogramBlue:
      histogram = std::cref(m_histogramB);
      break;

   default:
      ATLASSERT(false);
      return;
   }

   vecHistogramData.resize(256);
   std::copy(std::begin(histogram.get()), std::end(histogram.get()), vecHistogramData.begin());
}

void ViewfinderImpl::GetImage(std::vector<BYTE>& vecImage)
{
   if (!m_hSourceDevice.IsValid())
      return;

   MutexTryLock<RecursiveMutex> tryLock(m_hSourceDevice.GetRef()->SdkFunctionMutex());

   while (!tryLock.Try(10))
      EDSDK::Ref::OnIdle();

   //LOG_TRACE(_T("GetImage() start\n"));
   m_bInGetImage = true;

   LightweightMutex::LockType lock(*m_spMtxLock);

   // create memory stream
   Handle hStream(m_hSourceDevice.GetRef());
   EdsError err = EdsCreateMemoryStream(0, &hStream);
   if (err != EDS_ERR_OK)
      return;

   // create EvfImageRef
   Handle hEvfImage(m_hSourceDevice.GetRef());
   err = EdsCreateEvfImageRef(hStream, &hEvfImage);
   if (err != EDS_ERR_OK)
      return;

   // download live view image data
   err = EdsDownloadEvfImage(m_hSourceDevice, hEvfImage);
   if (err != EDS_ERR_OK)
      return;

   // transfer the image data
   EdsUInt64 uiLength = 0;
   err = EdsGetLength(hStream, &uiLength);
   if (err != EDS_ERR_OK)
      return;

   EdsVoid* pData = nullptr;
   err = EdsGetPointer(hStream, &pData);
   if (err != EDS_ERR_OK)
      return;

   if (pData != 0 && uiLength > 0)
   {
      const BYTE* pbData = reinterpret_cast<BYTE*>(pData);
      vecImage.assign(pbData, pbData + uiLength);
   }

   ReadHistogram(hEvfImage);

   m_bInGetImage = false;
   //LOG_TRACE(_T("GetImage() end\n"));
}

void ViewfinderImpl::ReadHistogram(Handle& hEvfImage)
{
   LightweightMutex::LockType lock(m_mtxHistogram);

   EdsGetPropertyData(hEvfImage, kEdsPropID_Evf_HistogramY, 0, sizeof(m_histogramY), m_histogramY);
   EdsGetPropertyData(hEvfImage, kEdsPropID_Evf_HistogramR, 0, sizeof(m_histogramR), m_histogramR);
   EdsGetPropertyData(hEvfImage, kEdsPropID_Evf_HistogramG, 0, sizeof(m_histogramG), m_histogramG);
   EdsGetPropertyData(hEvfImage, kEdsPropID_Evf_HistogramB, 0, sizeof(m_histogramB), m_histogramB);
}

void ViewfinderImpl::OnGetViewfinderImage()
{
   {
      LightweightMutex::LockType lock(m_mtxFnOnAvailViewfinderImage);

      if (m_fnOnAvailViewfinderImage == nullptr)
         return;
   }

   std::vector<BYTE> vecImage;
   GetImage(vecImage);

   LightweightMutex::LockType lock(m_mtxFnOnAvailViewfinderImage);

   if (m_fnOnAvailViewfinderImage != nullptr)
      m_fnOnAvailViewfinderImage(vecImage);
}
