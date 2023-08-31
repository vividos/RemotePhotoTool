//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file CameraFileSystemDropSource.cpp IDropSource implementation for camera file system
//
#include "stdafx.h"
#include "CameraFileSystemDropSource.hpp"
#include "CameraFileSystem.hpp"
#include <ulib/thread/Event.hpp>
#include <ulib/Path.hpp>
#include "File.hpp"
#include "DropFilesHelper.hpp"

/// initializes drop source by the infos of files to be dragged
bool CameraFileSystemDropSource::Init(CameraFileSystemDraggedFilesInfo draggedFilesInfo)
{
   m_draggedFilesInfo = draggedFilesInfo;

   GetTempPath(MAX_PATH, m_tempFolder.GetBuffer(MAX_PATH));
   m_tempFolder.ReleaseBuffer();

   // create 0-length target files in the temp folder and call SetData() to
   // set the data to drag-and-drop
   DropFilesHelper dropFilesHelper;

   for (const auto& fileInfo : draggedFilesInfo.m_cameraFileInfos)
   {
      CString tempFilename =
         Path::Combine(m_tempFolder, Path::FilenameAndExt(fileInfo.m_filename));

      dropFilesHelper.AddFilename(tempFilename);

      // also create a 0-length placeholder file
      File::WriteAllBytes(tempFilename, std::vector<BYTE>());

      // Keep track of this temp file so we can clean up when the app exits.
      m_tempFilenames.push_back(tempFilename);
   }

   FORMATETC fetc;
   STGMEDIUM stg;
   if (!dropFilesHelper.GetDropFilesData(fetc, stg))
      return false;

   if (FAILED(SetData(&fetc, &stg, TRUE)))
   {
      GlobalFree(stg.hGlobal);
      return false;
   }

   return true;
}

void CameraFileSystemDropSource::Cleanup()
{
   for (const CString& filename : m_tempFilenames)
      DeleteFile(filename);
}

STDMETHODIMP CameraFileSystemDropSource::QueryContinueDrag(BOOL escapePressed, DWORD keyState)
{
   // if ESC was pressed, cancel the drag.
   if (escapePressed)
      return DRAGDROP_S_CANCEL;

   // if the left button was released, do the drop.
   if (!(keyState & (MK_LBUTTON | MK_RBUTTON)))
   {
      // If the last DROPEFFECT we got in GiveFeedback()
      // was DROPEFFECT_NONE, we abort because the allowable
      // effects of the source and target don't match up.
      if (DROPEFFECT_NONE == m_lastEffect)
         return DRAGDROP_S_CANCEL;

      /// The drop was accepted, so do the transfer
      if (TransferFiles())
         return DRAGDROP_S_DROP;
      else
         return E_UNEXPECTED;
   }

   return S_OK;
}

bool CameraFileSystemDropSource::TransferFiles()
{
   CString targetFolder = m_tempFolder;
   for (const FileInfo& fileInfo : m_draggedFilesInfo.m_cameraFileInfos)
   {
      ManualResetEvent eventFinished{ false };

      m_draggedFilesInfo.m_cameraFileSystem->StartDownload(fileInfo,
         [this, &eventFinished, &targetFolder](const FileInfo& fileInfo, const std::vector<unsigned char>& imageData)
         {
            SaveFile(fileInfo, targetFolder, imageData);
            eventFinished.Set();
         });

      eventFinished.Wait();
   }

   return true;
}

void CameraFileSystemDropSource::SaveFile(const FileInfo& fileInfo, const CString& targetFolder, const std::vector<unsigned char>& imageData)
{
   CString targetFilename =
      Path::Combine(targetFolder, Path::FilenameAndExt(fileInfo.m_filename));

   File::WriteAllBytes(targetFilename, imageData);

   // also set file date from camera
   File::SetModifiedTime(targetFilename, fileInfo.m_modifiedTime);
}
