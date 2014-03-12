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

//
// SDKInstance
//

SDKInstance::SDKInstance()
{
   // start SDK
   EdsError err = EdsInitializeSDK();
   LOG_TRACE(_T("EdsInitializeSDK() returned %08x\n"), err);
   CheckError(_T("EdsInitializeSDK"), err, __FILE__, __LINE__);
}

SDKInstance::~SDKInstance() throw()
{
   // end SDK
   EdsError err = EdsTerminateSDK();
   LOG_TRACE(_T("EdsTerminateSDK() returned %08x\n"), err);
}


//
// Ref
//

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
   LOG_TRACE(_T("EdsGetCameraList(&ref = %08x) returned %08x\n"), hCameraListRef.Get(), err);
   CheckError(_T("EdsGetCameraList"), err, __FILE__, __LINE__);

   EdsUInt32 uiCount = 0;
   err = EdsGetChildCount(hCameraListRef, &uiCount);
   LOG_TRACE(_T("EdsGetChildCount(ref = %08x, count = %u) returned %08x\n"),
      hCameraListRef.Get(), uiCount, err);
   CheckError(_T("EdsGetChildCount"), err, __FILE__, __LINE__);

   for (EdsUInt32 ui = 0; ui<uiCount; ui++)
   {
      EDSDK::Handle hCameraRef(const_cast<Ref*>(this)->shared_from_this());

      EdsGetChildAtIndex(hCameraListRef, static_cast<EdsInt32>(ui), &hCameraRef);
      LOG_TRACE(_T("EdsGetChildAtIndex(ref = %08x, idx = %u, &camRef = %08x) returned %08x\n"),
         hCameraListRef.Get(), ui, hCameraRef.Get(), err);
      EDSDK::CheckError(_T("EdsGetChildAtIndex"), err, __FILE__, __LINE__);

      std::shared_ptr<EDSDK::SourceInfoImpl> spImpl(new EDSDK::SourceInfoImpl(hCameraRef));

      err = EdsGetDeviceInfo(hCameraRef, &spImpl->GetDeviceInfo());
      LOG_TRACE(_T("EdsGetDeviceInfo(ref = %08x, &name = \"%hs\") returned %08x\n"),
         hCameraRef.Get(), spImpl->GetDeviceInfo().szDeviceDescription, err);
      EDSDK::CheckError(_T("EdsGetDeviceInfo"), err, __FILE__, __LINE__);

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
      LOG_TRACE(_T("EdsSetCameraAddedHandler(&handler, this) returned %08x\n"), err);
      EDSDK::CheckError(_T("EdsSetCameraAddedHandler"), err, __FILE__, __LINE__);
   }
   else
   {
      // stop waiting for camera
      EdsError err = EdsSetCameraAddedHandler(nullptr, nullptr);
      LOG_TRACE(_T("EdsSetCameraAddedHandler(nullptr, nullptr) returned %08x\n"), err);
      EDSDK::CheckError(_T("EdsSetCameraAddedHandler"), err, __FILE__, __LINE__);
   }
}

void Ref::OnIdle()
{
   EdsError err = EdsGetEvent();
   if (err != EDS_ERR_OK && err != EDS_ERR_INTERNAL_ERROR)
      LOG_TRACE(_T("EdsGetEvent() returned %08x\n"), err);
}

void EDSDK::MsgWaitForEvent(Event& evt)
{
   LOG_TRACE(_T("MsgWaitForEvent started\n"));

   bool bEventSignaled = false;
   do
   {
      HANDLE h = evt.Handle();
      DWORD dwTimeout = 100;
      DWORD dwRet = ::MsgWaitForMultipleObjects(1, &h, FALSE, dwTimeout, QS_ALLINPUT);

      if (WAIT_TIMEOUT == dwRet)
      {
         EDSDK::Ref::OnIdle();
         continue;
      }

      if (WAIT_OBJECT_0 == dwRet)
         bEventSignaled = true;

      // process window messages
      MSG msg = {0};
      for (;;)
      {
         if (::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
         {
            BOOL bRet = ::GetMessage(&msg, nullptr, 0, 0);
            if(!bRet)
            {
               // WM_QUIT: repost message, exit loop
               PostQuitMessage(msg.wParam);
               break;
            }

            //LOG_TRACE(_T("MsgWaitForEvent: HWND=%08x MSG=%04x\n"), msg.hwnd, msg.message);

            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
         }
         else
            break;
      }

   } while (!bEventSignaled);

   LOG_TRACE(_T("MsgWaitForEvent finished\n"));
}
