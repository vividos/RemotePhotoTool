//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PsrecSourceInfoImpl.hpp PS-ReC - SourceInfo impl
//
#pragma once

// includes
#include "SourceInfo.hpp"
#include "SourceDevice.hpp"
#include "PsrecCommon.hpp"
#include "PsrecSourceDeviceImpl.hpp"

namespace PSREC
{

/// source info impl for PSREC
class SourceInfoImpl: public SourceInfo
{
public:
   /// ctor
   SourceInfoImpl(RefSp spRef, const prDeviceInfoTable& deviceInfo)
      :m_spRef(spRef),
       m_deviceInfo(deviceInfo)
   {
   }

   /// dtor
   virtual ~SourceInfoImpl() {}

   virtual CString Name() const override
   {
      CString cszName(reinterpret_cast<LPCWSTR>(m_deviceInfo.ModelName));
      return cszName;
   }

   virtual CString DeviceId() const override
   {
      CString cszDeviceId(reinterpret_cast<LPCWSTR>(m_deviceInfo.DeviceInternalName));
      return cszDeviceId;
   }

   virtual std::shared_ptr<SourceDevice> Open() override
   {
      return std::shared_ptr<SourceDevice>(new SourceDeviceImpl(m_spRef, m_deviceInfo));
   }

private:
   /// device info
   prDeviceInfoTable m_deviceInfo;

   /// SDK ref
   RefSp m_spRef;
};

} // namespace PSREC
