//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file CmdlineApp.hpp Command line app
//
#pragma once

// includes
#include <vector>
#include "AppCommand.hpp"

// forward references
class SourceDevice;
class DeviceProperty;
class RemoteReleaseControl;

/// command line app
class CmdlineApp
{
public:
   /// ctor
   CmdlineApp();

   /// inits crash reporter
   static void InitCrashReporter();

   /// runs command line
   void Run(int argc, TCHAR* argv[]);

private:
   /// executes app command
   void Exec(const AppCommand& cmd);

   void PrintVersionInfo();                     ///< outputs version info
   void ListDevices();                          ///< outputs list of devices
   void OpenByName(const CString& cszName);     ///< opens device by name
   void OutputDeviceInfo();                     ///< outputs device infos
   void ListDeviceProperties();                 ///< outputs device properties
   void CheckUnknownDeviceProperties();         ///< checks for unknown device properties
   void PrintValidDevicePropertyValues(const DeviceProperty& dp) const; ///< prints valid device property values
   void ListImageProperties();                  ///< outputs image properties
   void CheckUnknownImageProperties();          ///< checks for unknown image properties
   void PrintValidImagePropertyValues(unsigned int propertyId); ///< prints valid image property values
   void ListRemoteCapabilities();               ///< outputs remote capabilities
   void ListenToEvents();                       ///< listens to events
   void EnsureReleaseControl();                 ///< ensures that remote release control is set
   void ReleaseShutter();                       ///< releases shutter
   void RunScript(const CString& cszFilename);  ///< runs Lua script

private:
   /// app command list
   std::vector<AppCommand> m_vecCommandList;

   /// current source device
   std::shared_ptr<SourceDevice> m_spSourceDevice;

   /// current remote release control
   std::shared_ptr<RemoteReleaseControl> m_spReleaseControl;
};
