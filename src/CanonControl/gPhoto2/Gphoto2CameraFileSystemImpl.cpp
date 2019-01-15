//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2019 Michael Fink
//
/// \file Gphoto2CameraFileSystemImpl.cpp gPhoto2 - CameraFileSystem impl
//
#pragma once

// includes
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
      info.m_filename = CString(name);

      //CameraFileInfo cameraFileInfo;
      //ret = gp_camera_file_get_info(m_spCamera.get(), folder, name, &cameraFileInfo, m_spContext.get());
      //if (ret < GP_OK)
      //   break;

      fileList.push_back(info);
   }

   return fileList;
}

void CameraFileSystemImpl::StartDownload(const FileInfo& fileInfo, T_fnDownloadFinished fnDownloadFinished)
{
   // TODO implement
   fileInfo;
   fnDownloadFinished;
}
