//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool\AppSettings.hpp Settings
//
#pragma once

// includes
#include "WindowPlacement.hpp"

/// application settings
class AppSettings
{
public:
   /// ctor
   AppSettings(LPCTSTR pszSettingsRegkey);


   // properties

   /// projects folder
   CString m_cszProjectsFolder;

   /// indicates if current date is used for first level subfolder
   bool m_bCurrentDateSubfolder;

   /// indicates if image type should be used as second level subfolder
   bool m_bImageTypeSubfolder;

   /// photomatix executable
   CString m_cszPhotomatixPath;

   /// hugin executable
   CString m_cszHuginPath;

   /// indicates if logging is enabled
   bool m_bLogging;

   /// path for logfile
   CString m_cszLogfilePath;

   /// window placement of main frame
   WindowPlacement m_windowPlacementMainFrame;


   // actions

   /// stores settings
   void Store();

   /// loads settings
   void Load();

private:
   /// settings registry key
   LPCTSTR m_pszSettingsRegkey;
};
