//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file WiaCameraFileSystemImpl.hpp WIA - CameraFileSystem impl
//
#pragma once

// includes
#include "CameraFileSystem.hpp"
#include "WiaCommon.hpp"

class SourceDevice;

namespace WIA
{
   /// file system implementation for WIA
   class CameraFileSystemImpl :
      public CameraFileSystem
   {
   public:
      /// ctor
      CameraFileSystemImpl(RefSp ref, CComPtr<IWiaItem> wiaDeviceRootItem)
         :m_ref(ref),
         m_wiaDeviceRootItem(wiaDeviceRootItem)
      {
      }

      /// dtor
      virtual ~CameraFileSystemImpl() throw()
      {
      }

      virtual std::vector<CString> EnumFolders(const CString& path) const override;

      virtual std::vector<FileInfo> EnumFiles(const CString& path) const override;

      virtual void StartDownload(const FileInfo& fileInfo, T_fnDownloadFinished fnDownloadFinished) override;

   private:
      /// follows path, starting with base item
      CComPtr<IWiaItem> FollowPath(CComPtr<IWiaItem> baseItem, const CString& path) const;

      /// opens child item by name, using a path part string
      CComPtr<IWiaItem> OpenChildByName(CComPtr<IWiaItem> parentItem, const CString& pathPart) const;

      /// enumerates volumes or directories on camera
      std::vector<CString> EnumFolders(CComPtr<IWiaItem> item) const;

      /// enumerates files in given folder
      std::vector<FileInfo> EnumFiles(CComPtr<IWiaItem> item) const;

   private:
      /// WIA reference
      RefSp m_ref;

      /// WIA device root item
      CComPtr<IWiaItem> m_wiaDeviceRootItem;
   };

} // namespace EDSDK
