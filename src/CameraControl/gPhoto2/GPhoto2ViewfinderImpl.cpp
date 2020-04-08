//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2ViewfinderImpl.cpp gPhoto2 - Viewfinder impl
//
#include "stdafx.h"
#include "GPhoto2ViewfinderImpl.hpp"
#include "GPhoto2PropertyAccess.hpp"
#include "GPhoto2Include.hpp"
#include "BackgroundTimer.hpp"

using GPhoto2::ViewfinderImpl;

ViewfinderImpl::ViewfinderImpl(RefSp ref,
   std::shared_ptr<_Camera> camera,
   std::shared_ptr<PropertyAccess> properties,
   boost::asio::io_service& ioService)
   :m_ref(ref),
   m_camera(camera),
   m_properties(properties),
   m_ioService(ioService),
   m_eventTimerStopped(false)
{
}

ViewfinderImpl::~ViewfinderImpl()
{
   try
   {
      Close();
   }
   catch (...)
   {
   }
}

bool ViewfinderImpl::GetCapability(T_enViewfinderCapability viewfinderCapability) const
{
   switch (viewfinderCapability)
   {
   case Viewfinder::capOutputTypeVideoOut:
      return m_properties->IsAvailPropertyName("output");

   case Viewfinder::capGetHistogram:
      return false; // TODO check for EOS histogram property?

   default:
      ATLASSERT(false);
      break;
   }

   return false;
}

void ViewfinderImpl::SetOutputType(T_enOutputType outputType)
{
   // TODO implement
}

void ViewfinderImpl::SetAvailImageHandler(T_fnOnAvailViewfinderImage onAvailViewfinderImage)
{
   {
      LightweightMutex::LockType lock{ m_mutexFnOnAvailViewfinderImage };

      m_onAvailViewfinderImage = onAvailViewfinderImage;
   }

   if (onAvailViewfinderImage != nullptr)
      StartBackgroundThread();
   else
      StopBackgroundThread();
}

void ViewfinderImpl::GetHistogram(T_enHistogramType histogramType, std::vector<unsigned int>& histogramData)
{
   // TODO
}

void ViewfinderImpl::Close()
{
   StopBackgroundThread();
}

void ViewfinderImpl::StartBackgroundThread()
{
   m_viewfinderImageTimer.reset(); // clear old timers

   m_viewfinderImageTimer.reset(
      new BackgroundTimer(
         m_ioService,
         50, // 50 ms results in 20 fps
         std::bind(&ViewfinderImpl::OnGetViewfinderImage, this)
      ));

   m_viewfinderImageTimer->Start();
}

void ViewfinderImpl::StopBackgroundThread()
{
   if (m_viewfinderImageTimer == nullptr)
      return;

   m_eventTimerStopped.Reset();

   m_ioService.post(
      std::bind(&ViewfinderImpl::AsyncStopBackgroundThread, shared_from_this()));

   m_eventTimerStopped.Wait();
}

void ViewfinderImpl::AsyncStopBackgroundThread()
{
   // stop timer
   if (m_viewfinderImageTimer != nullptr)
      m_viewfinderImageTimer->Stop();

   m_viewfinderImageTimer.reset();

   m_eventTimerStopped.Set();
}

void ViewfinderImpl::OnGetViewfinderImage()
{
   {
      LightweightMutex::LockType lock(m_mutexFnOnAvailViewfinderImage);

      if (m_onAvailViewfinderImage == nullptr)
         return;
   }

   std::vector<BYTE> imageData;
   GetImage(imageData);

   LightweightMutex::LockType lock(m_mutexFnOnAvailViewfinderImage);

   if (m_onAvailViewfinderImage != nullptr)
      m_onAvailViewfinderImage(imageData);
}

void ViewfinderImpl::GetImage(std::vector<BYTE>& imageData)
{
   CameraFile* rawFile = nullptr;
   int ret = gp_file_new(&rawFile);
   CheckError(_T("gp_file_new"), ret, __FILE__, __LINE__);

   auto file = std::shared_ptr<CameraFile>(rawFile, gp_file_free);

   ret = gp_camera_capture_preview(m_camera.get(), file.get(), m_ref->GetContext().get());
   CheckError(_T("gp_camera_capture_preview"), ret, __FILE__, __LINE__);

   unsigned long size = 0;
   const char* data = nullptr;
   ret = gp_file_get_data_and_size(file.get(), &data, &size);
   CheckError(_T("gp_file_get_data_and_size"), ret, __FILE__, __LINE__);

   const BYTE* dataBytes = reinterpret_cast<const BYTE*>(data);
   imageData.assign(data, data + size);
}
