//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file CameraFileSystemFileListView.hpp File list view for camera file system
//
#pragma once

class IPhotoModeViewHost;
class CameraFileSystemTreeView;

/// win traits for camera file system file list view
typedef CWinTraitsOR<LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE, CControlWinTraits>
CameraFileSystemFileListViewWinTraits;

/// \brief list view showing the file list of the camera file system
class CameraFileSystemFileListView :
   public CWindowImpl<CameraFileSystemFileListView, CListViewCtrl, CameraFileSystemFileListViewWinTraits>
{
   /// base class type
   typedef CWindowImpl<CameraFileSystemFileListView, CListViewCtrl, CameraFileSystemFileListViewWinTraits> BaseClass;

public:
   /// ctor
   CameraFileSystemFileListView(CameraFileSystemTreeView& treeView)
      :m_treeView(treeView)
   {
   }

   DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

   /// inits file list
   void Init(std::shared_ptr<CameraFileSystem> cameraFileSystem);

   /// refreshes camera file system file list
   void RefreshList();

   /// navigates to new path
   void NavigateToPath(const CString& path);

private:
   /// column indices
   enum T_enFileListViewColumns
   {
      columnFilename = 0,  ///< filename column
   };

private:
   BEGIN_MSG_MAP(CameraFileSystemFileListView)
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   // LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)

private:
   // UI

   /// file system tree view
   CameraFileSystemTreeView& m_treeView;

   // model

   /// camera file system
   std::shared_ptr<CameraFileSystem> m_cameraFileSystem;

   /// current path
   CString m_currentPath;
};
