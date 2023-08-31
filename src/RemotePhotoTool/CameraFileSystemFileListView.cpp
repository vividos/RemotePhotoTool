//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file CameraFileSystemFileListView.cpp File list view for camera file system
//
#include "stdafx.h"
#include "CameraFileSystemFileListView.hpp"
#include "CameraFileSystemDropSource.hpp"
#include "SystemImageList.hpp"
#include "IPhotoModeViewHost.hpp"
#include "CameraErrorDlg.hpp"
#include "ImageFileManager.hpp"
#include <ulib/Path.hpp>
#include "File.hpp"

void CameraFileSystemFileListView::Init(std::shared_ptr<CameraFileSystem> cameraFileSystem)
{
   m_cameraFileSystem = cameraFileSystem;

   InsertColumn(T_enFileListViewColumns::columnFilename, _T("Name"), LVCFMT_LEFT, 300);
   InsertColumn(T_enFileListViewColumns::columnSize, _T("Size"), LVCFMT_LEFT, 100);
   InsertColumn(T_enFileListViewColumns::columnModifiedDate, _T("Modified"), LVCFMT_LEFT, 150);

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

   m_mapItemIndexToFileInfo.clear();

   std::vector<FileInfo> fileInfoList = m_cameraFileSystem->EnumFiles(m_currentPath);

   int itemCount = 0;
   for (const FileInfo& fileInfo : fileInfoList)
   {
      CString filename = fileInfo.m_filename;
      int pos = filename.ReverseFind(CameraFileSystem::PathSeparator[0]);
      if (pos != -1)
         filename = filename.Mid(pos + 1);

      int itemIndex = InsertItem(itemCount++, filename, SystemImageList::IndexFromFilename(fileInfo.m_filename));

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

      m_mapItemIndexToFileInfo[itemIndex] = fileInfo;
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
   std::vector<FileInfo> fileInfoList;

   // get all selected files
   for (int itemIndex = GetNextItem(-1, LVNI_SELECTED); itemIndex >= 0; itemIndex = GetNextItem(itemIndex, LVNI_SELECTED))
   {
      size_t fileInfoIndex = GetItemData(itemIndex);

      fileInfoList.push_back(m_mapItemIndexToFileInfo[fileInfoIndex]);
   }

   DownloadFiles(fileInfoList);

   return 0;
}

LRESULT CameraFileSystemFileListView::OnBeginDrag(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
   OleInitialize(nullptr);

   CComObjectStackEx<CameraFileSystemDropSource> dropSource;

   CameraFileSystemDraggedFilesInfo draggedFilesInfo;
   draggedFilesInfo.m_cameraFileSystem = m_cameraFileSystem;

   // get all selected files
   std::vector<FileInfo> fileInfoList;
   for (int itemIndex = GetNextItem(-1, LVNI_SELECTED); itemIndex >= 0; itemIndex = GetNextItem(itemIndex, LVNI_SELECTED))
   {
      size_t fileInfoIndex = GetItemData(itemIndex);

      const FileInfo& fileInfo = m_mapItemIndexToFileInfo[fileInfoIndex];
      fileInfoList.push_back(fileInfo);

      draggedFilesInfo.m_cameraFileInfos.push_back(fileInfo);
   }

   if (!dropSource.Init(draggedFilesInfo))
      return 0;

   DWORD effect = DROPEFFECT_NONE;
   HRESULT hr = dropSource.DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_LINK, &effect);

   if (hr == DRAGDROP_S_DROP)
      ATLTRACE(_T("Drop succeeded"));

   dropSource.Cleanup();

   OleUninitialize();

   return 0;
}

void CameraFileSystemFileListView::DownloadFiles(const std::vector<FileInfo>& fileInfoList)
{
   CWaitCursor waitCursor;
   waitCursor.Set();

   for (auto fileInfo : fileInfoList)
   {
      try
      {
         m_cameraFileSystem->StartDownload(fileInfo,
            std::bind(&CameraFileSystemFileListView::OnDownloadFinished, this, std::placeholders::_1, std::placeholders::_2));
      }
      catch (const CameraException& ex)
      {
         waitCursor.Restore();

         CameraErrorDlg dlg(_T("Couldn't download file"), ex);
         dlg.DoModal(m_hWnd);

         break;
      }
   }

   waitCursor.Restore();
}

void CameraFileSystemFileListView::OnDownloadFinished(const FileInfo& fileInfo, const std::vector<unsigned char>& data)
{
   ImageFileManager& mgr = m_host.GetImageFileManager();

   CString nextFilename = mgr.NextFilename(T_enImageType::imageTypeNormal, fileInfo.m_modifiedTime, false);

   CString baseFolder = Path::FolderName(nextFilename);
   CString baseFilename = fileInfo.m_filename;
   int pos = baseFilename.ReverseFind(_T('/'));
   if (pos != -1)
      baseFilename = baseFilename.Mid(pos + 1);

   CString filename = Path::Combine(baseFolder, baseFilename);

   unsigned int index = 1;
   CString filenameVariation;
   while (Path::FileExists(filename))
   {
      filenameVariation.Format(_T("%s-%i%s"),
         Path::FilenameOnly(baseFilename).GetString(),
         index++,
         Path::ExtensionOnly(baseFilename).GetString());

      filename = Path::Combine(baseFolder, filenameVariation);
   }

   // store file
   File::WriteAllBytes(filename, data);

   // also set file date from camera
   File::SetModifiedTime(filename, fileInfo.m_modifiedTime);
}
