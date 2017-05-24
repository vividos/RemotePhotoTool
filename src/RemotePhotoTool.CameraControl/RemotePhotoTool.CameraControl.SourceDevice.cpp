//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.SourceDevice.cpp SourceDevice wrapper class
//

// includes
#include "stdafx.h"
#include "RemotePhotoTool.CameraControl.SourceDevice.hpp"
#include "SourceDevice.hpp"
#include "RemotePhotoTool.CameraControl.DeviceProperty.hpp"
#include "RemotePhotoTool.CameraControl.RemoteReleaseControl.hpp"

System::String^ RemotePhotoTool::CameraControl::SourceDevice::ModelName::get()
{
   return gcnew System::String(m_sourceDevice->get()->ModelName().GetString());
}

System::String^ RemotePhotoTool::CameraControl::SourceDevice::SerialNumber::get()
{
   return gcnew System::String(m_sourceDevice->get()->SerialNumber().GetString());
}

bool RemotePhotoTool::CameraControl::SourceDevice::GetDeviceCapability(
   RemotePhotoTool::CameraControl::SourceDevice::DeviceCapability deviceCapability)
{
   ::SourceDevice::T_enDeviceCapability enDeviceCapability =
      static_cast<::SourceDevice::T_enDeviceCapability>(deviceCapability);

   return m_sourceDevice->get()->GetDeviceCapability(enDeviceCapability);
}

System::Collections::Generic::List<RemotePhotoTool::CameraControl::DeviceProperty^>^
RemotePhotoTool::CameraControl::SourceDevice::EnumDeviceProperties()
{
   std::vector<unsigned int> devicePropertyList = m_sourceDevice->get()->EnumDeviceProperties();

   auto devicePropertyCollection = gcnew System::Collections::Generic::List<RemotePhotoTool::CameraControl::DeviceProperty^>();

   for (size_t index = 0, max = devicePropertyList.size(); index < max; index++)
   {
      unsigned int devicePropertyId = devicePropertyList[index];

      ::DeviceProperty deviceProperty = m_sourceDevice->get()->GetDeviceProperty(devicePropertyId);

      devicePropertyCollection->Add(gcnew RemotePhotoTool::CameraControl::DeviceProperty(deviceProperty));
   }

   return devicePropertyCollection;
}

RemotePhotoTool::CameraControl::RemoteReleaseControl^
RemotePhotoTool::CameraControl::SourceDevice::EnterReleaseControl()
{
   std::shared_ptr<::RemoteReleaseControl> spRemoteReleaseControl =
      m_sourceDevice->get()->EnterReleaseControl();

   auto remoteReleaseControl =
      gcnew RemotePhotoTool::CameraControl::RemoteReleaseControl(spRemoteReleaseControl);

   remoteReleaseControl->InitEventHandler();

   return remoteReleaseControl;
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

RemotePhotoTool::CameraControl::SourceDevice::SourceDevice(
   std::shared_ptr<::SourceDevice> sourceDevice)
:m_sourceDevice(new std::shared_ptr<::SourceDevice>(sourceDevice))
{
}
