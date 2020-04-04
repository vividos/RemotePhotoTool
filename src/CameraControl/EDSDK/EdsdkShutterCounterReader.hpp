//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file EdsdkShutterCounterReader.hpp EDSDK - Shutter counter reader
//
#pragma once

// includes
#include "EDSDK.h"
#include <memory>

namespace EDSDK
{

/// class to read shutter counter value from EOS cameras
class ShutterCounterReader
{
public:
   /// ctor
   ShutterCounterReader();
   /// dtor
   ~ShutterCounterReader();

   /// reads shutter counter for camera with given device id
   bool Read(LPCSTR pszDeviceId, unsigned int& uiShutterCounter) const;

private:
   typedef EdsError(EDSSTDCALL *T_fnEdsInitializeSDK)(); ///< EdsInitializeSDK function type
   typedef EdsError(EDSSTDCALL *T_fnEdsTerminateSDK)(); ///< EdsTerminateSDK function type
   typedef EdsError(EDSSTDCALL *T_fnEdsGetCameraList)(EdsCameraListRef*); ///< EdsGetCameraList function type
   typedef EdsError(EDSSTDCALL *T_fnEdsGetChildCount)(EdsBaseRef, EdsUInt32*); ///< EdsGetChildCount function type
   typedef EdsError(EDSSTDCALL *T_fnEdsGetChildAtIndex)(EdsBaseRef, EdsInt32, EdsBaseRef*); ///< EdsGetChildAtIndex function type
   typedef EdsError(EDSSTDCALL *T_fnEdsGetDeviceInfo)(EdsCameraRef, EdsDeviceInfo*); ///< EdsGetDeviceInfo function type
   typedef EdsError(EDSSTDCALL *T_fnEdsOpenSession)(EdsCameraRef); ///< EdsOpenSession function type
   typedef EdsError(EDSSTDCALL *T_fnEdsCloseSession)(EdsCameraRef); ///< EdsCloseSession function type
   typedef EdsError(EDSSTDCALL *T_fnEdsGetPropertySize)(EdsBaseRef, EdsPropertyID, EdsInt32, EdsDataType*, EdsUInt32*); ///< EdsGetPropertySize function type
   typedef EdsError(EDSSTDCALL *T_fnEdsGetPropertyData)(EdsBaseRef, EdsPropertyID, EdsInt32, EdsUInt32, EdsVoid*); ///< EdsGetPropertyData function type
   typedef EdsError(EDSSTDCALL *T_fnEdsRelease)(EdsBaseRef); ///< EdsRelease function type

   std::shared_ptr<HINSTANCE__> m_spLibrary; ///< DLL handle to EDSDK_25.dll

   T_fnEdsInitializeSDK m_fnEdsInitializeSDK;      ///< EdsInitializeSDK function
   T_fnEdsTerminateSDK m_fnEdsTerminateSDK;        ///< EdsTerminateSDK function
   T_fnEdsGetCameraList m_fnEdsGetCameraList;      ///< EdsGetCameraList function
   T_fnEdsGetChildCount m_fnEdsGetChildCount;      ///< EdsGetChildCount function
   T_fnEdsGetChildAtIndex m_fnEdsGetChildAtIndex;  ///< EdsGetChildAtIndex function
   T_fnEdsGetDeviceInfo m_fnEdsGetDeviceInfo;      ///< EdsGetDeviceInfo function
   T_fnEdsOpenSession m_fnEdsOpenSession;          ///< EdsOpenSession function
   T_fnEdsCloseSession m_fnEdsCloseSession;        ///< EdsCloseSession function
   T_fnEdsGetPropertySize m_fnEdsGetPropertySize;  ///< EdsGetPropertySize function
   T_fnEdsGetPropertyData m_fnEdsGetPropertyData;  ///< EdsGetPropertyData function
   T_fnEdsRelease m_fnEdsRelease;                  ///< EdsRelease function
};

} // namespace EDSDK
