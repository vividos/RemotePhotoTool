//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file CameraFileSystemView.cpp View for device properties
//

// includes
#include "stdafx.h"
#include "CameraFileSystemView.hpp"
#include "SourceDevice.hpp"
#include "IPhotoModeViewHost.hpp"
#include "CameraFileSystem.hpp"

HWND CameraFileSystemView::CreateView(HWND hWndParent)
{
   HWND hWndClient = Create(hWndParent, CWindow::rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   m_treeView.Create(hWndClient, CWindow::rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);

   m_listView.Create(hWndClient, CWindow::rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);

   SetSplitterPanes(m_treeView, m_listView);
   SetSinglePaneMode(SPLIT_PANE_NONE);

   SetSplitterPosPct(20);

   m_cameraFileSystem = m_host.GetSourceDevice()->GetFileSystem();

   m_listView.Init(m_cameraFileSystem);
   m_treeView.Init(m_cameraFileSystem);

   m_treeView.RefreshTree();
   m_treeView.NavigateToPath(CameraFileSystem::PathSeparator);
   m_listView.NavigateToPath(CameraFileSystem::PathSeparator);

   return hWndClient;
}
