//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file Instance.hpp Canon control - Instance class
//
#pragma once

// includes
#include <vector>
#include "ImplFwd.hpp"

// forward references
class SourceInfo;

/// canon control instance class
class Instance
{
public:
   /// callback function type for AsyncWaitForCamera()
   typedef std::function<void()> T_fnOnCameraAdded;

   /// enables or disables logging; default: disabled
   static void EnableLogging(bool bEnable, const CString& cszLogfilePath);

   /// returns new camera instance
   static Instance Get();

   /// dtor
   ~Instance();

   /// returns version numbers and strings of all SDKs supported (multiline)
   CString Version() const;

   /// registers a handler function that is called when a camera is detected
   void AsyncWaitForCamera(T_fnOnCameraAdded fnOnCameraAdded = T_fnOnCameraAdded());

   /// enumerates all devices
   void EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& vecSourceDevices) const;

private:
   class Impl;

   /// ctor
   Instance(std::shared_ptr<Impl> spImpl);

   /// handler when camera is connected
   void OnCameraAddedHandler();

private:
   /// implementation
   std::shared_ptr<Impl> m_spImpl;
};
