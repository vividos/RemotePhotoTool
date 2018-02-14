//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file CameraFileSystemTreeView.cpp Tree view for camera file system
//
#include "stdafx.h"
#include "resource.h"
#include "CameraFileSystemTreeView.hpp"
#include "CameraFileSystemFileListView.hpp"
#include "CameraFileSystem.hpp"
#include "Icons.hpp"

void CameraFileSystemTreeView::Init(std::shared_ptr<CameraFileSystem> cameraFileSystem)
{
   m_cameraFileSystem = cameraFileSystem;

   // load image list
   {
      CBitmap bmpIcons;
      bmpIcons.LoadBitmap(IDB_ICONS);

      m_iconList.Create(16, 16, ILC_MASK | ILC_COLOR32, 0, 0);
      m_iconList.Add(bmpIcons, RGB(255, 0, 255));

      SetImageList(m_iconList, TVSIL_NORMAL);
   }
}

void CameraFileSystemTreeView::RefreshTree()
{
   SetRedraw(FALSE);

   DeleteAllItems();
   m_mapTreeItemToPath.clear();
   m_mapPathToTreeItem.clear();

   HTREEITEM rootItem = CTreeViewCtrl::InsertItem(
      CameraFileSystem::PathSeparator,
      T_enSmallIconIndex::iconSmallFolderClosed,
      T_enSmallIconIndex::iconSmallFolderOpen,
      TVI_ROOT, TVI_LAST);

   m_mapTreeItemToPath.insert(std::make_pair(rootItem, CameraFileSystem::PathSeparator));
   m_mapPathToTreeItem.insert(std::make_pair(CameraFileSystem::PathSeparator, rootItem));

   RecursivelyAddTreeItems(rootItem, CameraFileSystem::PathSeparator);

   Expand(rootItem, TVE_EXPAND);

   SetRedraw(TRUE);
}

LRESULT CameraFileSystemTreeView::OnSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   LPNMTREEVIEW pnmtreeview = (LPNMTREEVIEW)pnmh;

   HTREEITEM selectedItem = pnmtreeview->itemNew.hItem;

   auto iter = m_mapTreeItemToPath.find(selectedItem);
   if (iter != m_mapTreeItemToPath.end())
   {
      CString path = iter->second;
      m_listView.NavigateToPath(path);
      NavigateToPath(path);
   }

   return 0;
}

void CameraFileSystemTreeView::RecursivelyAddTreeItems(HTREEITEM parentItem, const CString& path)
{
   std::vector<CString> allFolders = m_cameraFileSystem->EnumFolders(path);

   for (const CString folderName : allFolders)
   {
      HTREEITEM item = CTreeViewCtrl::InsertItem(
         folderName,
         T_enSmallIconIndex::iconSmallFolderClosed,
         T_enSmallIconIndex::iconSmallFolderOpen,
         parentItem,
         TVI_LAST);

      CString subPath = path;
      if (path != CameraFileSystem::PathSeparator)
         subPath += CameraFileSystem::PathSeparator;
      subPath += folderName;

      m_mapTreeItemToPath.insert(std::make_pair(item, subPath));
      m_mapPathToTreeItem.insert(std::make_pair(subPath, item));

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
