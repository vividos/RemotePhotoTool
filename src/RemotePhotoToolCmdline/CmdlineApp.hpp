//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CmdlineApp.hpp Command line app
//
#pragma once

// includes
#include <vector>
#include "AppCommand.hpp"

// forward references
class SourceDevice;
class RemoteReleaseControl;

/// command line app
class CmdlineApp
{
public:
   /// ctor
   CmdlineApp();

   /// runs command line
   void Run(int argc, TCHAR* argv[]);

private:
   /// executes app command
   void Exec(const AppCommand& cmd);

   void PrintVersionInfo();                     ///< outputs version info
   void ListDevices();                          ///< outputs list of devices
   void OpenByName(const CString& cszName);     ///< opens device by name
   void OutputDeviceInfo();                     ///< outputs device inf
   void ListDeviceProperties();                 ///< outputs device properties
   void ListImageProperties();                  ///< outputs image properties
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
