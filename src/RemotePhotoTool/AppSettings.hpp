//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file AppSettings.hpp Settings
//
#pragma once

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


   // actions

   /// stores settings
   void Store();

   /// loads settings
   void Load();

private:
   /// settings registry key
   LPCTSTR m_pszSettingsRegkey;
};
