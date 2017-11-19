//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file CameraFileSystemTreeView.hpp Tree view for camera file system
//
#pragma once

class IPhotoModeViewHost;
class CameraFileSystemFileListView;

/// win traits for camera file system tree view
typedef CWinTraitsOR<TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE | TVS_EX_DOUBLEBUFFER, CControlWinTraits>
CameraFileSystemTreeViewWinTraits;

class CameraFileSystemTreeView :
   public CWindowImpl<CameraFileSystemTreeView, CTreeViewCtrl, CameraFileSystemTreeViewWinTraits>
{
public:
   CameraFileSystemTreeView(CameraFileSystemFileListView& listView)
      :m_listView(listView)
   {
   }

   DECLARE_WND_SUPERCLASS(NULL, CTreeViewCtrl::GetWndClassName())

   /// initializes tree
   void Init(std::shared_ptr<CameraFileSystem> cameraFileSystem);

   /// refreshes camera file system tree
   void RefreshTree();

   /// navigates to new path
   void NavigateToPath(const CString& path);

private:
   BEGIN_MSG_MAP(CameraFileSystemFileListView)
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   // LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)

   /// recursively adds folder items from a path to the parent tree item
   void RecursivelyAddTreeItems(HTREEITEM parentItem, const CString& path);

private:
   // UI

   /// access to file list view
   CameraFileSystemFileListView& m_listView;

   // model

   /// camera file system
   std::shared_ptr<CameraFileSystem> m_cameraFileSystem;

   /// current path
   CString m_currentPath;

   /// mapping from path name to tree item; path name is without ending slash
   std::map<CString, HTREEITEM> m_mapPathToTreeItem;
};
