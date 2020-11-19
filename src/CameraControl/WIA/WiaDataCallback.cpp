//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file WiaDataCallback.cpp WIA data callback class
//
#include "stdafx.h"
#include "WiaDataCallback.hpp"

using WIA::WiaDataCallback;

HRESULT STDMETHODCALLTYPE WiaDataCallback::QueryInterface(REFIID riid, void** ppvObject)
{
   if (ppvObject == nullptr)
      return E_INVALIDARG;

   if (IsEqualIID(riid, IID_IUnknown))
      *ppvObject = static_cast<WiaDataCallback*>(this);
   else if (IsEqualIID(riid, IID_IWiaDataCallback))
      *ppvObject = static_cast<WiaDataCallback*>(this);
   else
   {
      *ppvObject = nullptr;
      return E_NOINTERFACE;
   }

   reinterpret_cast<IUnknown*>(*ppvObject)->AddRef();
   return S_OK;
}

ULONG STDMETHODCALLTYPE WiaDataCallback::AddRef()
{
   return InterlockedIncrement(&m_refCount);
}

ULONG STDMETHODCALLTYPE WiaDataCallback::Release()
{
   LONG newRefCount = InterlockedDecrement(&m_refCount);
   if (0 == newRefCount)
   {
      delete this;
   }

   return newRefCount;
}

HRESULT STDMETHODCALLTYPE WiaDataCallback::BandedDataCallback(LONG lMessage, LONG lStatus, LONG lPercentComplete,
   LONG lOffset, LONG lLength, LONG lReserved, LONG lResLength, BYTE* pbBuffer)
{
   UNUSED(lReserved);
   UNUSED(lResLength);

   ATLTRACE(_T("WiaDataCallback::BandedDataCallback: message=%u, status=%u, percent=%u, offset=%u, length=%u, buffer=%p\n"),
      lMessage, lStatus, lPercentComplete, lOffset, lLength, pbBuffer);

   switch (lMessage)
   {
   case IT_MSG_DATA_HEADER:
   {
      PWIA_DATA_CALLBACK_HEADER pHeader = reinterpret_cast<PWIA_DATA_CALLBACK_HEADER>(pbBuffer);
      if (pHeader && pHeader->lBufferSize)
      {
         m_fileBuffer.resize(pHeader->lBufferSize);

         // m_guidFormat = pHeader->guidFormatID;
      }
      break;
   }

   case IT_MSG_STATUS:
   case IT_MSG_DATA:
   {
      size_t endPos = static_cast<size_t>(lOffset + lLength);
      if (endPos > m_fileBuffer.size())
         m_fileBuffer.resize(endPos);

      std::copy_n(pbBuffer, static_cast<size_t>(lLength), m_fileBuffer.data() + static_cast<size_t>(lOffset));
      break;
   }

   case IT_MSG_TERMINATION:
      m_fnDownloadFinished(m_fileInfo, m_fileBuffer);
      break;

   default:
      // do nothing
      break;
   }

   return S_OK;
}
