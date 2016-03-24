//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file AppCommand.hpp Command line app command
//
#pragma once

/// command line app command
struct AppCommand
{
   /// app command type
   enum T_enCommand
   {
      showVersion,      ///< shows version
      listDevices,      ///< lists all devices
      openDevice,       ///< opens device
      closeDevice,      ///< closes device
      deviceInfo,       ///< outputs device info
      deviceProperties, ///< outputs device properties
      checkUnknownDeviceProps, ///< checks for unknown device properties
      imageProperties,  ///< outputs image properties
      checkUnknownImageProps, ///< checks for unknown image properties
      remoteCapabilities, ///< outputs remote capabilities
      listenEvents,     ///< listens for events
      releaseShutter,   ///< releases shutter
      runScript,        ///< runs Lua script
   };

   /// ctor
   AppCommand(T_enCommand enCommand, const CString& cszData = _T(""))
      :m_enCommand(enCommand),
       m_cszData(cszData)
   {
   }

   /// command
   T_enCommand m_enCommand;

   /// data needed for command
   CString m_cszData;
};
