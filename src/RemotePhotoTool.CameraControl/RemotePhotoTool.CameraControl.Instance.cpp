//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.Instance.cpp Instance wrapper class
//

// includes
#include "stdafx.h"
#include "RemotePhotoTool.CameraControl.Instance.hpp"

void RemotePhotoTool::CameraControl::Instance::EnableLogging(bool enable, System::String^ logFilePath)
{
   ::Instance::EnableLogging(enable, CString(logFilePath));
}

RemotePhotoTool::CameraControl::Instance^ RemotePhotoTool::CameraControl::Instance::Get()
{
   return gcnew RemotePhotoTool::CameraControl::Instance();
}

System::String^ RemotePhotoTool::CameraControl::Instance::Version::get()
{
   return gcnew System::String(m_instance->Version().GetString());
}

void RemotePhotoTool::CameraControl::Instance::AsyncWaitForCamera()
{
   AsyncWaitForCamera(nullptr);
}

static void OnCameraAdded(msclr::gcroot<RemotePhotoTool::CameraControl::Instance::CameraAddedHandler^> handler)
{
   RemotePhotoTool::CameraControl::Instance::CameraAddedHandler^ callback = handler;
   callback();
}

void RemotePhotoTool::CameraControl::Instance::AsyncWaitForCamera(CameraAddedHandler^ handler)
{
   // TODO implement
   if (handler == nullptr)
   {
      m_instance->AsyncWaitForCamera();
      return;
   }

   // lambdas can't capture delegates, so jump through some hoops here
   msclr::gcroot<CameraAddedHandler^> callbackHandler(handler);

   ::Instance::T_fnOnCameraAdded fn =
      std::bind(&OnCameraAdded, callbackHandler);

   m_instance->AsyncWaitForCamera(fn);
}

System::Collections::Generic::List<RemotePhotoTool::CameraControl::SourceInfo^>^
RemotePhotoTool::CameraControl::Instance::EnumerateDevices()
{
   auto sourceInfoList = gcnew System::Collections::Generic::List<RemotePhotoTool::CameraControl::SourceInfo^>();

   // TODO implement

   return sourceInfoList;
}

void RemotePhotoTool::CameraControl::Instance::OnIdle()
{
   ::Instance::OnIdle();
}

/// cleans up unmanaged resources
RemotePhotoTool::CameraControl::Instance::!Instance()
{
   delete m_instance;
   m_instance = nullptr;
}

/// cleans up managed resources
RemotePhotoTool::CameraControl::Instance::~Instance()
{
}

RemotePhotoTool::CameraControl::Instance::Instance()
:m_instance(new ::Instance(::Instance::Get()))
{
}
