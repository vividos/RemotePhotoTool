//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file CameraConnectionManager.cpp Camera connection manager
//

// includes
#include "stdafx.h"
#include "CameraConnectionManager.hpp"
#include "SourceDevice.hpp"
#include "RemoteReleaseControl.hpp"
#include "CameraErrorDlg.hpp"

bool CameraConnectionManager::Connect(HWND hWnd, std::shared_ptr<SourceDevice> spSourceDevice)
{
   Disconnect();

   if (spSourceDevice == nullptr)
      return false;

   m_spSourceDevice = spSourceDevice;

   try
   {
      m_spRemoteReleaseControl = m_spSourceDevice->EnterReleaseControl();
   }
   catch (const CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't start remote release connection"), ex);
      dlg.DoModal(hWnd);

      m_spRemoteReleaseControl.reset();
   }

   return IsConnected();
}

void CameraConnectionManager::Disconnect()
{
   m_spRemoteReleaseControl.reset();
   m_spSourceDevice.reset();
}

std::shared_ptr<Viewfinder> CameraConnectionManager::StartViewfinder(HWND hWnd)
{
   bool bViewFinderAvail =
      IsConnected() &&
      GetRemoteReleaseControl()->GetCapability(RemoteReleaseControl::capViewfinder);

   if (!bViewFinderAvail)
      return nullptr;

   try
   {
      std::shared_ptr<Viewfinder> spViewfinder = m_spRemoteReleaseControl->StartViewfinder();

      return spViewfinder;
   }
   catch (const CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't start viewfinder"), ex);
      dlg.DoModal(hWnd);

      return nullptr;
   }
}
