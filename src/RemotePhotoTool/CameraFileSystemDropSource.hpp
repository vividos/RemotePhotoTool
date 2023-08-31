//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file CameraFileSystemDropSource.hpp IDropSource implementation for camera file system
//
#pragma once

#include <atlcom.h>
#include <oleidl.h>
#include "atldataobject.h"

struct FileInfo;

/// infos about dragged files
struct CameraFileSystemDraggedFilesInfo
{
   /// full file infos from the camera
   std::vector<FileInfo> m_cameraFileInfos;

   /// camera file system to access
   std::shared_ptr<CameraFileSystem> m_cameraFileSystem;
};

/// \brief IDropSource implementation for camera file system
/// \see https://www.codeproject.com/articles/14482/wtl-for-mfc-programmers-part-x-implementing-a-drag
/// \see https://web.archive.org/web/20050824130636/http://msdn.microsoft.com/library/en-us/dnwui/html/ddhelp_pt2.asp
class CameraFileSystemDropSource :
   public CComObjectRootEx<CComMultiThreadModel>,
   public CComCoClass<CameraFileSystemDropSource>,
   public CDataObjectImpl<CameraFileSystemDropSource>,
   public IDropSource
{
public:
   /// ctor
   CameraFileSystemDropSource()
      :m_lastEffect(DROPEFFECT_NONE)
   {
   }

   /// initializes drop source by the infos of files to be dragged
   bool Init(CameraFileSystemDraggedFilesInfo draggedFilesInfo);

   /// performs drag-drop and transfers the dropped files
   HRESULT DoDragDrop(DWORD okEffects, DWORD* effects)
   {
      return ::DoDragDrop(this, this, okEffects, effects);
   }

   /// cleans up drop source by removing the temp files
   void Cleanup();

   BEGIN_COM_MAP(CameraFileSystemDropSource)
      COM_INTERFACE_ENTRY(IDataObject)
      COM_INTERFACE_ENTRY(IDropSource)
   END_COM_MAP()

   // virtual methods from IDropSource

   /// called to update the last used drop effect
   STDMETHODIMP GiveFeedback(DWORD effect) override
   {
      m_lastEffect = effect;
      return DRAGDROP_S_USEDEFAULTCURSORS;
   }

   /// called to check if query should continue, be cancelled or completed
   STDMETHODIMP QueryContinueDrag(BOOL escapePressed, DWORD keyState) override;

   /// transfers all files
   bool TransferFiles();

   /// saves a file to the target folder
   void SaveFile(const FileInfo& fileInfo, const CString& targetFolder, const std::vector<unsigned char>& imageData);

private:
   /// temp folder to download the files into
   CString m_tempFolder;

   /// dragged files info
   CameraFileSystemDraggedFilesInfo m_draggedFilesInfo;

   /// list of all temp files produced during dropping
   std::vector<CString> m_tempFilenames;

   /// last drop effect that was reported to us
   DWORD m_lastEffect;
};
