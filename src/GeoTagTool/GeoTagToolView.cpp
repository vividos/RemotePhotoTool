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
