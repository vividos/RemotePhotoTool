//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.Instance.cpp SourceDevice wrapper class
//

// includes
#include "stdafx.h"
#include "RemotePhotoTool.CameraControl.SourceDevice.hpp"
#include "SourceDevice.hpp"

System::String^ RemotePhotoTool::CameraControl::SourceDevice::ModelName::get()
{
   return gcnew System::String(m_sourceDevice->get()->ModelName().GetString());
}

System::String^ RemotePhotoTool::CameraControl::SourceDevice::SerialNumber::get()
{
   return gcnew System::String(m_sourceDevice->get()->SerialNumber().GetString());
}

/// cleans up unmanaged resources
RemotePhotoTool::CameraControl::SourceDevice::!SourceDevice()
{
   delete m_sourceDevice;
   m_sourceDevice = nullptr;
}

/// cleans up managed resources
RemotePhotoTool::CameraControl::SourceDevice::~SourceDevice()
{
}

RemotePhotoTool::CameraControl::SourceDevice::SourceDevice(std::shared_ptr<::SourceDevice> sourceDevice)
:m_sourceDevice(new std::shared_ptr<::SourceDevice>(sourceDevice))
{
}
