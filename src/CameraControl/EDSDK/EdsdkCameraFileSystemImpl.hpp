//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file EdsdkCameraFileSystemImpl.hpp EDSDK - CameraFileSystem impl
//
#pragma once

// includes
#include "CameraFileSystem.hpp"
#include "EdsdkCommon.hpp"

class SourceDevice;

namespace EDSDK
{
   /// file system implementation for EDSDK
   class CameraFileSystemImpl :
      public CameraFileSystem
   {
   public:
      /// ctor
      CameraFileSystemImpl(std::shared_ptr<SourceDevice> spSourceDevice, const Handle& hCamera)
         :m_spSourceDevice(spSourceDevice),
         m_hCamera(hCamera)
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
      /// follows path, starting with base element handle
      Handle FollowPath(Handle baseElement, const CString& path, unsigned int& pathLevel) const;

      /// opens child handle by name, using a path part string
      Handle OpenChildByName(const Handle& parentHandle, const CString& pathPart, unsigned int pathLevel) const;

      /// enumerates volumes or directories on camera
      std::vector<CString> EnumFolders(const Handle& parent, unsigned int pathLevel) const;

      /// enumerates files in given folder
      std::vector<FileInfo> EnumFiles(const Handle& parent, unsigned int pathLevel) const;

   private:
      /// source device
      std::shared_ptr<SourceDevice> m_spSourceDevice;

      /// handle to camera object
      Handle m_hCamera;
   };

} // namespace EDSDK
