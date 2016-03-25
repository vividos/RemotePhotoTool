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

      cdUInt32 format = 0; // 0 = JPEG, 1 = Bitmap
      StartViewfinder(format);
   }

   /// dtor
   virtual ~ViewfinderImpl() throw()
   {
      try
      {
         SetAvailImageHandler(Viewfinder::T_fnOnAvailViewfinderImage());

         Close();
      }
      catch (...)
      {
      }
   }

   virtual void SetOutputType(Viewfinder::T_enOutputType enOutputType) override
   {
      // note that T_enOutputType values exactly correspond to cdRelViewfinderOutput
      cdRelViewfinderOutput output = static_cast<cdRelViewfinderOutput>(enOutputType);

      cdError err = CDSelectViewFinderCameraOutput(GetSource(), output);
      LOG_TRACE(_T("CDSelectViewFinderCameraOutput(%08x) returned %08x\n"), GetSource(), err);
      CheckError(_T("CDSelectViewFinderCameraOutput"), err, __FILE__, __LINE__);
   }

   virtual void SetAvailImageHandler(Viewfinder::T_fnOnAvailViewfinderImage fnOnAvailViewfinderImage) override
   {
      LightweightMutex::LockType lock(m_mtxFnOnAvailViewfinderImage);

      m_fnOnAvailViewfinderImage = fnOnAvailViewfinderImage;
   }

   virtual void Close() override
   {
      // may return cdINVALID_HANDLE, cdINVALID_FN_CALL
      cdError err = CDTermViewfinder(GetSource());
      LOG_TRACE(_T("CDTermViewfinder(%08x) returned %08x\n"), GetSource(), err);
      CheckError(_T("CDTermViewfinder"), err, __FILE__, __LINE__);
   }

private:
   /// starts viewfinder
   void StartViewfinder(cdUInt32 format)
   {
      // check: pointers (e.g. this) must fit into cdContext; may not work on 64-bit
      static_assert(sizeof(cdContext) == sizeof(void*), "pointers must fit into cdContext");

#pragma warning(push)
#pragma warning(disable: 4311) // 'reinterpret_cast' : pointer truncation from 'P' to 'T'
      cdContext context = reinterpret_cast<cdContext>(this);
#pragma warning(pop)

      // may return cdINVALID_HANDLE, cdNOT_SUPPORTED
      cdError err = CDStartViewfinder(GetSource(),
         format,
         &ViewfinderImpl::ViewfinderCallback,
         context);

      LOG_TRACE(_T("CDStartViewfinder(source = %08x, %u (%s), &CallbackFunc, context=%08x) returned %08x\n"),
         GetSource(),
         format,
         format == 0 ? _T("JPEG") :
         format == 1 ? _T("BMP") : _T("???"),
         this, err);
      CheckError(_T("CDStartViewfinder"), err, __FILE__, __LINE__);
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
