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
#include "SingleThreadExecutor.hpp"
#include "PeriodicExecuteTimer.hpp"

using GPhoto2::ViewfinderImpl;

ViewfinderImpl::ViewfinderImpl(RefSp ref,
   std::shared_ptr<_Camera> camera,
   std::shared_ptr<PropertyAccess> properties,
   SingleThreadExecutor& executor)
   :m_ref(ref),
   m_camera(camera),
   m_properties(properties),
   m_executor(executor),
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
   int outputValue = 0;
   switch (outputType)
   {
   case Viewfinder::outputTypeUndefined:
      return; // do nothing

   case Viewfinder::outputTypeLCD:
      outputValue = 0;
      break;

   case Viewfinder::outputTypeVideoOut:
      outputValue = 1;
      break;

   case Viewfinder::outputTypeOff:
      outputValue = 2;
      break;

   default:
      ATLASSERT(false);
      return;
   }

   std::vector<CString> validValuesList = m_properties->GetValidValues("output");

   if (static_cast<size_t>(outputValue) >= validValuesList.size())
      return; // index not available

   Variant value;
   value.Set(validValuesList[outputValue]);
   value.SetType(Variant::typeInt32);

   m_properties->SetPropertyByName(_T("output"), value);
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
   // TODO implement
   UNUSED(histogramType);
   UNUSED(histogramData);
}

void ViewfinderImpl::Close()
{
   StopBackgroundThread();
}

void ViewfinderImpl::StartBackgroundThread()
{
   m_viewfinderImageTimer.reset(); // clear old timers

   m_viewfinderImageTimer.reset(
      new PeriodicExecuteTimer(
         m_executor,
         50, // 50 ms results in 20 fps
         std::bind(&ViewfinderImpl::OnGetViewfinderImage, this)
      ));
}

void ViewfinderImpl::StopBackgroundThread()
{
   if (m_viewfinderImageTimer == nullptr)
      return;

   m_eventTimerStopped.Reset();

   m_executor.Schedule(
      std::bind(&ViewfinderImpl::AsyncStopBackgroundThread, shared_from_this()));

   m_eventTimerStopped.Wait();
}

void ViewfinderImpl::AsyncStopBackgroundThread()
{
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
   imageData.assign(dataBytes, dataBytes + size);
}
