//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file EdsdkSourceInfoImpl.hpp EDSDK - SourceInfo impl
//
#pragma once

// includes
#include "SourceInfo.hpp"
#include "EdsdkSourceDeviceImpl.hpp"

namespace EDSDK
{

/// source info impl for EDSDK
class SourceInfoImpl: public SourceInfo
{
public:
   /// ctor
   SourceInfoImpl(const Handle& hCamera)
      :m_hCamera(hCamera)
   {
      ZeroMemory(&m_deviceInfo, sizeof(m_deviceInfo));
   }
   /// dtor
   virtual ~SourceInfoImpl() {}

   virtual CString Name() const override
   {
      return CString(m_deviceInfo.szDeviceDescription) + _T(" [Canon EOS SDK]");
   }

   virtual CString DeviceId() const override
   {
      return m_deviceInfo.szPortName;
   }

   virtual std::shared_ptr<SourceDevice> Open() override
   {
      LOG_TRACE(_T("about to call EdsOpenSession...\n"));

      EdsError err = EdsOpenSession(m_hCamera);
      LOG_TRACE(_T("EdsOpenSession(ref = %08x) returned %08x\n"), m_hCamera.Get(), err);
      EDSDK::CheckError(_T("EdsOpenSession"), err, __FILE__, __LINE__);

      return std::shared_ptr<SourceDevice>(new SourceDeviceImpl(m_hCamera, m_deviceInfo));
   }

   /// returns device info struct
   EdsDeviceInfo& GetDeviceInfo() { return m_deviceInfo; }

private:
   /// device info
   EdsDeviceInfo m_deviceInfo;

   /// handle to camera object
   Handle m_hCamera;
};

} // namespace EDSDK
