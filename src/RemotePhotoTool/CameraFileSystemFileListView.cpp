//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file CameraFileSystemFileListView.cpp File list view for camera file system
//
#include "stdafx.h"
#include "CameraFileSystemFileListView.hpp"
#include "SystemImageList.hpp"
#include "IPhotoModeViewHost.hpp"
#include <CameraFileSystem.hpp>

void CameraFileSystemFileListView::Init(std::shared_ptr<CameraFileSystem> cameraFileSystem)
{
   m_cameraFileSystem = cameraFileSystem;

   InsertColumn(T_enFileListViewColumns::columnFilename, _T("Name"), LVCFMT_LEFT, 300);

   DWORD dwExStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
   SetExtendedListViewStyle(dwExStyle, dwExStyle);

   ATLASSERT((GetStyle() & LVS_SHAREIMAGELISTS) != 0);
   SetImageList(SystemImageList::Get(true), LVSIL_SMALL);
}

void CameraFileSystemFileListView::RefreshList()
{
   SetRedraw(FALSE);

   DeleteAllItems();

   std::vector<FileInfo> fileInfoList = m_cameraFileSystem->EnumFiles(m_currentPath);

   int itemCount = 0;
   for (const FileInfo& fileInfo : fileInfoList)
   {
      InsertItem(itemCount++, fileInfo.m_filename, SystemImageList::IndexFromFilename(fileInfo.m_filename));
   }

   SetRedraw(TRUE);
}

void CameraFileSystemFileListView::NavigateToPath(const CString& path)
{
   m_currentPath = path;

   RefreshList();
}
