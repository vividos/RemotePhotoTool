//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file GPhoto2SourceDeviceImpl.hpp gPhoto2 - SourceDevice impl
//
#pragma once

// includes
#include "GPhoto2Common.hpp"
#include "GPhoto2PropertyAccess.hpp"
#include "SourceDevice.hpp"

namespace GPhoto2
{

/// implementation of SourceDevice for gPhoto2 access
class SourceDeviceImpl : public SourceDevice
{
public:
   /// ctor
   SourceDeviceImpl(std::shared_ptr<_GPContext> spContext, std::shared_ptr<_Camera> spCamera);
   /// dtor
   virtual ~SourceDeviceImpl() throw();

   // SourceDevice virtual functions

   virtual bool GetDeviceCapability(T_enDeviceCapability enDeviceCapability) const override;

   virtual CString ModelName() const override;

   virtual CString SerialNumber() const override;

   virtual std::vector<unsigned int> EnumDeviceProperties() const override;

   virtual DeviceProperty GetDeviceProperty(unsigned int uiPropertyId) const override;

   virtual std::shared_ptr<CameraFileSystem> GetFileSystem() override;

   virtual std::shared_ptr<RemoteReleaseControl> EnterReleaseControl() override;

private:
   /// gPhoto2 context
   std::shared_ptr<_GPContext> m_spContext;

   /// camera instance
   std::shared_ptr<_Camera> m_spCamera;

   /// access to camera properties
   std::shared_ptr<PropertyAccess> m_spProperties;
};

} // namespace GPhoto2
