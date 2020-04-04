//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file SourceDevice.hpp Canon control - Source device
//
#pragma once

// includes
#include "DeviceProperty.hpp"

// forward references
class CameraFileSystem;
class RemoteReleaseControl;

/// camera source device
class SourceDevice
{
public:
   /// dtor
   virtual ~SourceDevice() {}

   /// device capabilities
   enum T_enDeviceCapability
   {
      capRemoteReleaseControl = 0,  ///< indicates if device supports remote release control
      capRemoteViewfinder = 1,      ///< indicates if device supports remote viewfinder
      capCameraFileSystem = 2,      ///< indicates if device supports file system access
   };

   /// returns device capability
   virtual bool GetDeviceCapability(T_enDeviceCapability enDeviceCapability) const = 0;

   /// returns model name
   virtual CString ModelName() const = 0;

   /// returns camera serial number
   virtual CString SerialNumber() const = 0;

   /// enumerates all available device properties
   virtual std::vector<unsigned int> EnumDeviceProperties() const = 0;

   /// returns property object for given property id
   virtual DeviceProperty GetDeviceProperty(unsigned int uiPropertyId) const = 0;

   /// returns camera file system instance
   virtual std::shared_ptr<CameraFileSystem> GetFileSystem() = 0;

   /// enters remote release control and returns class to let the user control release
   virtual std::shared_ptr<RemoteReleaseControl> EnterReleaseControl() = 0;
};
