//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file EdsdkRemoteReleaseControlImpl.cpp EDSDK - RemoteReleaseControl impl
//

// includes
#include "stdafx.h"
#include "EdsdkShutterCounterReader.hpp"

using namespace EDSDK;

namespace
{
/// shutter counter property; can't be read out with newest SDK anymore
#define kEdsPropID_ShutterCounter 0x00000022

/// EDSDK handle
template <typename THandle>
class Handle
{
public:
   /// EdsRelease() function type
   typedef EdsError(EDSSTDCALL *T_fnEdsRelease)(EdsBaseRef);

   /// ctor
   Handle(T_fnEdsRelease fnEdsRelease, THandle handle = nullptr)
      :m_handle(handle),
      m_fnEdsRelease(fnEdsRelease)
   {
   }

   /// dtor
   ~Handle()
   {
      if (m_handle != nullptr)
         m_fnEdsRelease(m_handle);
   }

   /// address-of operator
   EdsBaseRef* operator&() throw() { return &m_handle; }

   /// value operator for EdsBaseRef type common to all handles
   operator EdsBaseRef() const { return m_handle; }

private:
   /// handle
   THandle m_handle;

   /// release function
   T_fnEdsRelease m_fnEdsRelease;
};

} // unnamed namespace

ShutterCounterReader::ShutterCounterReader()
{
   HMODULE hMod = LoadLibrary(_T("EDSDK_25.dll"));
   if (hMod != nullptr)
      m_spLibrary.reset(hMod, ::FreeLibrary);

   m_fnEdsInitializeSDK = (T_fnEdsInitializeSDK)GetProcAddress(hMod, "EdsInitializeSDK");
   m_fnEdsTerminateSDK = (T_fnEdsTerminateSDK)GetProcAddress(hMod, "EdsTerminateSDK");
   m_fnEdsGetCameraList = (T_fnEdsGetCameraList)GetProcAddress(hMod, "EdsGetCameraList");
   m_fnEdsGetChildCount = (T_fnEdsGetChildCount)GetProcAddress(hMod, "EdsGetChildCount");
   m_fnEdsGetChildAtIndex = (T_fnEdsGetChildAtIndex)GetProcAddress(hMod, "EdsGetChildAtIndex");
   m_fnEdsGetDeviceInfo = (T_fnEdsGetDeviceInfo)GetProcAddress(hMod, "EdsGetDeviceInfo");
   m_fnEdsOpenSession = (T_fnEdsOpenSession)GetProcAddress(hMod, "EdsOpenSession");
   m_fnEdsCloseSession = (T_fnEdsCloseSession)GetProcAddress(hMod, "EdsCloseSession");
   m_fnEdsGetPropertySize = (T_fnEdsGetPropertySize)GetProcAddress(hMod, "EdsGetPropertySize");
   m_fnEdsGetPropertyData = (T_fnEdsGetPropertyData)GetProcAddress(hMod, "EdsGetPropertyData");
   m_fnEdsRelease = (T_fnEdsRelease)GetProcAddress(hMod, "EdsRelease");

   if (m_fnEdsInitializeSDK != nullptr)
      m_fnEdsInitializeSDK();
}

ShutterCounterReader::~ShutterCounterReader() throw()
{
   if (m_fnEdsTerminateSDK != nullptr)
      m_fnEdsTerminateSDK();
}

bool ShutterCounterReader::Read(LPCSTR pszDeviceId, unsigned int& uiShutterCounter) const
{
   if (m_spLibrary == nullptr)
      return false;

   // Get camera list
   Handle<EdsCameraListRef> cameraList(m_fnEdsRelease);
   EdsError err = m_fnEdsGetCameraList(&cameraList);

   // Get number of cameras
   if (err != EDS_ERR_OK)
      return false;

   EdsUInt32 count = 0;
   err = m_fnEdsGetChildCount(cameraList, &count);
   if (err != EDS_ERR_OK || count == 0)
      return false;

   for (EdsUInt32 i = 0; i < count; i++)
   {
      Handle<EdsCameraRef> camera(m_fnEdsRelease);
      err = m_fnEdsGetChildAtIndex(cameraList, 0, &camera);

      if (err != EDS_ERR_OK)
         continue;

      EdsDeviceInfo deviceInfo = { 0 };
      err = m_fnEdsGetDeviceInfo(camera, &deviceInfo);
      if (err != EDS_ERR_OK)
         continue;

      // when device id wasn't passed, use first camera
      // when passed, check if it's the one we're looking for
      if (pszDeviceId == nullptr || strlen(pszDeviceId) == 0 ||
          CStringA(deviceInfo.szPortName) == pszDeviceId)
      {
         err = m_fnEdsOpenSession(camera);
         if (err != EDS_ERR_OK)
            continue;

         //std::shared_ptr<void> spSession(camera, m_fnEdsCloseSession);

         EdsDataType dataType = kEdsDataType_Unknown;
         EdsUInt32 dataSize = 0;
         err = m_fnEdsGetPropertySize(camera, kEdsPropID_ShutterCounter, 0, &dataType, &dataSize);
         if (err != EDS_ERR_OK || dataSize != 4)
            continue;

         err = m_fnEdsGetPropertyData(camera, kEdsPropID_ShutterCounter, 0, dataSize, &uiShutterCounter);
         if (err != EDS_ERR_OK)
            continue;

         return true;
      }
   }

   return false;
}
