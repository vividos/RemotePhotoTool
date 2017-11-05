//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemoteScriptingEditor\AppSettings.cpp Settings
//

// includes
#include "stdafx.h"
#include "AppSettings.hpp"

LPCTSTR g_pszWindowPlacementMainFrame = _T("WindowPlacementMainFrame");  ///< Window placement for main frame

AppSettings::AppSettings(LPCTSTR pszSettingsRegkey)
:m_pszSettingsRegkey(pszSettingsRegkey)
{
}

void AppSettings::Store()
{
   // open root key and try to create key
   CRegKey regRoot;
   if (ERROR_SUCCESS != regRoot.Open(HKEY_CURRENT_USER, m_pszSettingsRegkey) &&
      ERROR_SUCCESS != regRoot.Create(HKEY_CURRENT_USER, m_pszSettingsRegkey))
      return;

   m_windowPlacementMainFrame.Store(regRoot, g_pszWindowPlacementMainFrame);
}

void AppSettings::Load()
{
   // open root key
   CRegKey regRoot;
   if (ERROR_SUCCESS != regRoot.Open(HKEY_CURRENT_USER, m_pszSettingsRegkey, KEY_READ))
      return;

   m_windowPlacementMainFrame.Load(regRoot, g_pszWindowPlacementMainFrame);
}
