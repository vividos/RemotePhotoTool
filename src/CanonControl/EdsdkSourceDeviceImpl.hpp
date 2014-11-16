//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file EdsdkSourceDeviceImpl.hpp EDSDK - SourceDevice impl
//
#pragma once

// includes
#include "SourceDevice.hpp"
#include "EdsdkRemoteReleaseControlImpl.hpp"
#include "EdsdkShutterCounterReader.hpp"

namespace EDSDK
{

/// source device implementation for EDSDK
class SourceDeviceImpl:
   public SourceDevice,
   public std::enable_shared_from_this<SourceDeviceImpl>
{
public:
   /// ctor
   SourceDeviceImpl(const Handle& hCamera, const EdsDeviceInfo& deviceInfo)
      :m_hCamera(hCamera),
      m_deviceInfo(deviceInfo)
   {
   }

   /// dtor
   virtual ~SourceDeviceImpl() throw()
   {
      // Ugly workaround: EdsCloseSession() may lock up; call EdsGetEvent() to process internal events.
      // This should probably go into an Idle handler for the SDK.
      for(int i=0; i<100; i++)
         EdsGetEvent();

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
      if (uiPropertyId == kEdsPropID_ShutterCounter)
      {
         ShutterCounterReader reader;

         unsigned int uiShutterCounter = 0;
         reader.Read(m_deviceInfo.szPortName, uiShutterCounter);

         Variant value;
         value.Set(uiShutterCounter);
         value.SetType(Variant::typeUInt32);

         return DeviceProperty(variantEdsdk, uiPropertyId, value, true);
      }

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
            _T("Not supported"), EDS_ERR_NOT_SUPPORTED, __FILE__, __LINE__);
      }

      std::shared_ptr<SourceDevice> spSourceDevice = shared_from_this();

      // there's no dedicated function to start "remote release control" mode, just add camera ref
      return std::shared_ptr<RemoteReleaseControl>(new RemoteReleaseControlImpl(spSourceDevice, m_hCamera));
   }

private:
   /// handle to camera object
   Handle m_hCamera;

   /// camera device info
   EdsDeviceInfo m_deviceInfo;
};

} // namespace EDSDK
