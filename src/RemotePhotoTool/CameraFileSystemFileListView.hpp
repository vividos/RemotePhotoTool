//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file CameraFileSystemFileListView.hpp File list view for camera file system
//
#pragma once

#include "resource.h"
#include <CameraFileSystem.hpp>

class IPhotoModeViewHost;
class CameraFileSystemTreeView;

/// win traits for camera file system file list view
typedef CWinTraitsOR<LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS, WS_EX_CLIENTEDGE, CControlWinTraits>
CameraFileSystemFileListViewWinTraits;

/// \brief list view showing the file list of the camera file system
class CameraFileSystemFileListView :
   public CWindowImpl<CameraFileSystemFileListView, CListViewCtrl, CameraFileSystemFileListViewWinTraits>
{
   /// base class type
   typedef CWindowImpl<CameraFileSystemFileListView, CListViewCtrl, CameraFileSystemFileListViewWinTraits> BaseClass;

public:
   /// ctor
   CameraFileSystemFileListView(IPhotoModeViewHost& host, CameraFileSystemTreeView& treeView)
      :m_host(host),
      m_treeView(treeView)
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
      columnFilename = 0,     ///< filename column
      columnSize = 1,         ///< size column
      columnModifiedDate = 2, ///< modified date column
   };

private:
   BEGIN_MSG_MAP(CameraFileSystemFileListView)
      NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnItemChanged)
      COMMAND_ID_HANDLER(ID_FILESYSTEM_DOWNLOAD, OnFileSystemDownload)
      NOTIFY_CODE_HANDLER(LVN_BEGINDRAG, OnBeginDrag)
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   // LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)

   /// called when the item (selection) in the list view has changed
   LRESULT OnItemChanged(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

   /// called when the "download" button on the file system ribbon is being selected
   LRESULT OnFileSystemDownload(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when a file is started to be dragged, e.g. out of this app
   LRESULT OnBeginDrag(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

   /// downloads all files in the background
   void DownloadFiles(const std::vector<FileInfo>& fileInfoList);

   /// called when download has finished
   void OnDownloadFinished(const FileInfo& fileInfo, const std::vector<unsigned char>& data);

private:
   // UI

   /// file system tree view
   CameraFileSystemTreeView& m_treeView;

   // model

   /// host access
   IPhotoModeViewHost& m_host;

   /// camera file system
   std::shared_ptr<CameraFileSystem> m_cameraFileSystem;

   /// current path
   CString m_currentPath;

   /// mapping from list view item index to file info object
   std::map<int, FileInfo> m_mapItemIndexToFileInfo;
};
