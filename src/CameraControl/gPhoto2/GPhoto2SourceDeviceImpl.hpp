//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2SourceDeviceImpl.hpp gPhoto2 - SourceDevice impl
//
#pragma once

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
      SourceDeviceImpl(RefSp ref, std::shared_ptr<_Camera> camera);
      /// dtor
      virtual ~SourceDeviceImpl();

      // SourceDevice virtual functions

      virtual bool GetDeviceCapability(T_enDeviceCapability deviceCapability) const override;

      virtual CString ModelName() const override;

      virtual CString SerialNumber() const override;

      virtual std::vector<unsigned int> EnumDeviceProperties() const override;

      virtual DeviceProperty GetDeviceProperty(unsigned int propertyId) const override;

      virtual std::shared_ptr<CameraFileSystem> GetFileSystem() override;

      virtual std::shared_ptr<RemoteReleaseControl> EnterReleaseControl() override;

   private:
      /// gPhoto2 reference
      RefSp m_ref;

      /// camera instance
      std::shared_ptr<_Camera> m_camera;

      /// access to camera properties
      std::shared_ptr<PropertyAccess> m_properties;
   };

} // namespace GPhoto2
