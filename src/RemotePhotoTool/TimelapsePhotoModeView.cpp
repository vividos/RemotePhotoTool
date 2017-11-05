//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file TimeLapsePhotoModeView.cpp Timelapse photo mode view
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "TimeLapsePhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"

TimeLapsePhotoModeView::TimeLapsePhotoModeView(IPhotoModeViewHost& host)
:m_host(host)
{
}

LRESULT TimeLapsePhotoModeView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   m_spRemoteReleaseControl = m_host.GetRemoteReleaseControl();

   return TRUE;
}

LRESULT TimeLapsePhotoModeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // note: don't reset default release settings; must be done in new view

   return 0;
}
