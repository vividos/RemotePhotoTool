//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file Gphoto2CameraFileSystemImpl.hpp gPhoto2 - CameraFileSystem impl
//
#pragma once

#include "CameraFileSystem.hpp"
#include "Gphoto2Common.hpp"

namespace GPhoto2
{
   /// file system implementation for gPhoto2
   class CameraFileSystemImpl :
      public CameraFileSystem
   {
   public:
      /// ctor
      CameraFileSystemImpl(std::shared_ptr<_GPContext> spContext, std::shared_ptr<_Camera> spCamera)
         :m_spContext(spContext),
         m_spCamera(spCamera)
      {
      }

      /// dtor
      virtual ~CameraFileSystemImpl()
      {
      }

      virtual std::vector<CString> EnumFolders(const CString& path) const override;

      virtual std::vector<FileInfo> EnumFiles(const CString& path) const override;

      virtual void StartDownload(const FileInfo& fileInfo, T_fnDownloadFinished fnDownloadFinished) override;

   private:
      /// context handle
      std::shared_ptr<_GPContext> m_spContext;

      /// camera handle
      std::shared_ptr<_Camera> m_spCamera;
   };

} // namespace GPhoto2
