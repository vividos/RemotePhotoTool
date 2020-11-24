//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file Gphoto2CameraFileSystemImpl.cpp gPhoto2 - CameraFileSystem impl
//
#pragma once

#include "stdafx.h"
#include "Gphoto2CameraFileSystemImpl.hpp"
#include "Gphoto2Include.hpp"

using namespace GPhoto2;

std::vector<CString> CameraFileSystemImpl::EnumFolders(const CString& path) const
{
   std::vector<CString> folderList;

   CameraList* list = nullptr;
   int ret = gp_list_new(&list);
   if (ret < GP_OK)
      return folderList;

   std::shared_ptr<CameraList> spAutoFreeFolderList(list, gp_list_free);

   CStringA folder(path);
   if (folder == ".")
      folder = "/";
   if (folder.Left(1) == ".")
      folder.TrimLeft('.');

   ret = gp_camera_folder_list_folders(m_spCamera.get(), folder, list, m_spContext.get());

   if (ret < GP_OK)
      return folderList;

   int count = gp_list_count(list);

   for (int index = 0; index < count; index++)
   {
      const char* name = nullptr;
      ret = gp_list_get_name(list, index, &name);
      if (ret < GP_OK)
         break;

      folderList.push_back(CString(name));
   }

   return folderList;
}

std::vector<FileInfo> CameraFileSystemImpl::EnumFiles(const CString& path) const
{
   std::vector<FileInfo> fileList;

   CameraList* list = nullptr;
   int ret = gp_list_new(&list);
   if (ret < GP_OK)
      return fileList;

   std::shared_ptr<CameraList> spAutoFreeFileList(list, gp_list_free);

   CStringA folder(path);
   if (folder == ".")
      folder = "/";
   if (folder.Left(1) == ".")
      folder.TrimLeft('.');

   ret = gp_camera_folder_list_files(m_spCamera.get(), folder, list, m_spContext.get());

   if (ret < GP_OK)
      return fileList;

   int count = gp_list_count(list);

   for (int index = 0; index < count; index++)
   {
      const char* name = nullptr;
      ret = gp_list_get_name(list, index, &name);
      if (ret < GP_OK)
         break;

      FileInfo info;
      info.m_filename = CString(folder) + "/" + CString(name);

      CameraFileInfo cameraFileInfo = {};
      cameraFileInfo.file.fields = CameraFileInfoFields(GP_FILE_INFO_SIZE | GP_FILE_INFO_MTIME);
      cameraFileInfo.preview.fields = GP_FILE_INFO_NONE;
      cameraFileInfo.audio.fields = GP_FILE_INFO_NONE;
      ret = gp_camera_file_get_info(m_spCamera.get(), folder, name, &cameraFileInfo, m_spContext.get());
      if (ret < GP_OK)
         continue;

      info.m_fileSize = static_cast<unsigned long>(cameraFileInfo.file.size);
      info.m_modifiedTime = cameraFileInfo.file.mtime;

      fileList.push_back(info);
   }

   return fileList;
}

void CameraFileSystemImpl::StartDownload(const FileInfo& fileInfo, T_fnDownloadFinished fnDownloadFinished)
{
   int pos = fileInfo.m_filename.ReverseFind(_T('/'));
   CStringA folder;
   if (pos != -1)
      folder = fileInfo.m_filename.Left(pos);

   CStringA name = fileInfo.m_filename.Mid(pos + 1);

   std::vector<unsigned char> buffer;
   buffer.resize(fileInfo.m_fileSize);

   uint64_t fileSize = fileInfo.m_fileSize;

   int ret = gp_camera_file_read(
      m_spCamera.get(), folder, name, GP_FILE_TYPE_NORMAL,
      0,
      reinterpret_cast<char*>(buffer.data()),
      &fileSize,
      m_spContext.get());

   CheckError(_T("CameraFileSystemImpl::StartDownload"), ret, __FILE__, __LINE__);

   if (fnDownloadFinished != nullptr)
      fnDownloadFinished(fileInfo, buffer);
}
