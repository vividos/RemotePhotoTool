//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CdsdkViewfinderImpl.hpp CDSDK - Viewfinder impl
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
// TODO use CDSelectViewfinderCameraOutput
class ViewfinderImpl : public Viewfinder
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
      cdContext context = reinterpret_cast<cdContext>(this);
#pragma warning(pop)

      // may return cdINVALID_HANDLE, cdNOT_SUPPORTED
      cdError err = CDStartViewfinder(GetSource(),
         0, // format; 0 = JPEG, 1 = Bitmap
         &ViewfinderImpl::ViewfinderCallback,
         context);

      LOG_TRACE(_T("CDStartViewfinder(%08x, 1 (BMP), &CallbackFunc, context=%08x) returned %08x\n"),
         GetSource(), this, err);
      CheckError(_T("CDStartViewfinder"), err, __FILE__, __LINE__);
   }

   /// dtor
   virtual ~ViewfinderImpl() throw()
   {
      // disable handler
      SetAvailImageHandler(Viewfinder::T_fnOnAvailViewfinderImage());

      // may return cdINVALID_HANDLE, cdINVALID_FN_CALL
      cdError err = CDTermViewfinder(GetSource());
      LOG_TRACE(_T("CDTermViewfinder(%08x) returned %08x\n"), GetSource(), err);
   }

   virtual void SetAvailImageHandler(Viewfinder::T_fnOnAvailViewfinderImage fnOnAvailViewfinderImage) override
   {
      LightweightMutex::LockType lock(m_mtxFnOnAvailViewfinderImage);

      m_fnOnAvailViewfinderImage = fnOnAvailViewfinderImage;
   }

private:
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

   /// called when new viewfinder image is available
   void OnViewfinderImage(const std::vector<BYTE>& vecData, bool /*bFormatIsJpeg*/)
   {
      LightweightMutex::LockType lock(m_mtxFnOnAvailViewfinderImage);

      if (m_fnOnAvailViewfinderImage)
         m_fnOnAvailViewfinderImage(vecData);
   }

   /// returns source
   cdHSource GetSource() const throw();

private:
   /// source device
   std::shared_ptr<SourceDeviceImpl> m_spSourceDevice;

   /// last tick where viewfinder image was retrieved
   DWORD m_dwLastViewfinderCallbackTick;

   /// mutex to protect m_fnOnAvailViewfinderImage
   LightweightMutex m_mtxFnOnAvailViewfinderImage;

   /// viewfinder image handler
   Viewfinder::T_fnOnAvailViewfinderImage m_fnOnAvailViewfinderImage;
};

} // namespace CDSDK
