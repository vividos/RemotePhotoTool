//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PsrecViewfinderImpl.hpp PS-ReC - Viewfinder impl
//
#pragma once

// includes
#include "Viewfinder.hpp"
#include "PsrecCommon.hpp"
#include "LightweightMutex.hpp"

namespace PSREC
{

/// viewfinder implementation for PSREC
class ViewfinderImpl: public Viewfinder
{
public:
   /// ctor
   ViewfinderImpl(std::shared_ptr<SourceDeviceImpl> spSourceDevice, prHandle hCamera)
      :m_spSourceDevice(spSourceDevice),
       m_hCamera(hCamera)
   {
      // may return prINVALID_FN_CALL, prINVALID_HANDLE, prMEM_ALLOC_FAILED, prINVALID_PARAMETER or @ERR
      prResponse err = PR_RC_StartViewFinder(m_hCamera,
         reinterpret_cast<prContext>(this),
         &ViewfinderImpl::OnThumbnailImageData_);

      LOG_TRACE(_T("PR_RC_StartViewFinder(%08x, this) returned %08x\n"), m_hCamera, err);
      CheckError(_T("PR_RC_StartViewFinder"), err, __FILE__, __LINE__);
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

   virtual void SetAvailImageHandler(Viewfinder::T_fnOnAvailViewfinderImage fnOnAvailViewfinderImage) override
   {
      LightweightMutex::LockType lock(m_mtxFnOnAvailViewfinderImage);

      m_fnOnAvailViewfinderImage = fnOnAvailViewfinderImage;
   }

   virtual void Close() override
   {
      // may return prINVALID_FN_CALL, prINVALID_HANDLE, prMEM_ALLOC_FAILED or @ERR
      prResponse err = PR_RC_TermViewFinder(m_hCamera);
      LOG_TRACE(_T("PR_RC_TermViewFinder(%08x) returned %08x\n"), m_hCamera, err);
      CheckError(_T("PR_RC_TermViewFinder"), err, __FILE__, __LINE__);
   }

private:
   /// callback function to transfer image data
   static prResponse prSTDCALL OnThumbnailImageData_(
      prHandle /*CameraHandle*/,
      prContext Context,
      prUInt32 Size,
      prVoid* pVFData)
   {
      ViewfinderImpl* pThis = reinterpret_cast<ViewfinderImpl*>(Context);

      pThis->OnThumbnailImageData(reinterpret_cast<BYTE*>(pVFData), Size);

      return prOK;
   }

   /// called when new thumbnail image can be transferred
   void OnThumbnailImageData(const BYTE* pData, UINT Size)
   {
      LightweightMutex::LockType lock(m_mtxFnOnAvailViewfinderImage);

      if (m_fnOnAvailViewfinderImage != nullptr)
         m_fnOnAvailViewfinderImage(std::vector<BYTE>(pData, pData + Size));
   }

private:
   /// source device
   std::shared_ptr<SourceDeviceImpl> m_spSourceDevice;

   /// camera handle
   prHandle m_hCamera;

   /// mutex for locking m_fnOnAvailViewfinderImage
   LightweightMutex m_mtxFnOnAvailViewfinderImage;

   /// handler function to receive viewfinder thumbnail image
   Viewfinder::T_fnOnAvailViewfinderImage m_fnOnAvailViewfinderImage;
};

} // namespace PSREC
