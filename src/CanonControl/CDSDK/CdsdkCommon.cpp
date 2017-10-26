//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file CdsdkCommon.cpp CDSDK - Common functions
//

// includes
#include "stdafx.h"
#include "CdsdkCommon.hpp"
#include "CameraException.hpp"
#include "CdsdkSourceInfoImpl.hpp"
#include "ErrorText.hpp"
#include <ulib/config/Wtl.hpp> // for RunTimeHelper

using namespace CDSDK;

void CDSDK::CheckError(const CString& cszFunction, cdError err, LPCSTR pszFile, UINT uiLine)
{
   if (err == cdOK)
      return;

   cdError componentId = err & cdERROR_COMPONENTID_MASK;
   cdError errorId = err & cdERROR_ERRORID_MASK;

   CString cszMessage;
   cszMessage.Format(_T("Error in function \"%s\": %s, %s%s (%08x)"),
      cszFunction.GetString(),
      componentId == cdERROR_CLIENT_COMPONENTID ? _T("Client") :
      componentId == cdERROR_LLSDK_COMPONENTID ? _T("LLSDK") :
      componentId == cdERROR_HLSDK_COMPONENTID ? _T("HLSDK") :
      componentId == cdERROR_PROPERTY_PARSING_COMPONENTID ? _T("PropertyParsing") :
      componentId == cdERROR_VIEW_DEVELOPMENT_COMPONENTID ? _T("ViewDevelopment") :
      componentId == cdERROR_VIEW_DECODING_COMPONENTID ? _T("ViewDecoding") :
      componentId == cdERROR_COLOR_MAPPING_COMPONENTID ? _T("ColorMapping") :
      componentId == cdERROR_PICTURE_COLLECTION_COMPONENTID ? _T("PictureCollection") :
      componentId == cdERROR_SETUP_COMPONENTID ? _T("Setup") :
      componentId == cdERROR_IWRAP_COMPONENTID ? _T("IWrap") :
      componentId == cdERROR_PSUSD_COMPONENTID ? _T("PSUSD") :
      componentId == cdERROR_CDSDK_COMPONENTID ? _T("CDSDK") :
      componentId == cdERROR_RDSDK_COMPONENTID ? _T("RDSDK") :  _T("???"),
      ErrorTextFromErrorId(errorId),
      (err & cdERROR_ISSPECIFIC_MASK) != 0 ? _T(", IsSpecific") : _T(""),
      err);

   throw CameraException(cszFunction, cszMessage, err, pszFile, uiLine);
}

Ref::Ref()
{
   cdVersionInfo version = {0};
   version.Size = sizeof(version);
   version.MajorVersion = 7;
   version.MinorVersion = 3;

   cdUInt32 option = 0; // reserved

   // may return cdINVALID_PARAMETER when version mismatch with dlls used
   // may return cdINVALID_FN_CALL when called a second time
   cdError err = CDStartSDK(&version, option);
   LOG_TRACE(_T("CDStartSDK(&version, 0) returned %08x\n"), err);
   CheckError(_T("CDStartSDK"), err, __FILE__, __LINE__);
}

Ref::~Ref() throw()
{
   // may return cdMEM_FREE_FAILED
   cdError err = CDFinishSDK();
   LOG_TRACE(_T("CDFinishSDK() returned %08x\n"), err);
}

void Ref::AddVersionText(CString& cszVersionText) const
{
   cszVersionText += _T("CD-SDK (Canon Digital camera SDK)\n");

   cdVersionInfo versionInfo = { sizeof(versionInfo) };

   // may return cdINVALID_PARAMETER
   cdError err = CDGetSDKVersion(&versionInfo);
   LOG_TRACE(_T("CDGetSDKVersion() returned %08x\n"), err);

   CString cszTemp;
   cszTemp.Format(_T("   CD-SDK %u.%u.%u (%hs)\n"),
      versionInfo.MajorVersion,
      versionInfo.MinorVersion,
      versionInfo.ReleaseVersion,
      versionInfo.chVersion);

   cszVersionText += cszTemp;
   cszVersionText += _T("\n");
}

/// SEH-safe call to CDEnumDeviceReset() function
cdError CDEnumDeviceReset_SEH(cdUInt32 Kind, cdHEnum* phEnum)
{
   __try
   {
      cdError err = CDEnumDeviceReset(Kind, phEnum);
      return err;
   }
   __except(EXCEPTION_EXECUTE_HANDLER)
   {
      DWORD dwCode = GetExceptionCode();
      LOG_TRACE(_T("CDEnumDeviceReset(1, %08x) caused a structured exception: 0x%08x\n"), *phEnum, dwCode);
      return dwCode;
   }
}

void Ref::EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& vecSourceDevices) const
{
   // Note: CDSDK shows wait cursor for a small time, so only enumerate on XP or lower
   // since the camera would't work on Vista and above anyway.
   if (RunTimeHelper::IsVista())
      return;

   cdUInt32 uiCount = 0;
   cdHEnum hEnum = 0;

   // may return cdINVALID_PARAMETER, or any exception code
   cdError err = CDEnumDeviceReset_SEH(1, &hEnum);
   if (err != cdOK) LOG_TRACE(_T("CDEnumDeviceReset(1, %08x) returned %08x\n"), hEnum, err);
   CheckError(_T("CDEnumDeviceReset"), err, __FILE__, __LINE__);

   // returns cdINVALID_HANDLE, cdINVALID_PARAMETER
   err = CDGetDeviceCount(hEnum, &uiCount);
   if (err != cdOK) LOG_TRACE(_T("CDGetDeviceCount(%08x, &count = %u) returned %08x\n"), hEnum, uiCount, err);
   CheckError(_T("CDGetDeviceCount"), err, __FILE__, __LINE__);

   for (cdUInt32 ui=0; ui<uiCount; ui++)
   {
      std::shared_ptr<CDSDK::SourceInfoImpl> spImpl(new SourceInfoImpl(const_cast<Ref*>(this)->shared_from_this()));

      cdSourceInfo& si = spImpl->GetSourceInfo();

      // may return cdINVALID_PARAMETER, cdINVALID_HANDLE, cdENUM_NA
      err = CDEnumDeviceNext(hEnum, &si);
      if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
         break; // done

      if (err != cdOK) LOG_TRACE(_T("CDEnumDeviceNext(%08x, &si = \"%hs\") returned %08x\n"), hEnum, si.Name, err);

      CheckError(_T("CDEnumDeviceNext"), err, __FILE__, __LINE__);

      CString cszDetails;
      switch (si.PortType)
      {
      case cdPORT_TYPE_STI:
         cszDetails.Format(_T(", DataType=%s, DeviceInternalName=%ls"),
            si.u.STI.DataType == cdDEV_DATA_TYPE_UNKNOWN ? _T("Unknown") :
            si.u.STI.DataType == cdDEV_DATA_TYPE_UNDECIDED ? _T("Undecided") :
            si.u.STI.DataType == cdDEV_DATA_TYPE_1 ? _T("Type 1") :
            si.u.STI.DataType == cdDEV_DATA_TYPE_2 ? _T("Type 2") : _T("???"),
            si.u.STI.DeviceInternalName);
         break;

      case cdPORT_TYPE_WIA:
         cszDetails.Format(_T(", DataType=%s, DeviceID=%ls"),
            si.u.WIA.DataType == cdDEV_DATA_TYPE_UNKNOWN ? _T("Unknown") :
            si.u.WIA.DataType == cdDEV_DATA_TYPE_UNDECIDED ? _T("Undecided") :
            si.u.WIA.DataType == cdDEV_DATA_TYPE_1 ? _T("Type 1") :
            si.u.WIA.DataType == cdDEV_DATA_TYPE_2 ? _T("Type 2") : _T("???"),
            si.u.WIA.pDIPDevID);
         break;
      };

      LOG_TRACE(_T("device %u, ModelName=%hs, InternalName=%hs, PortType=%s%s\n"),
         ui,
         si.Name,
         si.NameInOS,
         si.PortType == cdPORT_TYPE_NO_PORT ? _T("None") :
         si.PortType == cdPORT_TYPE_STI ? _T("STI") :
         si.PortType == cdPORT_TYPE_WIA ? _T("WIA") :
         si.PortType == cdPORT_TYPE_ANY ? _T("Any") : _T("???"),
         cszDetails.GetString());

      vecSourceDevices.push_back(spImpl);
   }

   // may return cdINVALID_HANDLE, cdINVALID_FN_CALL
   err = CDEnumDeviceRelease(hEnum);
   if (err != cdOK) LOG_TRACE(_T("CDEnumDeviceRelease(%08x) returned %08x\n"), hEnum, err);
   CheckError(_T("CDEnumDeviceRelease"), err, __FILE__, __LINE__);
}
