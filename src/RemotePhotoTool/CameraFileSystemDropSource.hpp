//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file CameraFileSystemDropSource.hpp IDropSource implementation for camera file system
//
#pragma once

#include <atlcom.h>
#include <oleidl.h>

struct FileInfo;

/// infos about dragged files
struct CameraFileSystemDraggedFilesInfo
{
   // full file infos from the camera
   std::vector<FileInfo> m_cameraFileInfos;

   /// camera file system to access
   std::shared_ptr<CameraFileSystem> m_cameraFileSystem;
};

/// \brief IDropSource implementation for camera file system
/// \see https://www.codeproject.com/articles/14482/wtl-for-mfc-programmers-part-x-implementing-a-drag
class CameraFileSystemDropSource :
   public CComObjectRootEx<CComMultiThreadModel>,
   public CComCoClass<CameraFileSystemDropSource>,
   public IDataObject,
   public IDropSource
{
public:
   /// ctor
   CameraFileSystemDropSource()
      :m_lastEffect(DROPEFFECT_NONE)
   {
   }

   /// initializes drop source by the infos of files to be dragged
   void Init(CameraFileSystemDraggedFilesInfo draggedFilesInfo)
   {
      m_draggedFilesInfo = draggedFilesInfo;
   }

   HRESULT DoDragDrop(DWORD okEffects, DWORD* effects)
   {
      return ::DoDragDrop(this, this, okEffects, effects);
   }

   BEGIN_COM_MAP(CameraFileSystemDropSource)
      COM_INTERFACE_ENTRY(IDataObject)
      COM_INTERFACE_ENTRY(IDropSource)
   END_COM_MAP()

   // virtual methods from IDataObject

   STDMETHODIMP SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease) override
   {
      UNUSED(pformatetc);
      UNUSED(pmedium);
      UNUSED(fRelease);
      return E_NOTIMPL;
   }

   STDMETHODIMP GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium) override
   {
      UNUSED(pformatetcIn);
      UNUSED(pmedium);
      return E_NOTIMPL;
   }

   STDMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc) override
   {
      UNUSED(dwDirection);
      UNUSED(ppenumFormatEtc);
      return E_NOTIMPL;
   }

   STDMETHODIMP QueryGetData(FORMATETC* pformatetc) override
   {
      UNUSED(pformatetc);
      return E_NOTIMPL;
   }

   STDMETHODIMP GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium) override
   {
      UNUSED(pformatetc);
      UNUSED(pmedium);
      return E_NOTIMPL;
   }

   STDMETHODIMP GetCanonicalFormatEtc(FORMATETC* pformatectIn,
      FORMATETC* pformatetcOut)
   {
      UNUSED(pformatectIn);
      UNUSED(pformatetcOut);
      return E_NOTIMPL;
   }

   STDMETHODIMP DAdvise(FORMATETC* pformatetc, DWORD advf,
      IAdviseSink* pAdvSink, DWORD* pdwConnection) override
   {
      UNUSED(pformatetc);
      UNUSED(advf);
      UNUSED(pAdvSink);
      UNUSED(pdwConnection);
      return E_NOTIMPL;
   }

   STDMETHODIMP DUnadvise(DWORD dwConnection) override
   {
      UNUSED(dwConnection);
      return E_NOTIMPL;
   }

   STDMETHODIMP EnumDAdvise(IEnumSTATDATA** ppenumAdvise) override
   {
      UNUSED(ppenumAdvise);
      return E_NOTIMPL;
   }

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
   /// dragged files info
   CameraFileSystemDraggedFilesInfo m_draggedFilesInfo;

   /// last drop effect that was reported to us
   DWORD m_lastEffect;
};
