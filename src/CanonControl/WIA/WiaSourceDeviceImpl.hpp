//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file WiaSourceDeviceImpl.hpp WIA - SourceDevice impl
//
#pragma once

// includes
#include "WiaCommon.hpp"
#include "SourceDevice.hpp"
#include "CameraException.hpp"
#include <memory>

class DeviceProperty;

namespace WIA
{
   /// source device implementation for WIA
   class SourceDeviceImpl :
      public SourceDevice,
      public std::enable_shared_from_this<SourceDeviceImpl>
   {
   public:
      /// ctor
      SourceDeviceImpl(RefSp ref, CComPtr<IWiaItem> wiaDeviceRootItem)
         :m_ref(ref),
         m_wiaDeviceRootItem(wiaDeviceRootItem)
      {
      }

      /// dtor
      virtual ~SourceDeviceImpl() throw()
      {
      }

      virtual bool GetDeviceCapability(SourceDevice::T_enDeviceCapability deviceCapability) const override;

      virtual CString ModelName() const override;

      virtual CString SerialNumber() const override;

      virtual std::vector<unsigned int> EnumDeviceProperties() const override;

      virtual DeviceProperty GetDeviceProperty(unsigned int propertyId) const override;

      virtual std::shared_ptr<CameraFileSystem> GetFileSystem() override;

      virtual std::shared_ptr<RemoteReleaseControl> EnterReleaseControl() override;

   private:
      /// WIA reference
      RefSp m_ref;

      /// WIA device root item
      CComPtr<IWiaItem> m_wiaDeviceRootItem;
   };

} // namespace WIA
