//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.SourceDevice.hpp SourceDevice wrapper class
//
#pragma once

// includes
#include "Instance.hpp"

class SourceDevice;

namespace RemotePhotoTool
{
   namespace CameraControl
   {
      ref class DeviceProperty;
      ref class RemoteReleaseControl;

      /// camera source device
      public ref class SourceDevice
      {
      public:
         /// device capabilities
         enum class DeviceCapability
         {
            /// indicates if device supports remote release control
            RemoteReleaseControl = 0,

            /// indicates if device supports remote viewfinder
            RemoteViewfinder = 1,
         };

         /// returns model name
         property System::String^ ModelName
         {
            System::String^ get();
         }

         /// returns camera serial number
         property System::String^ SerialNumber
         {
            System::String^ get();
         }

         /// returns device capability
         bool GetDeviceCapability(DeviceCapability deviceCapability);

         /// enumerates all available device properties
         System::Collections::Generic::List<DeviceProperty^>^ EnumDeviceProperties();

         /// enters remote release control and returns class to let the user control release
         RemoteReleaseControl^ EnterReleaseControl();

         /// finalizer
         !SourceDevice();

         /// dtor
         ~SourceDevice();

      internal:
         /// ctor; used in SourceInfo.Open()
         SourceDevice(std::shared_ptr<::SourceDevice> sourceDevice);

      private:
         /// implementation
         std::shared_ptr<::SourceDevice>* m_sourceDevice;
      };
   }
}
