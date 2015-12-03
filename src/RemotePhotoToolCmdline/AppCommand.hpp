//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
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
      imageProperties,  ///< outputs image properties
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
