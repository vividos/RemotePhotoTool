//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file SdkReferenceBase.hpp SDK Reference base class
//
#pragma once

class SourceInfo;

/// base class for all SDK Reference classes
class SdkReferenceBase
{
public:
   /// dtor
   virtual ~SdkReferenceBase()
   {
   }

   /// adds version text for this SDK
   virtual void AddVersionText(CString& versionText) const = 0;

   /// enumerates cameras
   virtual void EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& sourceDevicesList) const = 0;

   /// returns if AsyncWaitForCamera() is possible for this SDK, or if polling of EnumerateDevices() has to be used
   virtual bool IsAsyncWaitPossible() const = 0;

   /// starts waiting for camera
   virtual void AsyncWaitForCamera(bool start, std::function<void()> fnOnCameraConnected = std::function<void()>())
   {
      UNUSED(start);
      UNUSED(fnOnCameraConnected);
   };
};
