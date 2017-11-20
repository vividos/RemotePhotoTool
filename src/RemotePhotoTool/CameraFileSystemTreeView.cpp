//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file CameraFileSystemTreeView.cpp Tree view for camera file system
//
#include "stdafx.h"
#include "CameraFileSystemTreeView.hpp"
#include "CameraFileSystem.hpp"

void CameraFileSystemTreeView::Init(std::shared_ptr<CameraFileSystem> cameraFileSystem)
{
   m_cameraFileSystem = cameraFileSystem;
}

void CameraFileSystemTreeView::RefreshTree()
{
   SetRedraw(FALSE);

   DeleteAllItems();

   HTREEITEM rootItem = CTreeViewCtrl::InsertItem(CameraFileSystem::PathSeparator, TVI_ROOT, TVI_LAST);

   RecursivelyAddTreeItems(rootItem, CameraFileSystem::PathSeparator);

   Expand(rootItem, TVE_EXPAND);

   SetRedraw(TRUE);
}

void CameraFileSystemTreeView::RecursivelyAddTreeItems(HTREEITEM parentItem, const CString& path)
{
   std::vector<CString> allFolders = m_cameraFileSystem->EnumFolders(path);

   for (const CString folderName : allFolders)
   {
      HTREEITEM item = CTreeViewCtrl::InsertItem(folderName, parentItem, TVI_LAST);

      CString subPath = path + CameraFileSystem::PathSeparator + folderName;

      RecursivelyAddTreeItems(item, subPath);
   }
}

void CameraFileSystemTreeView::NavigateToPath(const CString& path)
{
   m_currentPath = path;

   auto iter = m_mapPathToTreeItem.find(path);
   if (iter == m_mapPathToTreeItem.end())
      return;

   CTreeViewCtrl::SelectItem(iter->second);
}
