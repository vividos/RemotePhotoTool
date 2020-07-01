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

void CameraFileSystemFileListView::Init(std::shared_ptr<CameraFileSystem> cameraFileSystem)
{
   m_cameraFileSystem = cameraFileSystem;

   InsertColumn(T_enFileListViewColumns::columnFilename, _T("Name"), LVCFMT_LEFT, 300);
   InsertColumn(T_enFileListViewColumns::columnFilename, _T("Size"), LVCFMT_LEFT, 100);
   InsertColumn(T_enFileListViewColumns::columnFilename, _T("Modified"), LVCFMT_LEFT, 150);

   DWORD dwExStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
   SetExtendedListViewStyle(dwExStyle, dwExStyle);

   ATLASSERT((GetStyle() & LVS_SHAREIMAGELISTS) != 0);
   SetImageList(SystemImageList::Get(true), LVSIL_SMALL);

   m_host.EnableUI(ID_FILESYSTEM_DOWNLOAD, false);
}

void CameraFileSystemFileListView::RefreshList()
{
   SetRedraw(FALSE);

   DeleteAllItems();

   std::vector<FileInfo> fileInfoList = m_cameraFileSystem->EnumFiles(m_currentPath);

   int itemCount = 0;
   for (const FileInfo& fileInfo : fileInfoList)
   {
      int itemIndex = InsertItem(itemCount++, fileInfo.m_filename, SystemImageList::IndexFromFilename(fileInfo.m_filename));

      CString sizeText;
      sizeText.Format(_T("%u"), fileInfo.m_fileSize);
      for (int charIndex = sizeText.GetLength() - 3; charIndex >= 0; charIndex -= 3)
         sizeText.Insert(charIndex, _T("."));

      SetItemText(itemIndex, 1, sizeText);

      CString timeText{ _T("???") };
      if (fileInfo.m_modifiedTime != -1)
      {
         struct tm modifiedTime = {};
         if (0 == localtime_s(&modifiedTime, &fileInfo.m_modifiedTime))
         {
            _tcsftime(timeText.GetBuffer(32), 32, _T("%Y-%m-%d %H:%M:%S"), &modifiedTime);
            timeText.ReleaseBuffer();
         }
      }

      SetItemText(itemIndex, 2, timeText);

      SetItemData(itemIndex, itemIndex);
   }

   SetRedraw(TRUE);
}

void CameraFileSystemFileListView::NavigateToPath(const CString& path)
{
   m_currentPath = path;

   RefreshList();
}

LRESULT CameraFileSystemFileListView::OnItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
   bool isAnyItemSelected = GetSelectedCount();
   m_host.EnableUI(ID_FILESYSTEM_DOWNLOAD, isAnyItemSelected);

   return 0;
}

LRESULT CameraFileSystemFileListView::OnFileSystemDownload(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   // TODO start download
}
