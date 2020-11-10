//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file EdsdkCameraFileSystemImpl.cpp EDSDK - CameraFileSystem impl
//
#pragma once

// includes
#include "stdafx.h"
#include "EdsdkCameraFileSystemImpl.hpp"

using namespace EDSDK;

namespace
{

   /// enumerator for child items of an EDSDK handle
   class ChildItemEnumerator
   {
   public:
      /// ctor; takes parent handle
      ChildItemEnumerator(Handle parent)
         :m_parent(parent),
         m_count(std::numeric_limits<size_t>::max())
      {
      }

      /// returns number of child items
      size_t Count() const
      {
         if (m_count == std::numeric_limits<size_t>::max())
         {
            EdsUInt32 count = 0;
            EdsError err = EdsGetChildCount(m_parent, &count);
            if (err != EDS_ERR_OK)
            {
               LOG_TRACE(_T("EdsGetChildCount(ref = %08x, count = %u) returned %08x\n"),
                  m_parent.Get(), count, err);
            }

            CheckError(_T("EdsGetChildCount"), err, __FILE__, __LINE__);

            m_count = count;
         }

         return m_count;
      }

      /// returns child item by given index
      Handle Item(size_t index)
      {
         Handle child(const_cast<Handle&>(m_parent).GetRef());

         EdsError err = EdsGetChildAtIndex(m_parent, static_cast<EdsInt32>(index), &child);
         if (err != EDS_ERR_OK)
         {
            LOG_TRACE(_T("EdsGetChildAtIndex(ref = %08x, idx = %u, &camRef = %08x) returned %08x\n"),
               m_parent.Get(), index, child.Get(), err);
         }

         EDSDK::CheckError(_T("EdsGetChildAtIndex"), err, __FILE__, __LINE__);

         return child;
      }

   private:
      /// parent handle
      Handle m_parent;

      /// number of children; lazy-init'ed
      mutable size_t m_count;
   };

} // anonymous namespace

std::vector<CString> CameraFileSystemImpl::EnumFolders(const CString& path) const
{
   unsigned int pathLevel = 0;
   Handle current = FollowPath(m_hCamera, path, pathLevel);

   if (!current.IsValid())
      return std::vector<CString>();

   return EnumFolders(current, pathLevel);
}

std::vector<FileInfo> CameraFileSystemImpl::EnumFiles(const CString& path) const
{
   unsigned int pathLevel = 0;
   Handle current = FollowPath(m_hCamera, path, pathLevel);

   if (!current.IsValid())
      return std::vector<FileInfo>();

   return EnumFiles(current, pathLevel);
}

void CameraFileSystemImpl::StartDownload(const FileInfo& fileInfo, T_fnDownloadFinished fnDownloadFinished)
{
   // TODO implement
   fileInfo;
   fnDownloadFinished;
}

Handle CameraFileSystemImpl::FollowPath(Handle baseElement, const CString& path, unsigned int& pathLevel) const
{
   Handle current = baseElement;

   int currentIndex = 0;
   do
   {
      CString pathPart = path.Tokenize(PathSeparator, currentIndex);

      if (pathPart == _T("."))
         continue;

      if (!pathPart.IsEmpty())
      {
         current = OpenChildByName(current, pathPart, pathLevel);
         pathLevel++;
      }
   } while (currentIndex >= 0 && currentIndex < path.GetLength() && current.IsValid());

   return current;
}

Handle CameraFileSystemImpl::OpenChildByName(const Handle& parentHandle, const CString& pathPart, unsigned int pathLevel) const
{
   ChildItemEnumerator itemEnumerator(parentHandle);

   for (size_t index = 0; index < itemEnumerator.Count(); index++)
   {
      Handle child = itemEnumerator.Item(index);

      if (pathLevel == 0)
      {
         EdsVolumeInfo volumeInfo = { 0 };
         EdsError err = EdsGetVolumeInfo(child, &volumeInfo);

         if (err != EDS_ERR_OK)
         {
            LOG_TRACE(_T("EdsGetVolumeInfo(ref = %08x, &label = \"%hs\") returned %08x\n"),
               child.Get(), volumeInfo.szVolumeLabel, err);
         }

         EDSDK::CheckError(_T("EdsGetVolumeInfo"), err, __FILE__, __LINE__);

         if (pathPart == volumeInfo.szVolumeLabel &&
            volumeInfo.access != kEdsAccess_Error)
         {
            return child;
         }
      }
      else
      {
         EdsDirectoryItemInfo dirItemInfo = { 0 };

         EdsError err = EdsGetDirectoryItemInfo(child, &dirItemInfo);
         if (err != EDS_ERR_OK)
         {
            LOG_TRACE(_T("EdsGetDirectoryItemInfo(ref = %08x, &filename = \"%hs\") returned %08x\n"),
               child.Get(), dirItemInfo.szFileName, err);
         }

         EDSDK::CheckError(_T("EdsGetDirectoryItemInfo"), err, __FILE__, __LINE__);

         if (dirItemInfo.isFolder == TRUE &&
            pathPart == dirItemInfo.szFileName)
         {
            return child;
         }
      }
   }

   return Handle();
}

std::vector<CString> CameraFileSystemImpl::EnumFolders(const Handle& parent, unsigned int pathLevel) const
{
   std::vector<CString> allFoldersList;

   ChildItemEnumerator itemEnumerator(parent);

   for (size_t index = 0; index < itemEnumerator.Count(); index++)
   {
      Handle child = itemEnumerator.Item(index);

      if (pathLevel == 0)
      {
         EdsVolumeInfo volumeInfo = { 0 };
         EdsError err = EdsGetVolumeInfo(child, &volumeInfo);

         if (err != EDS_ERR_OK)
         {
            LOG_TRACE(_T("EdsGetVolumeInfo(ref = %08x, &label = \"%hs\") returned %08x\n"),
               child.Get(), volumeInfo.szVolumeLabel, err);
         }

         EDSDK::CheckError(_T("EdsGetVolumeInfo"), err, __FILE__, __LINE__);

         LOG_TRACE(_T("volume %u, StorageType=%u, Access=%u, MaxCapacity=%I64u kb, FreeSpace=%I64u kb, Label=%hs\n"),
            index,
            volumeInfo.storageType,
            volumeInfo.access,
            volumeInfo.maxCapacity / 1024,
            volumeInfo.freeSpaceInBytes / 1024,
            volumeInfo.szVolumeLabel
         );

         if (volumeInfo.access != kEdsAccess_Error)
            allFoldersList.push_back(volumeInfo.szVolumeLabel);

         continue;
      }
      else
      {
         EdsDirectoryItemInfo dirItemInfo = { 0 };

         EdsError err = EdsGetDirectoryItemInfo(child, &dirItemInfo);
         if (err != EDS_ERR_OK)
         {
            LOG_TRACE(_T("EdsGetDirectoryItemInfo(ref = %08x, &filename = \"%hs\") returned %08x\n"),
               child.Get(), dirItemInfo.szFileName, err);
         }

         EDSDK::CheckError(_T("EdsGetDirectoryItemInfo"), err, __FILE__, __LINE__);

         LOG_TRACE(_T("directory %u, Size=%I64u kb, IsFolder=%s, GroupID=%u kb, Option=%u, Format=%u, DateTime=%08x, Filename=%hs\n"),
            index,
            dirItemInfo.size / 1024,
            dirItemInfo.isFolder ? _T("yes") : _T("no"),
            dirItemInfo.groupID,
            dirItemInfo.option,
            dirItemInfo.format,
            dirItemInfo.dateTime,
            dirItemInfo.szFileName
         );

         if (err == EDS_ERR_OK &&
            dirItemInfo.isFolder == TRUE)
         {
            allFoldersList.push_back(dirItemInfo.szFileName);
         }
      }
   }

   return allFoldersList;
}

std::vector<FileInfo> CameraFileSystemImpl::EnumFiles(const Handle& parent, unsigned int pathLevel) const
{
   std::vector<FileInfo> allFilesList;

   if (pathLevel == 0)
      return allFilesList; // no files on volume level

   ChildItemEnumerator itemEnumerator(parent);

   for (size_t index = 0; index < itemEnumerator.Count(); index++)
   {
      Handle child = itemEnumerator.Item(index);

      EdsDirectoryItemInfo dirItemInfo = { 0 };

      EdsError err = EdsGetDirectoryItemInfo(child, &dirItemInfo);
      if (err != EDS_ERR_OK)
      {
         LOG_TRACE(_T("EdsGetDirectoryItemInfo(ref = %08x, &filename = \"%hs\") returned %08x\n"),
            child.Get(), dirItemInfo.szFileName, err);
      }

      EDSDK::CheckError(_T("EdsGetDirectoryItemInfo"), err, __FILE__, __LINE__);

      LOG_TRACE(_T("directory %u, Size=%I64u kb, IsFolder=%s, GroupID=%u kb, Option=%u, Format=%u, DateTime=%08x, Filename=%hs\n"),
         index,
         dirItemInfo.size / 1024,
         dirItemInfo.isFolder ? _T("yes") : _T("no"),
         dirItemInfo.groupID,
         dirItemInfo.option,
         dirItemInfo.format,
         dirItemInfo.dateTime,
         dirItemInfo.szFileName
      );

      if (err == EDS_ERR_OK &&
         dirItemInfo.isFolder != TRUE)
      {
         FileInfo fileInfo;
         fileInfo.m_filename = dirItemInfo.szFileName;
         fileInfo.m_fileSize = static_cast<unsigned long>(dirItemInfo.size);
         fileInfo.m_modifiedTime = dirItemInfo.dateTime;

         allFilesList.push_back(fileInfo);
      }
   }

   return allFilesList;
}
