//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PhotoStackingPhotoModeView.cpp Photo stacking photo mode view
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "PhotoStackingPhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"

PhotoStackingPhotoModeView::PhotoStackingPhotoModeView(IPhotoModeViewHost& host) throw()
:m_host(host)
{
}

LRESULT PhotoStackingPhotoModeView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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

LRESULT PhotoStackingPhotoModeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // note: don't reset default release settings; must be done in new view

   return 0;
}
