//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PsrecCommon.cpp PS-ReC common header
//

// includes
#include "stdafx.h"
#include "PsrecCommon.hpp"
#include "PsrecSourceInfoImpl.hpp"

using PSREC::Ref;
using PSREC::CheckError;

void PSREC::CheckError(const CString& cszFunction, prResponse err, LPCSTR pszFile, UINT uiLine)
{
   if (err == prOK)
      return;

   prResponse componentId = err & prERROR_COMPONENTID_MASK;
   prResponse errorId = err & prERROR_ERRORID_MASK;

   CString cszMessage;
   cszMessage.Format(_T("Error in function \"%s\": %s, %s (%08x)"),
      cszFunction.GetString(),
      componentId == prERROR_PTP_COMPONENTID ? _T("PTP") :
      componentId == prERROR_PRSDK_COMPONENTID ? _T("PRSDK") :
      componentId == prERROR_WIA_STI_COMPONENTID ? _T("WIA/STI") :
      componentId == prERROR_WINDOWS_COMPONENTID ? _T("Windows") :
      componentId == prERROR_COMIF_COMPONENTID ? _T("COM") : _T("???"),
      ErrorTextFromErrorId(errorId, true),
      err);

   throw CameraException(cszFunction, cszMessage, err, pszFile, uiLine);
}

Ref::Ref()
{
   // may return prNOT_SUPPORTED when OS is not supported
   // may return prMEM_ALLOC_FAILED when insufficient memory
   // may return prINTERNAL_ERROR on internal error
   // may also return Win32 error value
   prResponse err = PR_StartSDK();

   LOG_TRACE(_T("PR_StartSDK() returned %08x\n"), err);
   CheckError(_T("PR_StartSDK"), err, __FILE__, __LINE__);
}

Ref::~Ref()
{
   // may return prINVALID_FN_CALL
   // may also return Win32 error value
   prResponse err = PR_FinishSDK();

   // can only trace here
   LOG_TRACE(_T("PR_FinishSDK() returned %08x\n"), err);
}

void Ref::AddVersionText(CString& cszVersionText) const
{
   cszVersionText += _T("PS-ReC (Powershot Remote Capture SDK)\n");

   // get size of buffer
   prUInt32 uiSize = 0;
   prResponse err = PR_GetDllsVersion(&uiSize, nullptr);
   if ((err & prERROR_ERRORID_MASK) != prINSUFFICIENT_BUFFER)
   {
      LOG_TRACE(_T("PR_GetDllsVersion(0, nullptr) returned %08x, buffer size %u\n"), err, uiSize);
      CheckError(_T("PR_GetDllsVersion"), err, __FILE__, __LINE__);
      return;
   }

   if (uiSize == 0)
      return;

   std::vector<BYTE> vecBuffer(uiSize);
   prDllsVerInfo* pVerInfo = reinterpret_cast<prDllsVerInfo*>(&vecBuffer[0]);

   // may return prINVALID_PARAMETER, prMEM_ALLOC_FAILED, prINSUFFICIENT_BUFFER or @ERR
   err = PR_GetDllsVersion(&uiSize, pVerInfo);
   LOG_TRACE(_T("PR_GetDllsVersion(size = %u, buffer) returned %08x\n"), uiSize, err);
   CheckError(_T("PR_GetDllsVersion"), err, __FILE__, __LINE__);

   prDllsVerInfo& allVersionInfos = *pVerInfo;
   for (prUInt32 ui=0; ui<allVersionInfos.Entry; ui++)
   {
      prVerInfo& versionInfo = allVersionInfos.VerInfo[ui];

      LOG_TRACE(_T("%u: ModuleName=%ls, Version=%ls\n"),
         ui,
         versionInfo.ModuleName,
         versionInfo.Version
      );

      CString cszModuleName(reinterpret_cast<LPCWSTR>(versionInfo.ModuleName));
      int iPos = cszModuleName.ReverseFind(_T('\\'));
      if (iPos != -1)
         cszModuleName = cszModuleName.Mid(iPos+1);

      CString cszTemp;
      cszTemp.Format(_T("   %s: %ls\n"),
         cszModuleName.GetString(),
         versionInfo.Version);

      // version numbers are separated with comma; use points instead
      cszTemp.Replace(_T(", "), _T("."));

      cszVersionText += cszTemp;
   }

   cszVersionText += _T("\n");
}

void Ref::EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& vecSourceDevices) const
{
   // get size of buffer
   prUInt32 uiSize = 0;
   prResponse err = PR_GetDeviceList(&uiSize, nullptr);
   if ((err & prERROR_ERRORID_MASK) != prINSUFFICIENT_BUFFER)
   {
      LOG_TRACE(_T("PR_GetDeviceList(0, nullptr) returned %08x, buffer size %u\n"), err, uiSize);
      CheckError(_T("PR_GetDeviceList"), err, __FILE__, __LINE__);
      return;
   }

   if (uiSize == 0)
      return;

   std::vector<BYTE> vecBuffer(uiSize);
   prDeviceList* pDeviceList = reinterpret_cast<prDeviceList*>(&vecBuffer[0]);

   // may return prINVALID_FN_CALL, prINVALID_PARAMETER, prMEM_ALLOC_FAILED, prINSUFFICIENT_BUFFER, prINTERNAL_ERROR or @ERR
   err = PR_GetDeviceList(&uiSize, pDeviceList);
   if (uiSize != 4 && err != prOK) // only log non-empty lists
      LOG_TRACE(_T("PR_GetDeviceList(size = %u, buffer) returned %08x\n"), uiSize, err);
   CheckError(_T("PR_GetDeviceList"), err, __FILE__, __LINE__);

   prDeviceList& deviceList = *pDeviceList;
   for (prUInt32 ui=0; ui<deviceList.NumList; ui++)
   {
      prDeviceInfoTable& deviceInfo = deviceList.DeviceInfo[ui];

      LOG_TRACE(_T("device %u, ModelName=%ls, InternalName=%ls, Generation=%u, ModelID=%08x, PortType=%s\n"),
         ui,
         deviceInfo.ModelName,
         deviceInfo.DeviceInternalName,
         prSUB_GENERATION_CAMERA(deviceInfo.Generation),
         deviceInfo.ModelID,
         deviceInfo.PortType == 0x01 ? _T("WIA") : deviceInfo.PortType == 0x02 ? _T("STI") : _T("???")
      );

      RefSp ref = const_cast<Ref*>(this)->shared_from_this();
      std::shared_ptr<SourceInfo> spSourceInfo(
         new SourceInfoImpl(ref, deviceInfo));

      vecSourceDevices.push_back(spSourceInfo);
   }
}
