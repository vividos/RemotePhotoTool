//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file CameraFileSystem.hpp Canon control - Camera file system class
//
#pragma once

// includes
#include <vector>
#include <functional>

/// camera file info
struct FileInfo
{
   /// default ctor
   FileInfo()
      :m_fileSize(0UL),
      m_modifiedTime(-1)
   {
   }

   /// filename
   CString m_filename;

   /// file size in bytes
   unsigned long m_fileSize;

   /// time of last file modification
   time_t m_modifiedTime;
};

/// \brief camera file system
class CameraFileSystem
{
public:
   /// path separator for paths in the camera file system
   static LPCTSTR PathSeparator;

   /// dtor
   virtual ~CameraFileSystem() {}

   /// returns list of subfolders in this folder
   virtual std::vector<CString> EnumFolders(const CString& path) const = 0;

   /// returns list of files in the folder
   virtual std::vector<FileInfo> EnumFiles(const CString& path) const = 0;

   /// function that is called when download of a file has finished
   typedef std::function<void(const FileInfo&, std::vector<unsigned char>)> T_fnDownloadFinished;

   /// starts download of given file in a worker thread
   virtual void StartDownload(const FileInfo& fileInfo, T_fnDownloadFinished fnDownloadFinished) = 0;
};
