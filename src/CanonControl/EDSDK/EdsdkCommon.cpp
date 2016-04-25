//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file EdsdkCommon.cpp EDSDK - Common functions
//

// includes
#include "stdafx.h"
#include "EdsdkCommon.hpp"
#include "EdsdkSourceInfoImpl.hpp"

#pragma comment(lib, "version.lib")

using namespace EDSDK;

void EDSDK::CheckError(const CString& cszFunction, EdsError err, LPCSTR pszFile, UINT uiLine)
{
   if (err == EDS_ERR_OK)
      return;

   EdsError componentId = err & EDS_COMPONENTID_MASK;
   EdsError errorId = err & EDS_ERRORID_MASK;

   CString cszMessage;
   cszMessage.Format(_T("Error in function \"%s\": %s, %s%s (%08x)"),
      cszFunction.GetString(),
      componentId == 0 ? _T("SDK") :
      componentId == EDS_CMP_ID_CLIENT_COMPONENTID ? _T("Client") :
      componentId == EDS_CMP_ID_LLSDK_COMPONENTID ? _T("LLSDK") :
      componentId == EDS_CMP_ID_HLSDK_COMPONENTID ? _T("HLSDK") : _T("???"),
      ErrorTextFromErrorId(errorId),
      (err & EDS_ISSPECIFIC_MASK) != 0 ? _T(", IsSpecific") : _T(""),
      err);

   throw CameraException(cszFunction, cszMessage, err, pszFile, uiLine);
}


//
// Ref
//

Ref::Ref()
{
   EdsError err = EdsInitializeSDK();
   LOG_TRACE(_T("EdsInitializeSDK() returned %08x\n"), err);
   CheckError(_T("EdsInitializeSDK"), err, __FILE__, __LINE__);
}

Ref::~Ref() throw()
{
   EdsError err = EdsTerminateSDK();
   LOG_TRACE(_T("EdsTerminateSDK() returned %08x\n"), err);
}

void Ref::AddVersionText(CString& cszVersionText) const
{
   cszVersionText += _T("ED-SDK (Canon EOS Digital SDK)\n");

   LPCTSTR pszEdsdkDll = _T("EDSDK.dll");

   DWORD dwSize = GetFileVersionInfoSize(pszEdsdkDll, nullptr);
   if (dwSize == 0)
      return;

   std::vector<BYTE> vecVersionInfo(dwSize);
   GetFileVersionInfo(pszEdsdkDll, 0, dwSize, vecVersionInfo.data());

   LPVOID pFileInfo = nullptr;
   UINT uiSize = 0;
   if (FALSE == VerQueryValue(vecVersionInfo.data(), _T("\\"), &pFileInfo, &uiSize))
      return;

   VS_FIXEDFILEINFO& fileInfo = *reinterpret_cast<VS_FIXEDFILEINFO*>(pFileInfo);

   CString cszTemp;
   cszTemp.Format(_T("   EDSDK.dll: %u.%u.%u.%u\n\n"),
      HIWORD(fileInfo.dwFileVersionMS), LOWORD(fileInfo.dwFileVersionMS),
      HIWORD(fileInfo.dwFileVersionLS), LOWORD(fileInfo.dwFileVersionLS));

   cszVersionText += cszTemp;
}

void Ref::EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& vecSourceDevices) const
{
   Handle hCameraListRef;

   EdsError err = EdsGetCameraList(&hCameraListRef);
   if (err != EDS_ERR_OK) LOG_TRACE(_T("EdsGetCameraList(&ref = %08x) returned %08x\n"), hCameraListRef.Get(), err);
   CheckError(_T("EdsGetCameraList"), err, __FILE__, __LINE__);

   EdsUInt32 uiCount = 0;
   err = EdsGetChildCount(hCameraListRef, &uiCount);
   if (err != EDS_ERR_OK) LOG_TRACE(_T("EdsGetChildCount(ref = %08x, count = %u) returned %08x\n"),
      hCameraListRef.Get(), uiCount, err);
   CheckError(_T("EdsGetChildCount"), err, __FILE__, __LINE__);

   for (EdsUInt32 ui = 0; ui<uiCount; ui++)
   {
      EDSDK::Handle hCameraRef(const_cast<Ref*>(this)->shared_from_this());

      EdsGetChildAtIndex(hCameraListRef, static_cast<EdsInt32>(ui), &hCameraRef);
      if (err != EDS_ERR_OK) LOG_TRACE(_T("EdsGetChildAtIndex(ref = %08x, idx = %u, &camRef = %08x) returned %08x\n"),
         hCameraListRef.Get(), ui, hCameraRef.Get(), err);
      EDSDK::CheckError(_T("EdsGetChildAtIndex"), err, __FILE__, __LINE__);

      std::shared_ptr<EDSDK::SourceInfoImpl> spImpl(new EDSDK::SourceInfoImpl(hCameraRef));

      EdsDeviceInfo& deviceInfo = spImpl->GetDeviceInfo();

      err = EdsGetDeviceInfo(hCameraRef, &deviceInfo);
      if (err != EDS_ERR_OK) LOG_TRACE(_T("EdsGetDeviceInfo(ref = %08x, &name = \"%hs\") returned %08x\n"),
         hCameraRef.Get(), deviceInfo.szDeviceDescription, err);
      EDSDK::CheckError(_T("EdsGetDeviceInfo"), err, __FILE__, __LINE__);

      LOG_TRACE(_T("device %u, Desc=%hs, Port=%hs, PortType=%s\n"),
         ui,
         deviceInfo.szDeviceDescription,
         deviceInfo.szPortName,
         deviceInfo.deviceSubType == 0x00 ? _T("WIA") :
         deviceInfo.deviceSubType == 0x01 ? _T("Canon PTP") :
         deviceInfo.deviceSubType == 0x02 ? _T("Canon PTP-IP") : _T("???")
         );

      vecSourceDevices.push_back(spImpl);
   }
}

EdsError EDSCALLBACK Ref::OnCameraAddedHandler(EdsVoid* inContext)
{
   Ref* pT = reinterpret_cast<Ref*>(inContext);
   ATLASSERT(pT != nullptr);

   LightweightMutex::LockType lock(pT->m_mtxFnOnCameraConnected);

   if (pT->m_fnOnCameraConnected != nullptr)
      pT->m_fnOnCameraConnected();

   return EDS_ERR_OK;
}

void Ref::AsyncWaitForCamera(bool bStart, std::function<void()> fnOnCameraConnected)
{
   {
      LightweightMutex::LockType lock(m_mtxFnOnCameraConnected);

      m_fnOnCameraConnected = fnOnCameraConnected;
   }

   if (bStart)
   {
      EdsError err = EdsSetCameraAddedHandler(&Ref::OnCameraAddedHandler, this);
      if (err != EDS_ERR_OK) LOG_TRACE(_T("EdsSetCameraAddedHandler(&handler, this) returned %08x\n"), err);
      EDSDK::CheckError(_T("EdsSetCameraAddedHandler"), err, __FILE__, __LINE__);
   }
   else
   {
      // stop waiting for camera
      EdsError err = EdsSetCameraAddedHandler(nullptr, nullptr);
      if (err != EDS_ERR_OK) LOG_TRACE(_T("EdsSetCameraAddedHandler(nullptr, nullptr) returned %08x\n"), err);
      EDSDK::CheckError(_T("EdsSetCameraAddedHandler"), err, __FILE__, __LINE__);
   }
}

void Ref::OnIdle()
{
   MSG msg = { 0 };
   const unsigned int c_uiMaxMessages = 100;
   unsigned int uiCountMessages = 0;
   while (::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE) &&
      uiCountMessages++ < c_uiMaxMessages)
   {
      // don't process quit messages; should be handled by our application
      if (msg.message == WM_QUIT)
         break;

      // EdsGetEvent() processes thread messages inside EDSDK
      EdsError err = EdsGetEvent();
      if (err != EDS_ERR_OK && err != EDS_ERR_INTERNAL_ERROR)
         LOG_TRACE(_T("EdsGetEvent() returned %08x\n"), err);
   }
}

void EDSDK::MsgWaitForEvent(Event& evt) throw()
{
   LOG_TRACE(_T("MsgWaitForEvent started\n"));

   EDSDK::Ref::OnIdle();

   HANDLE h = evt.Handle();
   DWORD dwTimeout = 10;

   for (;;)
   {
      DWORD dwRet = ::MsgWaitForMultipleObjects(1, &h, FALSE, dwTimeout, QS_ALLINPUT);

      if (WAIT_TIMEOUT == dwRet)
      {
         EDSDK::Ref::OnIdle();
         continue;
      }

      if (WAIT_OBJECT_0 == dwRet)
         break;
   }

   EDSDK::Ref::OnIdle();

   LOG_TRACE(_T("MsgWaitForEvent finished\n"));
}
