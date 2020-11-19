//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file WiaCameraFileSystemImpl.cpp WIA - CameraFileSystem impl
//
#include "stdafx.h"
#include "WiaCameraFileSystemImpl.hpp"
#include "WiaPropertyAccess.hpp"
#include "WiaDataCallback.hpp"
#include <ulib/win32/ErrorMessage.hpp>

using namespace WIA;

namespace
{
   /// enumerator for child items of an IWiaItem
   class ChildItemEnumerator
   {
   public:
      /// ctor; takes parent handle
      ChildItemEnumerator(CComPtr<IWiaItem> parent)
         :m_parent(parent)
      {
         HRESULT hr = parent->EnumChildItems(&m_enumWiaItem);
         CheckError(_T("IWiaItem::EnumChildItems"), hr, __FILE__, __LINE__);
      }

      /// returns child item by given index
      CComPtr<IWiaItem> Next()
      {
         CComPtr<IWiaItem> childItem;

         HRESULT hr = m_enumWiaItem->Next(1, &childItem, nullptr);
         CheckError(_T("IEnumWiaItem::Next"), hr, __FILE__, __LINE__);

         if (hr == S_FALSE)
            childItem = nullptr;

         return childItem;
      }

   private:
      /// parent item
      CComPtr<IWiaItem> m_parent;

      /// enumeration object
      CComPtr<IEnumWiaItem> m_enumWiaItem;
   };

} // anonymous namespace

std::vector<CString> CameraFileSystemImpl::EnumFolders(const CString& path) const
{
   CComPtr<IWiaItem> item = FollowPath(m_wiaDeviceRootItem, path);

   if (item == nullptr)
      return std::vector<CString>();

   return EnumFolders(item);
}

std::vector<FileInfo> CameraFileSystemImpl::EnumFiles(const CString& path) const
{
   CComPtr<IWiaItem> item = FollowPath(m_wiaDeviceRootItem, path);

   if (item == nullptr)
      return std::vector<FileInfo>();

   return EnumFiles(path, item);
}

void CameraFileSystemImpl::StartDownload(const FileInfo& fileInfo, T_fnDownloadFinished fnDownloadFinished)
{
   CComPtr<IWiaItem> item = FollowPath(m_wiaDeviceRootItem, fileInfo.m_filename);
   if (item == nullptr)
      return; // file not available anymore

   CComQIPtr<IWiaDataTransfer> transfer{ item };
   if (transfer == nullptr)
      return; // transfer not possible

   // the object frees itself when the refcount goes to zero
   WiaDataCallback* callback = new WiaDataCallback{ fileInfo, fnDownloadFinished };

   WIA_DATA_TRANSFER_INFO transferInfo = {};
   transferInfo.ulSize = sizeof(WIA_DATA_TRANSFER_INFO);
   transferInfo.ulSection = 0;
   transferInfo.bDoubleBuffer = true;

   HRESULT hr = transfer->idtGetBandedData(&transferInfo, callback);

   if (FAILED(hr))
   {
      throw CameraException(
         _T("WIA::CameraFileSystemImpl::StartDownload"),
         Win32::ErrorMessage(hr).ToString(),
         hr,
         __FILE__,
         __LINE__);
   }
}

CComPtr<IWiaItem> CameraFileSystemImpl::FollowPath(CComPtr<IWiaItem> baseItem, const CString& path) const
{
   if (path == PathSeparator)
      return baseItem;

   CComPtr<IWiaItem> currentItem = baseItem;

   int currentIndex = 0;
   do
   {
      CString pathPart = path.Tokenize(PathSeparator, currentIndex);

      if (pathPart == _T("."))
         continue;

      if (!pathPart.IsEmpty())
      {
         currentItem = OpenChildByName(currentItem, pathPart);
      }
   } while (currentIndex >= 0 && currentIndex < path.GetLength() && currentItem != nullptr);

   return currentItem;
}

CComPtr<IWiaItem> CameraFileSystemImpl::OpenChildByName(CComPtr<IWiaItem> parentItem, const CString& pathPart) const
{
   ChildItemEnumerator itemEnumerator{ parentItem };

   CComPtr<IWiaItem> childItem;
   while ((childItem = itemEnumerator.Next()) != nullptr)
   {
      LONG itemType = 0;
      HRESULT hr = childItem->GetItemType(&itemType);
      CheckError(_T("IWiaItem::GetItemType"), hr, __FILE__, __LINE__);

      if ((itemType & WiaItemTypeFolder) != 0)
      {
         PropertyAccess access{ childItem };
         CString filename = access.Get(WIA_IPA_ITEM_NAME);

         if (pathPart == filename)
            return childItem;
      }

      if ((itemType & WiaItemTypeFile) != 0)
      {
         PropertyAccess access{ childItem };
         CString filename = access.Get(WIA_IPA_ITEM_NAME) + _T(".") + access.Get(WIA_IPA_FILENAME_EXTENSION);

         if (pathPart == filename)
            return childItem;
      }
   }

   return CComPtr<IWiaItem>();
}

std::vector<CString> CameraFileSystemImpl::EnumFolders(CComPtr<IWiaItem> item) const
{
   std::vector<CString> allFoldersList;

   ChildItemEnumerator itemEnumerator(item);

   CComPtr<IWiaItem> childItem;
   while ((childItem = itemEnumerator.Next()) != nullptr)
   {
      LONG itemType = 0;
      HRESULT hr = childItem->GetItemType(&itemType);
      CheckError(_T("IWiaItem::GetItemType"), hr, __FILE__, __LINE__);

      if ((itemType & WiaItemTypeFolder) != 0)
      {
         PropertyAccess access(childItem);
         CString filename = access.Get(WIA_IPA_ITEM_NAME);

         allFoldersList.push_back(filename);
      }
   }

   return allFoldersList;
}

std::vector<FileInfo> CameraFileSystemImpl::EnumFiles(const CString& path, CComPtr<IWiaItem> item) const
{
   std::vector<FileInfo> allFileInfoList;

   ChildItemEnumerator itemEnumerator{ item };

   CComPtr<IWiaItem> childItem;
   while ((childItem = itemEnumerator.Next()) != nullptr)
   {
      LONG itemType = 0;
      HRESULT hr = childItem->GetItemType(&itemType);
      CheckError(_T("IWiaItem::GetItemType"), hr, __FILE__, __LINE__);

      if ((itemType & WiaItemTypeFile) != 0)
      {
         PropertyAccess access(childItem);

         FileInfo fileInfo;
         fileInfo.m_filename = path + access.Get(WIA_IPA_ITEM_NAME) + _T(".") + access.Get(WIA_IPA_FILENAME_EXTENSION);
         fileInfo.m_fileSize = access.GetUint(WIA_IPA_ITEM_SIZE);
         fileInfo.m_modifiedTime = access.GetSystemTime(WIA_IPA_ITEM_TIME);

         allFileInfoList.push_back(fileInfo);
      }
   }

   return allFileInfoList;
}
