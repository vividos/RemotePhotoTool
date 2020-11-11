//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file WiaCommon.cpp WIA - Windows Image Acquisition common functions
//

// includes
#include "stdafx.h"
#include "WiaCommon.hpp"
#include "WiaSourceInfoImpl.hpp"
#include "WiaPropertyAccess.hpp"
#include <ulib/win32/ErrorMessage.hpp>

#pragma comment(lib, "wiaguid.lib")

using namespace WIA;

/// returns an error text from HRESULT value
CString ErrorFromHRESULT(HRESULT hr)
{
   if (HRESULT_FACILITY(hr) != FACILITY_WIA)
   {
      return Win32::ErrorMessage(hr).ToString();
   }

   switch (hr)
   {
      // only handle WIA error codes relevant to cameras
   case WIA_ERROR_GENERAL_ERROR: return _T("General error");
   case WIA_ERROR_OFFLINE: return _T("Offline");
   case WIA_ERROR_BUSY: return _T("Busy");
   case WIA_ERROR_USER_INTERVENTION: return _T("User intervention");
   case WIA_ERROR_ITEM_DELETED: return _T("Item deleted");
   case WIA_ERROR_DEVICE_COMMUNICATION: return _T("Device communication");
   case WIA_ERROR_INVALID_COMMAND: return _T("Invalid command");
   case WIA_ERROR_INCORRECT_HARDWARE_SETTING: return _T("Incorrect hardware setting");
   case WIA_ERROR_DEVICE_LOCKED: return _T("Device locked");
   case WIA_ERROR_EXCEPTION_IN_DRIVER: return _T("Exception in driver");
   case WIA_ERROR_INVALID_DRIVER_RESPONSE: return _T("Invalid driver response");
   case WIA_S_NO_DEVICE_AVAILABLE: return _T("No device available");
   default:
      return _T("Unknown error");
   }
}

void WIA::CheckError(const CString& function, HRESULT hr, LPCSTR file, UINT line)
{
   if (SUCCEEDED(hr))
      return;

   CString message;
   message.Format(_T("Error in function \"%s\": %s (%08lx)"),
      function.GetString(),
      ErrorFromHRESULT(hr).GetString(),
      (DWORD)hr);

   throw CameraException(function, message, hr, file, line);
}


//
// Ref
//

Ref::Ref()
{
   // use WIA 1.0; in 2.0, supported from Vista onwards, cameras can't be accessed anymore
   HRESULT hr = CoCreateInstance(CLSID_WiaDevMgr, nullptr, CLSCTX_LOCAL_SERVER, IID_IWiaDevMgr, (void**)&m_wiaDevMgr);

   if (FAILED(hr))
      m_wiaDevMgr = nullptr;
}

Ref::~Ref()
{
   AsyncWaitForCamera(false);
}

void Ref::AddVersionText(CString& versionText) const
{
   if (m_wiaDevMgr == nullptr)
      return; // WIA not available

   versionText += _T("WIA - Windows Image Acquisition\n");
}

void Ref::EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& sourceInfoList) const
{
   if (m_wiaDevMgr == nullptr)
      return; // WIA not available

   CComPtr<IEnumWIA_DEV_INFO> wiaEnumDevInfo;
   HRESULT hr = m_wiaDevMgr->EnumDeviceInfo(WIA_DEVINFO_ENUM_LOCAL, &wiaEnumDevInfo);
   CheckError(_T("IWiaDevMgr::EnumDeviceInfo"), hr, __FILE__, __LINE__);

   hr = wiaEnumDevInfo->Reset();
   CheckError(_T("IEnumWIA_DEV_INFO::Reset"), hr, __FILE__, __LINE__);

   do
   {
      // for every camera, retrieve properties
      CComPtr<IWiaPropertyStorage> wiaPropertyStorage;
      hr = wiaEnumDevInfo->Next(1, &wiaPropertyStorage, nullptr);

      if (hr == S_OK)
      {
         ULONG numProperties = 0;
         hr = wiaPropertyStorage->GetCount(&numProperties);
         ATLASSERT(SUCCEEDED(hr));

         PROPSPEC propSpec[2] = { 0 };
         PROPVARIANT propVariant[2] = { 0 };

         const ULONG propertyCount = sizeof(propSpec) / sizeof(propSpec[0]);

         // read device id, name and description
         propSpec[0].ulKind = PRSPEC_PROPID;
         propSpec[0].propid = WIA_DIP_DEV_ID;

         propSpec[1].ulKind = PRSPEC_PROPID;
         propSpec[1].propid = WIA_DIP_DEV_NAME;

         hr = wiaPropertyStorage->ReadMultiple(propertyCount, propSpec, propVariant);
         if (SUCCEEDED(hr))
         {
            RefSp ref = const_cast<Ref*>(this)->shared_from_this();

            CString deviceId = PropertyAccess::StringFromPropVariant(propVariant[0]);
            CString deviceName = PropertyAccess::StringFromPropVariant(propVariant[1]);

            sourceInfoList.push_back(
               std::make_shared<SourceInfoImpl>(ref, deviceId, deviceName));
         }

         FreePropVariantArray(propertyCount, propVariant);
      }
   } while (hr == S_OK);
}

void Ref::AsyncWaitForCamera(bool bStart, std::function<void()> fnOnCameraConnected)
{
   if (m_wiaDevMgr == nullptr)
      return; // WIA not available

   {
      LightweightMutex::LockType lock(m_mtxFnOnCameraConnected);

      m_fnOnCameraConnected = fnOnCameraConnected;
   }

   m_eventObject.Release();

   if (bStart)
   {
      m_wiaDevMgr->RegisterEventCallbackInterface(
         0,
         nullptr, // listen for events on all devices
         &WIA_EVENT_DEVICE_CONNECTED,
         this,
         &m_eventObject);
   }
}

HRESULT Ref::QueryInterface(REFIID riid, void** ppvObject)
{
   if (riid == IID_IUnknown)
   {
      *ppvObject = static_cast<IUnknown*>(this);
      return S_OK;
   }

   if (riid == __uuidof(IWiaEventCallback))
   {
      *ppvObject = static_cast<IWiaEventCallback*>(this);
      return S_OK;
   }

   *ppvObject = nullptr;
   return E_NOINTERFACE;
}

ULONG Ref::AddRef()
{
   return 1;
}

ULONG Ref::Release()
{
   return 1;
}

HRESULT Ref::ImageEventCallback(
   const GUID* pEventGUID,
   BSTR bstrEventDescription,
   BSTR bstrDeviceID,
   BSTR bstrDeviceDescription,
   DWORD dwDeviceType,
   BSTR bstrFullItemName,
   ULONG* pulEventType,
   ULONG ulReserved)
{
   pulEventType;
   ulReserved;

   LOG_TRACE(
      _T("IWiaEventCallback::ImageEventCallback: Desc=%s, DeviceId=%s, DeviceDesc=%s, DeviceType=%08x, FullItemName=%s\n"),
      CString(bstrEventDescription).GetString(),
      CString(bstrDeviceID).GetString(),
      CString(bstrDeviceDescription).GetString(),
      dwDeviceType,
      CString(bstrFullItemName).GetString());

   if (IsEqualGUID(*pEventGUID, WIA_EVENT_DEVICE_CONNECTED) ||
      IsEqualGUID(*pEventGUID, WIA_EVENT_DEVICE_DISCONNECTED))
   {
      LightweightMutex::LockType lock(m_mtxFnOnCameraConnected);

      if (m_fnOnCameraConnected != nullptr)
      {
         m_fnOnCameraConnected();
      }
   }

   return S_OK;
}
