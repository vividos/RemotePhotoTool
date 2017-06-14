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

      virtual bool GetDeviceCapability(SourceDevice::T_enDeviceCapability deviceCapability) const override
      {
         deviceCapability;
         /*
         switch (enDeviceCapability)
         {
         case SourceDevice::capRemoteReleaseControl:
            // supported on all camera models
            return true;

         case SourceDevice::capRemoteViewfinder:
            // supported on all camera models
            return true;

         case SourceDevice::capCameraFileSystem:
            // supported on all camera models
            return true;

         default:
            ATLASSERT(false);
         }
         */

         return false;
      }

      virtual CString ModelName() const override
      {
         // TODO implement
         return CString();
      }

      virtual CString SerialNumber() const override
      {
         // TODO implement
         return CString();
      }

      virtual std::vector<unsigned int> EnumDeviceProperties() const override
      {
         throw CameraException(_T(""), _T(""), 0, __FILE__, __LINE__);
      }

      virtual DeviceProperty GetDeviceProperty(unsigned int propertyId) const override
      {
         propertyId;
         throw CameraException(_T(""), _T(""), 0, __FILE__, __LINE__);
      }

      virtual std::shared_ptr<CameraFileSystem> GetFileSystem() override
      {
         throw CameraException(_T(""), _T(""), 0, __FILE__, __LINE__);
      }

      virtual std::shared_ptr<RemoteReleaseControl> EnterReleaseControl() override
      {
         throw CameraException(_T(""), _T(""), 0, __FILE__, __LINE__);
      }

   private:
      /// WIA reference
      RefSp m_ref;

      /// WIA device root item
      CComPtr<IWiaItem> m_wiaDeviceRootItem;
   };

} // namespace WIA
