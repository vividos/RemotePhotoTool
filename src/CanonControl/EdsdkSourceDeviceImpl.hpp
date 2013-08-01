//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file EdsdkSourceDeviceImpl.hpp EDSDK - SourceDevice impl
//
#pragma once

// includes
#include "SourceDevice.hpp"
#include "EdsdkRemoteReleaseControlImpl.hpp"

namespace EDSDK
{

/// source device implementation for EDSDK
class SourceDeviceImpl:
   public SourceDevice,
   public std::enable_shared_from_this<SourceDeviceImpl>
{
public:
   /// ctor
   SourceDeviceImpl(const Handle& hCamera)
      :m_hCamera(hCamera)
   {
   }

   /// dtor
   virtual ~SourceDeviceImpl() throw()
   {
      EdsError err = EdsCloseSession(m_hCamera);
      LOG_TRACE(_T("EdsCloseSession(ref = %08x) returned %08x\n"), m_hCamera.Get(), err);
      // note: don't check error here, as we're in dtor
   }

   virtual bool GetDeviceCapability(SourceDevice::T_enDeviceCapability enDeviceCapability) const override
   {
      switch (enDeviceCapability)
      {
      case SourceDevice::capRemoteReleaseControl:
         // supported on all camera models
         return true;

      case SourceDevice::capRemoteViewfinder:
         // supported on all camera models
         return true;

      default:
         ATLASSERT(false);
      }
      return false;
   }

   virtual CString ModelName() const override
   {
      PropertyAccess p(m_hCamera);

      Variant v = p.Get(kEdsPropID_ProductName);
      return v.Get<CString>();
   }

   virtual CString SerialNumber() const override
   {
      PropertyAccess p(m_hCamera);

      Variant v = p.Get(kEdsPropID_BodyIDEx);
      return v.Get<CString>();
   }

   virtual std::vector<unsigned int> EnumDeviceProperties() const override
   {
      PropertyAccess p(m_hCamera);

      std::vector<unsigned int> vecDeviceIds;
      p.EnumDeviceIds(vecDeviceIds);

      return vecDeviceIds;
   }

   virtual DeviceProperty GetDeviceProperty(unsigned int uiPropertyId) const override
   {
      // get value
      PropertyAccess p(m_hCamera);
      Variant value = p.Get(uiPropertyId);
      bool bReadOnly = p.IsReadOnly(uiPropertyId);

      return DeviceProperty(variantEdsdk, uiPropertyId, value, bReadOnly);
   }

   virtual std::shared_ptr<RemoteReleaseControl> EnterReleaseControl() override
   {
      if (!GetDeviceCapability(capRemoteReleaseControl))
      {
         // throw an error code of 7, which means "not supported"
         throw CameraException(_T("SourceDevice::EnterReleaseControl"),
            false, 0, EDS_ERR_NOT_SUPPORTED, __FILE__, __LINE__);
      }

      std::shared_ptr<SourceDevice> spSourceDevice = shared_from_this();

      // there's no dedicated function to start "remote release control" mode, just add camera ref
      return std::shared_ptr<RemoteReleaseControl>(new RemoteReleaseControlImpl(spSourceDevice, m_hCamera));
   }

private:
   /// handle to camera object
   Handle m_hCamera;
};

} // namespace EDSDK
