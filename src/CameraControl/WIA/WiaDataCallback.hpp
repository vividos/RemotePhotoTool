//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file WiaDataCallback.hpp WIA data callback class
//
#pragma once

#include "CameraFileSystem.hpp"
#include "WiaCommon.hpp"

namespace WIA
{
   /// implementation of IWiaDataCallback
   class WiaDataCallback : public IWiaDataCallback
   {
   public:
      /// ctor
      WiaDataCallback(const FileInfo& fileInfo, CameraFileSystem::T_fnDownloadFinished fnDownloadFinished)
         :m_refCount(1),
         m_fileInfo(fileInfo),
         m_fnDownloadFinished(fnDownloadFinished)
      {
      }

   private:
      // Inherited via IWiaDataCallback
      virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;

      virtual ULONG STDMETHODCALLTYPE AddRef() override;

      virtual ULONG STDMETHODCALLTYPE Release() override;

      virtual HRESULT STDMETHODCALLTYPE BandedDataCallback(
         LONG lMessage, LONG lStatus, LONG lPercentComplete, LONG lOffset,
         LONG lLength, LONG lReserved, LONG lResLength, BYTE* pbBuffer) override;

   private:
      /// Object reference count
      LONG  m_refCount;

      /// file info of file to download
      FileInfo m_fileInfo;

      // handler to call when download has finished
      CameraFileSystem::T_fnDownloadFinished m_fnDownloadFinished;

      /// file buffer
      std::vector<BYTE> m_fileBuffer;
   };

} // namespace WIA
