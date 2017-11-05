//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file PhotoStackingPhotoModeView.cpp Photo stacking photo mode view
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "PhotoStackingPhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"

PhotoStackingPhotoModeView::PhotoStackingPhotoModeView(IPhotoModeViewHost& host)
:m_host(host)
{
}

LRESULT PhotoStackingPhotoModeView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   m_spRemoteReleaseControl = m_host.GetRemoteReleaseControl();

   return TRUE;
}

LRESULT PhotoStackingPhotoModeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // note: don't reset default release settings; must be done in new view

   return 0;
}
