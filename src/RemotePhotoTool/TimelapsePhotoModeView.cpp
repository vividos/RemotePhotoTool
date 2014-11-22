//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file TimeLapsePhotoModeView.cpp Timelapse photo mode view
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "TimeLapsePhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"

TimeLapsePhotoModeView::TimeLapsePhotoModeView(IPhotoModeViewHost& host) throw()
:m_host(host)
{
}

LRESULT TimeLapsePhotoModeView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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

LRESULT TimeLapsePhotoModeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // note: don't reset default release settings; must be done in new view

   return 0;
}
