//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ScriptingPhotoModeView.cpp Scripting photo mode view
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "ScriptingPhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"

ScriptingPhotoModeView::ScriptingPhotoModeView(IPhotoModeViewHost& host) throw()
:m_host(host)
{
}

LRESULT ScriptingPhotoModeView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   m_spRemoteReleaseControl = m_host.StartRemoteReleaseControl(true);
   if (m_spRemoteReleaseControl == nullptr)
   {
      AtlMessageBox(m_hWnd, _T("Couldn't start remote release control."), IDR_MAINFRAME, MB_OK);
      DestroyWindow();
      return 0;
   }

   return TRUE;
}

LRESULT ScriptingPhotoModeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   return 0;
}
