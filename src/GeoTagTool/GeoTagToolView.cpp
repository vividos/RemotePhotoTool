//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file GeoTagToolView.cpp View for GeoTagTool
//
#include "stdafx.h"
#include "res/Ribbon.h"
#include "resource.h"
#include "GeoTagToolView.hpp"

BOOL GeoTagToolView::PreTranslateMessage(MSG* pMsg)
{
   return CWindow::IsDialogMessage(pMsg);
}

void GeoTagToolView::OnStartingGPSReceiver(GPS::Receiver& gpsReceiver)
{
   UNUSED(gpsReceiver);
}

LRESULT GeoTagToolView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   return 0;
}
