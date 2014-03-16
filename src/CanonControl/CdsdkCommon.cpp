//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CdsdkCommon.cpp CDSDK - Common functions
//

// includes
#include "stdafx.h"
#include "CdsdkCommon.hpp"
#include "CameraException.hpp"
#include "CdsdkSourceInfoImpl.hpp"

using namespace CDSDK;

void CDSDK::CheckError(const CString& cszFunction, cdError err, LPCSTR pszFile, UINT uiLine)
{
   if (err != cdOK)
      throw CameraException(cszFunction,
         (err & cdERROR_ISSPECIFIC_MASK) != 0,
         err & cdERROR_COMPONENTID_MASK,
         err & cdERROR_ERRORID_MASK,
         pszFile, uiLine);
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
   cdUInt32 uiCount = 0;
   cdHEnum hEnum = 0;

   // may return cdINVALID_PARAMETER, or any exception code
   cdError err = CDEnumDeviceReset_SEH(1, &hEnum);
   LOG_TRACE(_T("CDEnumDeviceReset(1, %08x) returned %08x\n"), hEnum, err);
   CheckError(_T("CDEnumDeviceReset"), err, __FILE__, __LINE__);

   // returns cdINVALID_HANDLE, cdINVALID_PARAMETER
   err = CDGetDeviceCount(hEnum, &uiCount);
   LOG_TRACE(_T("CDGetDeviceCount(%08x, &count = %u) returned %08x\n"), hEnum, uiCount, err);
   CheckError(_T("CDGetDeviceCount"), err, __FILE__, __LINE__);

   for (cdUInt32 ui=0; ui<uiCount; ui++)
   {
      std::shared_ptr<CDSDK::SourceInfoImpl> spImpl(new SourceInfoImpl(const_cast<Ref*>(this)->shared_from_this()));

      cdSourceInfo& si = spImpl->GetSourceInfo();

      // may return cdINVALID_PARAMETER, cdINVALID_HANDLE, cdENUM_NA
      err = CDEnumDeviceNext(hEnum, &si);
      LOG_TRACE(_T("CDEnumDeviceNext(%08x, &si = \"%hs\") returned %08x\n"), hEnum, si.Name, err);
      if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
         break; // done

      CheckError(_T("CDEnumDeviceNext"), err, __FILE__, __LINE__);

      vecSourceDevices.push_back(spImpl);
   }

   // may return cdINVALID_HANDLE, cdINVALID_FN_CALL
   err = CDEnumDeviceRelease(hEnum);
   LOG_TRACE(_T("CDEnumDeviceRelease(%08x) returned %08x\n"), hEnum, err);
   CheckError(_T("CDEnumDeviceRelease"), err, __FILE__, __LINE__);
}
