//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file EdsdkViewfinderImpl.cpp EDSDK - Viewfinder impl
//
#pragma once

// includes
#include "stdafx.h"
#include "EdsdkViewfinderImpl.hpp"
#include "EdsdkPropertyAccess.hpp"
#include "BackgroundWorkerThread.hpp"

using namespace EDSDK;

ViewfinderImpl::ViewfinderImpl(Handle hSourceDevice, boost::asio::io_service& ioService, std::shared_ptr<LightweightMutex> spMtxLock)
   :m_hSourceDevice(hSourceDevice),
   m_ioService(ioService),
   m_spMtxLock(spMtxLock)
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

   // TODO wait for notification

   // note: don't bother downloading viewfinder images until handler is set
}

ViewfinderImpl::~ViewfinderImpl()
{
   // don't stop background thread in dtor, since background thread would be stopped asynchronous,
   // and in the dtor it's too late to use shared_from_this() there. So SetAvailImageHandler() must
   // be called before reset()ing a shared ptr to this class.
   //StopBackgroundThread();

   // clear live view flag in property
   PropertyAccess p(m_hSourceDevice);

   Variant vDevice = p.Get(kEdsPropID_Evf_OutputDevice);
   ATLASSERT(vDevice.Type() == Variant::typeUInt32);

   unsigned int device = vDevice.Get<unsigned int>();
   device &= ~kEdsEvfOutputDevice_PC;

   vDevice.Set(device);

   p.Set(kEdsPropID_Evf_OutputDevice, vDevice);
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

   // Run in worker thread, since we must not block the window message processing, needed in
   // EDSDK functions called in GetImage(). Since caller might destroy the Viewfinder class right
   // away, use shared_from_this() to manage lifetime of this class.
   m_ioService.post(
      std::bind(&ViewfinderImpl::AsyncStopBackgroundThread, shared_from_this()));
}

void ViewfinderImpl::AsyncStopBackgroundThread()
{
   // stop timer
   if (m_spViewfinderImageTimer != nullptr)
      m_spViewfinderImageTimer->Stop();

   m_spViewfinderImageTimer.reset();
}

void ViewfinderImpl::GetImage(std::vector<BYTE>& vecImage)
{
   LOG_TRACE(_T("GetImage() start\n"));

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
   EdsUInt32 uiLength = 0;
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
      vecImage.assign(pbData, pbData+uiLength);
   }

   LOG_TRACE(_T("GetImage() end\n"));
}

void ViewfinderImpl::OnGetViewfinderImage()
{
   std::vector<BYTE> vecImage;
   GetImage(vecImage);

   LightweightMutex::LockType lock(m_mtxFnOnAvailViewfinderImage);

   if (m_fnOnAvailViewfinderImage != nullptr)
      m_fnOnAvailViewfinderImage(vecImage);
}
