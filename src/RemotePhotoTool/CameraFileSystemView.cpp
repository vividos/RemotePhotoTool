//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file CameraFileSystemView.cpp View for device properties
//

// includes
#include "stdafx.h"
#include "CameraFileSystemView.hpp"
#include "SourceDevice.hpp"
#include "IPhotoModeViewHost.hpp"
#include "CameraFileSystem.hpp"
#include "CameraErrorDlg.hpp"

HWND CameraFileSystemView::CreateView(HWND hWndParent)
{
   try
   {
      m_cameraFileSystem = m_host.GetSourceDevice()->GetFileSystem();
   }
   catch (const CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't open camera file system"), ex);
      dlg.DoModal(m_hWnd);

      return nullptr;
   }

   HWND hWndClient = Create(hWndParent, CWindow::rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   m_treeView.Create(hWndClient, CWindow::rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);

   m_listView.Create(hWndClient, CWindow::rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);

   SetSplitterPanes(m_treeView, m_listView);
   SetSinglePaneMode(SPLIT_PANE_NONE);

   SetSplitterPosPct(20);

   m_listView.Init(m_cameraFileSystem);
   m_treeView.Init(m_cameraFileSystem);

   m_treeView.RefreshTree();
   m_treeView.NavigateToPath(CameraFileSystem::PathSeparator);
   m_listView.NavigateToPath(CameraFileSystem::PathSeparator);

   return hWndClient;
}
