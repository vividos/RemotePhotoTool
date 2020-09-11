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
   CString targetFolder; // TODO
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

   FILE* fd = nullptr;
   errno_t ret = _tfopen_s(&fd, targetFilename, _T("wb"));
   if (ret != 0 || fd == nullptr)
      return; // TODO error reporting?

   std::shared_ptr<FILE> file{ fd, &fclose };

   fwrite(imageData.data(), 1, imageData.size(), file.get());

   file.reset();

   // TODO also set file date from camera
   //SetFileTime()
}
