//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file CameraFileSystemView.hpp View for camera file system
//
#pragma once

#include "IPhotoModeView.hpp"
#include "CameraFileSystemTreeView.hpp"
#include "CameraFileSystemFileListView.hpp"

class IPhotoModeViewHost;

/// camera file system view
class CameraFileSystemView :
   public CSplitterWindowImpl<CameraFileSystemView>,
   public IPhotoModeView
{
public:
   /// ctor
   CameraFileSystemView(IPhotoModeViewHost& host)
      :m_host(host),
      m_treeView(m_listView),
      m_listView(host, m_treeView)
   {
   }

   /// dtor
   virtual ~CameraFileSystemView()
   {
   }

private:
   // message map
   BEGIN_MSG_MAP(CameraFileSystemView)
      CHAIN_MSG_MAP_MEMBER(m_treeView)
      CHAIN_MSG_MAP_MEMBER(m_listView)
      CHAIN_MSG_MAP(CSplitterWindowImpl<CameraFileSystemView>)
      REFLECT_NOTIFICATIONS() // to make sure superclassed controls get notification messages
   END_MSG_MAP()

   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hWndParent) override;

   virtual BOOL PreTranslateMessage(MSG* pMsg) override
   {
      UNUSED(pMsg);
      return FALSE;
   }

   virtual void DestroyView() override
   {
      ATLVERIFY(TRUE == DestroyWindow());
   }

private:
   // model

   /// host access
   IPhotoModeViewHost& m_host;

   /// camera file system
   std::shared_ptr<CameraFileSystem> m_cameraFileSystem;

   // UI

   /// vertical splitter
   CSplitterWindow m_splitterWindow;

   /// tree view showing the file system tree
   CameraFileSystemTreeView m_treeView;

   /// list view showing the current list of files
   CameraFileSystemFileListView m_listView;
};
