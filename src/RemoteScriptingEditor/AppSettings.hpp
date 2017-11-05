//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file RemoteScriptingEditor\AppSettings.hpp Settings
//
#pragma once

// includes
#include "../RemotePhotoTool/WindowPlacement.hpp"

/// application settings
class AppSettings
{
public:
   /// ctor
   explicit AppSettings(LPCTSTR pszSettingsRegkey);


   // properties

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
