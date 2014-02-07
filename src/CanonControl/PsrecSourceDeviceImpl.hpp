//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PsrecSourceDeviceImpl.hpp PS-ReC - SourceDevice impl
//
#pragma once

// includes
#include "SourceDevice.hpp"
#include "PsrecCommon.hpp"
#include "PsrecRemoteReleaseControlImpl.hpp"
#include "PsrecPropertyAccess.hpp"
#include "PsrecDeviceInfo.hpp"

namespace PSREC
{

/// source device impl for PSREC
class SourceDeviceImpl:
   public SourceDevice,
   public std::enable_shared_from_this<SourceDeviceImpl>
{
public:
   /// ctor
   SourceDeviceImpl(RefSp spRef, prDeviceInfoTable& deviceInfo)
      :m_spRef(spRef),
       m_hCamera(0)
   {
      LOG_TRACE(_T("about to call PR_CreateCameraObject...\n"));

      prHandle hCamera = 0;
      // may return prINVALID_FN_CALL when SDK wasn't initialized
      // may return prINVALID_PARAMETER when specified device info is invalid
      // may return prMEM_ALLOC_FAILED on memory error
      prResponse err = PR_CreateCameraObject(&deviceInfo, &hCamera);
      LOG_TRACE(_T("PR_CreateCameraObject(&si = \"%ls\", &handle) returned %08x, handle %08x\n"),
         deviceInfo.ModelName, err, hCamera);
      CheckError(_T("PR_CreateCameraObject"), err, __FILE__, __LINE__);

      m_hCamera = hCamera;

      // connect to camera itself
      // may return prINVALID_FN_CALL, prINVALID_HANDLE, prMEM_ALLOC_FAILED
      err = PR_ConnectCamera(m_hCamera);
      LOG_TRACE(_T("PR_ConnectCamera(handle = %08x) returned %08x\n"), m_hCamera, err);
      CheckError(_T("PR_ConnectCamera"), err, __FILE__, __LINE__);

      // get device info so that we don't have to query every time
      m_spDeviceInfo.reset(new DeviceInfo(m_hCamera));
   }

   /// dtor
   virtual ~SourceDeviceImpl()
   {
      // disconnect from camera
      prResponse err = PR_DisconnectCamera(m_hCamera);
      LOG_TRACE(_T("PR_DisconnectCamera(%08x) returned %08x\n"), m_hCamera, err);
      CheckError(_T("PR_DisconnectCamera"), err, __FILE__, __LINE__);

      // may return prINVALID_FN_CALL when SDK wasn't initialized
      // may return prINVALID_HANDLE when specified handle is invalid
      err = PR_DestroyCameraObject(m_hCamera);
      LOG_TRACE(_T("PR_DestroyCameraObject(%08x) returned %08x\n"), m_hCamera, err);
      CheckError(_T("PR_DestroyCameraObject"), err, __FILE__, __LINE__);
   }

   virtual bool GetDeviceCapability(SourceDevice::T_enDeviceCapability enDeviceCapability) const override
   {
      ATLASSERT(m_spDeviceInfo != nullptr);
      DeviceInfo& info = *m_spDeviceInfo.get();

      switch (enDeviceCapability)
      {
      case SourceDevice::capRemoteReleaseControl:
         {
            bool bInitiateRelease =
               info.m_setOperationsSupported.find(prPTP_INITIATE_RELEASE_CONTROL) != info.m_setOperationsSupported.end();
            return bInitiateRelease;
         }
         break;

      case SourceDevice::capRemoteViewfinder:
         {
            bool bInitiateViewfinder =
               info.m_setOperationsSupported.find(prPTP_RC_INITIATE_VIEW_FINDER) != info.m_setOperationsSupported.end();

            return bInitiateViewfinder;
         }
         break;

      default:
         ATLASSERT(false);
      }
      return false;
   }

   virtual CString ModelName() const override
   {
      ATLASSERT(m_spDeviceInfo != nullptr);

      return m_spDeviceInfo->m_cszModel;
   }

   virtual CString SerialNumber() const override
   {
      ATLASSERT(m_spDeviceInfo != nullptr);

      return m_spDeviceInfo->m_cszSerialNumber;
   }

   virtual std::vector<unsigned int> EnumDeviceProperties() const override
   {
      ATLASSERT(m_spDeviceInfo != nullptr);
      DeviceInfo& info = *m_spDeviceInfo.get();

      const std::set<prUInt16>& setDeviceProperties =  info.m_setDevicePropertiesSupported;
      std::set<prUInt16>::const_iterator iter = setDeviceProperties.begin(),
         stop = setDeviceProperties.end();

      std::vector<unsigned int> vecData;

      // just put the device property id's into the vector
      for (; iter != stop; iter++)
         vecData.push_back(*iter);

      return vecData;
   }

   virtual DeviceProperty GetDeviceProperty(unsigned int uiPropertyId) const override
   {
      prUInt16 propId = static_cast<prUInt16>(uiPropertyId & 0xFFFF);

      // get device property value
      DevicePropDesc desc(m_hCamera, propId, false);

      DeviceProperty dp(variantPsrec, uiPropertyId, desc.m_varCurrentValue, !desc.IsSetAllowed());

      // TODO needed to create PropertyAccess object here?
      PropertyAccess pa(m_hCamera);
      pa.Enum(propId, dp.m_vecValidValues);

      return dp;
   }

   virtual std::shared_ptr<RemoteReleaseControl> EnterReleaseControl() override
   {
      if (!GetDeviceCapability(capRemoteReleaseControl))
      {
         // throw an error code of 7, which means "not supported"
         throw CameraException(_T("SourceDevice::EnterReleaseControl"), false, 0, 7, __FILE__, __LINE__);
      }

      std::shared_ptr<SourceDeviceImpl> spSourceDevice = this->shared_from_this();

      return std::shared_ptr<RemoteReleaseControl>(new RemoteReleaseControlImpl(m_hCamera, spSourceDevice));
   }

   /// returns device info
   std::shared_ptr<DeviceInfo> GetDeviceInfo() { return m_spDeviceInfo; }

private:
   /// camera handle
   prHandle m_hCamera;

   /// SDK ref
   RefSp m_spRef;

   /// device info
   std::shared_ptr<DeviceInfo> m_spDeviceInfo;
};

} // namespace PSREC
