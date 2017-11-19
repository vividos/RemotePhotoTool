//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file CameraFileSystemFileListView.cpp File list view for camera file system
//
#include "stdafx.h"
#include "CameraFileSystemFileListView.hpp"
#include <CameraFileSystem.hpp>

void CameraFileSystemFileListView::Init(std::shared_ptr<CameraFileSystem> cameraFileSystem)
{
   m_cameraFileSystem = cameraFileSystem;

   InsertColumn(T_enFileListViewColumns::columnFilename, _T("Name"), LVCFMT_LEFT, 300);

   DWORD dwExStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
   SetExtendedListViewStyle(dwExStyle, dwExStyle);
}

void CameraFileSystemFileListView::RefreshList()
{
   SetRedraw(FALSE);

   DeleteAllItems();

   std::vector<FileInfo> fileInfoList = m_cameraFileSystem->EnumFiles(m_currentPath);

   int itemCount = 0;
   for (const FileInfo& fileInfo : fileInfoList)
   {
      // TODO int imageIndex = 0;
      InsertItem(itemCount++, fileInfo.m_filename);
   }

   SetRedraw(TRUE);
}

void CameraFileSystemFileListView::NavigateToPath(const CString& path)
{
   m_currentPath = path;

   RefreshList();
}
