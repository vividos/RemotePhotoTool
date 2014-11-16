//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
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
   ~ShutterCounterReader() throw();

   /// reads shutter counter for camera with given device id
   bool Read(LPCSTR pszDeviceId, unsigned int& uiShutterCounter) const;

private:
   typedef EdsError(EDSSTDCALL *T_fnEdsInitializeSDK)();
   typedef EdsError(EDSSTDCALL *T_fnEdsTerminateSDK)();
   typedef EdsError(EDSSTDCALL *T_fnEdsGetCameraList)(EdsCameraListRef*);
   typedef EdsError(EDSSTDCALL *T_fnEdsGetChildCount)(EdsBaseRef, EdsUInt32*);
   typedef EdsError(EDSSTDCALL *T_fnEdsGetChildAtIndex)(EdsBaseRef, EdsInt32, EdsBaseRef*);
   typedef EdsError(EDSSTDCALL *T_fnEdsGetDeviceInfo)(EdsCameraRef, EdsDeviceInfo*);
   typedef EdsError(EDSSTDCALL *T_fnEdsOpenSession)(EdsCameraRef);
   typedef EdsError(EDSSTDCALL *T_fnEdsCloseSession)(EdsCameraRef);
   typedef EdsError(EDSSTDCALL *T_fnEdsGetPropertySize)(EdsBaseRef, EdsPropertyID, EdsInt32, EdsDataType*, EdsUInt32*);
   typedef EdsError(EDSSTDCALL *T_fnEdsGetPropertyData)(EdsBaseRef, EdsPropertyID, EdsInt32, EdsUInt32, EdsVoid*);
   typedef EdsError(EDSSTDCALL *T_fnEdsRelease)(EdsBaseRef);

   std::shared_ptr<HINSTANCE__> m_spLibrary;
   T_fnEdsInitializeSDK m_fnEdsInitializeSDK;
   T_fnEdsTerminateSDK m_fnEdsTerminateSDK;
   T_fnEdsGetCameraList m_fnEdsGetCameraList;
   T_fnEdsGetChildCount m_fnEdsGetChildCount;
   T_fnEdsGetChildAtIndex m_fnEdsGetChildAtIndex;
   T_fnEdsGetDeviceInfo m_fnEdsGetDeviceInfo;
   T_fnEdsOpenSession m_fnEdsOpenSession;
   T_fnEdsCloseSession m_fnEdsCloseSession;
   T_fnEdsGetPropertySize m_fnEdsGetPropertySize;
   T_fnEdsGetPropertyData m_fnEdsGetPropertyData;
   T_fnEdsRelease m_fnEdsRelease;
};

} // namespace EDSDK
