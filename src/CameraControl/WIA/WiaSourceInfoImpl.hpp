//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file WiaSourceInfoImpl.hpp WIA - SourceInfo impl
//
#pragma once

// includes
#include "SourceInfo.hpp"
#include "WiaCommon.hpp"
#include "WiaSourceDeviceImpl.hpp"

namespace WIA
{
   /// source info impl for WIA
   class SourceInfoImpl : public SourceInfo
   {
   public:
      /// ctor
      SourceInfoImpl(RefSp& ref, const CString& deviceId, const CString& deviceName)
         :m_ref(ref),
         m_deviceId(deviceId),
         m_deviceName(deviceName)
      {
      }
      /// dtor
      virtual ~SourceInfoImpl()
      {
      }

      virtual CString Name() const override
      {
         return m_deviceName + _T(" [Windows Image Acquisition]");
      }

      virtual CString DeviceId() const override
      {
         return m_deviceId;
      }

      virtual std::shared_ptr<SourceDevice> Open() override
      {
         CComPtr<IWiaDevMgr> wiaDevMgr = m_ref->GetWiaDeviceManager();

         CComBSTR bstrDeviceId{ m_deviceId };

         CComPtr<IWiaItem> wiaDeviceRootItem;
         HRESULT hr = wiaDevMgr->CreateDevice(
            bstrDeviceId,
            &wiaDeviceRootItem);

         CheckError(_T("IWiaDevMgr::CreateDevice"), hr, __FILE__, __LINE__);

         return std::make_shared<SourceDeviceImpl>(m_ref, wiaDeviceRootItem);
      }

   private:
      /// WIA reference
      RefSp m_ref;

      /// WIA device id
      CString m_deviceId;

      /// printable device name
      CString m_deviceName;
   };

} // namespace WIA
