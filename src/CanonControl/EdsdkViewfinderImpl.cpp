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

ViewfinderImpl::ViewfinderImpl(Handle hSourceDevice)
   :m_hSourceDevice(hSourceDevice)
{
   // Get the output device for the live view image
   PropertyAccess p(hSourceDevice);

   Variant vDevice = p.Get(kEdsPropID_Evf_OutputDevice);
   ATLASSERT(vDevice.Type() == Variant::typeUInt32);

   unsigned int device = vDevice.Get<unsigned int>();

   // PC live view starts by setting the PC as the output device for the live view image.
   device |= kEdsEvfOutputDevice_PC;

   vDevice.Set(device);

   p.Set(kEdsPropID_Evf_OutputDevice, vDevice);

   // A property change event notification is issued from the camera if property settings are made successfully.
   // Start downloading of the live view image once the property change notification arrives.

   // TODO wait for notification

   // note: don't bother downloading viewfinder images until handler is set
}

ViewfinderImpl::~ViewfinderImpl()
{
   StopBackgroundThread();

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

   if (!fnOnAvailViewfinderImage.empty())
      StartBackgroundThread();
   else
      StopBackgroundThread();
}

void ViewfinderImpl::StartBackgroundThread()
{
   ATLTRACE(_T("!!! StartBackgroundThread\n"));
   m_spViewfinderImageThread.reset(); // clear old timers

   // start thread that polls for viewfinder images
   m_spWorkerThread.reset(new BackgroundWorkerThread);

   m_spViewfinderImageThread.reset(
      new BackgroundTimer(
         m_spWorkerThread->GetIoService(),
         50, // 50 ms results in 20 fps
         boost::bind(&ViewfinderImpl::OnGetViewfinderImage, this)
         ));

   m_spViewfinderImageThread->Start();
}

void ViewfinderImpl::StopBackgroundThread()
{
   ATLTRACE(_T("!!! StopBackgroundThread\n"));

   // stop timer
   if (m_spViewfinderImageThread != nullptr)
      m_spViewfinderImageThread->Stop();

   m_spViewfinderImageThread.reset();

   // stop thread
   m_spWorkerThread.reset();
}

void ViewfinderImpl::GetImage(std::vector<BYTE>& vecImage)
{
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
}

void ViewfinderImpl::OnGetViewfinderImage()
{
   std::vector<BYTE> vecImage;
   GetImage(vecImage);

   LightweightMutex::LockType lock(m_mtxFnOnAvailViewfinderImage);

   if (m_fnOnAvailViewfinderImage != nullptr)
      m_fnOnAvailViewfinderImage(vecImage);
}
