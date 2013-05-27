//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file CdsdkViewfinderImpl.hpp CDSDK - Viewfinder impl
//
#pragma once

// includes
#include "Viewfinder.hpp"
#include "CdsdkCommon.hpp"
#include "LightweightMutex.hpp"

namespace CDSDK
{

class SourceDeviceImpl;

/// viewfinder impl for CDSDK
class ViewfinderImpl: public Viewfinder
{
public:
   /// ctor
   ViewfinderImpl(std::shared_ptr<SourceDeviceImpl> spSourceDevice)
      :m_spSourceDevice(spSourceDevice),
       m_dwLastViewfinderCallbackTick(0)
   {
      // note: check if camera supports viewfinder before calling CDStartViewfinder()
      // Not all camera models support the Viewfinder function. Cameras that support Viewfinder
      // are those cameras for which the cdRELEASE_CONTROL_CAP_VIEWFINDER bit is set. Obtain the
      // value by executing CDGetDevicePropertyData using cdDEVICE_PROP_RELEASE_CONTROL_CAP.

      // check: pointers (e.g. this) must fit into cdContext; may not work on 64-bit
      static_assert(sizeof(cdContext) == sizeof(void*), "pointers must fit into cdContext");

#pragma warning(push)
#pragma warning(disable: 4311) // 'reinterpret_cast' : pointer truncation from 'P' to 'T'
      cdError err = CDStartViewfinder(GetSource(),
         1, // format; 0 = JPEG, 1 = Bitmap
         &ViewfinderImpl::ViewfinderCallback,
         reinterpret_cast<cdContext>(this));
#pragma warning(pop)

      LOG_TRACE(_T("CDStartViewfinder(%08x, 1 (BMP), &CallbackFunc, ctx=%08x) returned %08x\n"),
         GetSource(), this, err);
      CheckError(_T("CDStartViewfinder"), err, __FILE__, __LINE__);
   }

   /// dtor
   ~ViewfinderImpl()
   {
      cdError err = CDTermViewfinder(GetSource());
      LOG_TRACE(_T("CDTermViewfinder(%08x) returned %08x\n"), GetSource(), err);
      CheckError(_T("CDTermViewfinder"), err, __FILE__, __LINE__);
   }

   virtual void SetAvailImageHandler(Viewfinder::T_fnOnAvailViewfinderImage fnOnAvailViewfinderImage) override
   {
      // TODO implement
   }

   // check cdRELEASE_CONTROL_CAP_ABORT_VIEWFINDER if viewfinder has to be terminated to take a picture

   // CDSelectViewfinderCameraOutput
   // CDActViewfinderAutoFunctions

   /// called when new viewfinder image is available
   void OnViewfinderImage(const std::vector<BYTE>& vecData, bool /*bFormatIsJpeg*/) // TODO remove bFormatIsJpeg again?
   {
      // try to get hold of mutex
      LightweightMutex::LockType lock(m_mtxBuffer);

      m_vecCurrentData.assign(vecData.begin(), vecData.end());

      //m_evtNewData.Set();
   }

   /// callback function called when new viewfinder image is available
   static cdUInt32 __stdcall ViewfinderCallback(
      cdVoid* pBuf,        // Pointer to the buffer that contains one frame of Viewfinder data.
      cdUInt32 Size,       // Buffer size indicated by pBuf
      cdUInt32 Format,     // Viewfinder data format
      cdContext Context)   // Context
   {
#pragma warning(push)
#pragma warning(disable: 4312) // 'reinterpret_cast' : conversion from 'T' to 'P' of greater size
      ViewfinderImpl* pThis = reinterpret_cast<ViewfinderImpl*>(Context);
      ATLASSERT(pThis != nullptr);
#pragma warning(pop)

#ifdef _DEBUG
      DWORD dwCurrentTick = GetTickCount();

      LOG_TRACE(_T("\nViewfinderCallback(p = %p, size = %u, format = %u (%s), context) called with new viewfinder data;")
         _T("last callback was %u ms ago.\n"),
         pBuf, Size, Format,
         Format == 0 ? _T("JPEG") : Format == 1 ? _T("BMP") : _T("???"),
         pThis->m_dwLastViewfinderCallbackTick == 0 ? 0 : dwCurrentTick - pThis->m_dwLastViewfinderCallbackTick);
      pThis->m_dwLastViewfinderCallbackTick = dwCurrentTick;
#endif

      const BYTE* pbData = static_cast<BYTE*>(pBuf);

      std::vector<BYTE> vecData(pbData, pbData+Size);
      pThis->OnViewfinderImage(vecData, Format == 0);

      return 0;
   }

   /// returns source
   cdHSource GetSource() const throw();

private:
   /// source device
   std::shared_ptr<SourceDeviceImpl> m_spSourceDevice;

   /// lock for current image buffer
   LightweightMutex m_mtxBuffer;

   /// current viewfinder data
   std::vector<BYTE> m_vecCurrentData;

   /// last tick where viewfinder image was retrieved
   DWORD m_dwLastViewfinderCallbackTick;
};

} // namespace CDSDK
