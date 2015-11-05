//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PhotoModeManager.cpp Photo mode manager classes
//

// includes
#include "stdafx.h"
#include "PhotoModeManager.hpp"
#include "IPhotoModeViewHost.hpp"
#include "PhotomatixInterface.hpp"
#include "ImageFileManager.hpp"
#include "RemoteReleaseControl.hpp"
#include "ShutterReleaseSettings.hpp"
#include "ViewFinderView.hpp"
#include "CameraErrorDlg.hpp"
#include "ShootingMode.hpp"
#include "ShutterSpeedValue.hpp"
#include "HuginInterface.hpp"

//
// HDRPhotoModeManager
//

bool HDRPhotoModeManager::Init(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
{
   m_spRemoteReleaseControl = spRemoteReleaseControl;

   // set default release settings
   try
   {
      ShutterReleaseSettings& settings = m_host.GetReleaseSettings();

      CString cszFilename =
         m_host.GetImageFileManager().NextFilename(imageTypeHDR);
      settings.Filename(cszFilename);

      m_spRemoteReleaseControl->SetReleaseSettings(settings);
   }
   catch (CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Error while setting shooting settings"), ex);
      dlg.DoModal(m_hWnd);
      return false;
   }

   return true;
}

bool HDRPhotoModeManager::CheckManualMode()
{
   ShootingMode mode(m_spRemoteReleaseControl);

   // shooting mode change supported?
   if (!mode.IsSupportedChanging())
   {
      // no, notify user to switch to M
      ImageProperty shootingModeM = mode.Manual();
      ImageProperty currentShootingMode = mode.Current();

      if (currentShootingMode != shootingModeM)
      {
         AtlMessageBox(m_hWnd, _T("Please switch to Manual mode on camera!"), IDR_MAINFRAME, MB_OK);
         return false;
      }
   }
   else
   {
      // yes, switch to M
      try
      {
         ImageProperty shootingModeM = mode.Manual();
         m_spRemoteReleaseControl->SetImageProperty(shootingModeM);
      }
      catch (CameraException& ex)
      {
         CameraErrorDlg dlg(_T("Couldn't switch to Manual mode"), ex);
         dlg.DoModal(m_hWnd);
         return false;
      }
   }

   return true;
}

void HDRPhotoModeManager::OnReleasedAEBLast()
{
   // close viewfinder, if used at all
   m_spViewfinder.reset();

   m_bAEBInProgress = false;

   // now send images to Photomatix
   PhotomatixInterface pi(m_host.GetAppSettings().m_cszPhotomatixPath);
   if (!pi.IsInstalled())
   {
      AtlMessageBox(m_hWnd, _T("Photomatix is not installed"), IDR_MAINFRAME, MB_OK);
      return;
   }

   m_host.SetStatusText(_T("Starting Photomatix..."));

   pi.RunUI(m_vecAEBFilenameList);

   // enable viewfinder again when active
   if (m_bViewfinderActiveBeforeStart)
   {
      ViewFinderView* pViewfinder = m_host.GetViewFinderView();
      if (pViewfinder != NULL)
         pViewfinder->EnableUpdate(true);

      m_bViewfinderActiveBeforeStart = false;
   }
}

void HDRPhotoModeManager::RecalcAEBShutterSpeedList(size_t uiNumShots)
{
   m_vecAEBShutterSpeedValues.clear();

   unsigned int uiShutterSpeedPropertyId =
      m_spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propTv);

   try
   {
      ImageProperty currentShutterSpeed = m_spRemoteReleaseControl->GetImageProperty(uiShutterSpeedPropertyId);

      ShutterSpeedValue shutterSpeedBase(currentShutterSpeed, m_spRemoteReleaseControl);
      ShutterSpeedValue shutterDown = shutterSpeedBase, shutterUp = shutterSpeedBase;

      for (size_t ui = 1; ui <= uiNumShots; ui++)
      {
         if (ui == 1)
         {
            m_vecAEBShutterSpeedValues.push_back(shutterSpeedBase.Value());
         }
         else
            if ((ui & 1) == 0) // even: 2, 4, 6, ...
            {
            ShutterSpeedValue prev = shutterDown;
            shutterDown.Subtract2EV();

            // limit
            if (prev != shutterDown)
               m_vecAEBShutterSpeedValues.push_back(shutterDown.Value());
            }
            else
               if ((ui & 1) == 1) // odd: 3, 5, 7, ...
               {
                  ShutterSpeedValue prev = shutterUp;
                  shutterUp.Add2EV();

                  // limit
                  if (prev != shutterUp)
                     m_vecAEBShutterSpeedValues.push_back(shutterUp.Value());
               }
      }
   }
   catch (CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't get shutter speed values"), ex);
      dlg.DoModal(m_hWnd);
   }
}

void HDRPhotoModeManager::ReleaseAEBFirst()
{
   // disable viewfinder while shooting
   {
      ViewFinderView* pViewfinder = m_host.GetViewFinderView();

      m_bViewfinderActiveBeforeStart = pViewfinder != NULL;

      if (m_bViewfinderActiveBeforeStart)
         pViewfinder->EnableUpdate(false);
   }

   // action mode is only unlocked when we receive an image
   if (m_host.GetReleaseSettings().SaveTarget() != ShutterReleaseSettings::saveToCamera)
      m_host.LockActionMode(true);

   m_bAEBInProgress = true;
   m_vecAEBFilenameList.clear();
   m_uiCurrentAEBShutterSpeed = 0;

   if (!m_bViewfinderActiveBeforeStart)
   {
      try
      {
         // init viewfinder; this is done to speed up taking images;
         // in future it could be done with mirror lockup
         m_spViewfinder = m_spRemoteReleaseControl->StartViewfinder();
      }
      catch (CameraException& ex)
      {
         CameraErrorDlg dlg(_T("Couldn't start viewfinder"), ex);
         dlg.DoModal(m_hWnd);
      }
   }

   ReleaseAEBNext();
}

void HDRPhotoModeManager::ReleaseAEBNext()
{
   // called when shutter speed was changed successfully
   bool bLastShot = m_uiCurrentAEBShutterSpeed + 1 == m_vecAEBShutterSpeedValues.size();

   ShutterReleaseSettings& settings = m_host.GetReleaseSettings();

   settings.HandlerOnFinishedTransfer(
      bLastShot ?
         std::bind(&HDRPhotoModeManager::OnFinishedTransferLastAEB, this, std::placeholders::_1) :
         std::bind(&HDRPhotoModeManager::OnFinishedTransferNextAEB, this, std::placeholders::_1));

   CString cszFilename =
      m_host.GetImageFileManager().NextFilename(imageTypeHDR, m_uiCurrentAEBShutterSpeed == 0);
   settings.Filename(cszFilename);

   m_spRemoteReleaseControl->SetReleaseSettings(settings);

   // set status text
   CString cszText;
   cszText.Format(_T("Taking picture %Iu of %Iu with shutter speed %s"),
      m_uiCurrentAEBShutterSpeed + 1,
      m_vecAEBShutterSpeedValues.size(),
      m_vecAEBShutterSpeedValues[m_uiCurrentAEBShutterSpeed].AsString().GetString());
   m_host.SetStatusText(cszText);

   try
   {
      m_spRemoteReleaseControl->SetImageProperty(m_vecAEBShutterSpeedValues[m_uiCurrentAEBShutterSpeed]);

      m_spRemoteReleaseControl->Release();

      m_uiCurrentAEBShutterSpeed++;
   }
   catch (CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't release AEB shutter"), ex);
      dlg.DoModal(m_hWnd);
   }
}

void HDRPhotoModeManager::OnFinishedTransferNextAEB(const ShutterReleaseSettings& settings)
{
   // save filename for further processing
   CString cszFilename = settings.Filename();
   m_vecAEBFilenameList.push_back(cszFilename);

   ATLASSERT(m_uiCurrentAEBShutterSpeed < m_vecAEBShutterSpeedValues.size());

   PostMessage(m_hWnd, WM_HDR_AEB_NEXT, 0, 0);
}

void HDRPhotoModeManager::OnFinishedTransferLastAEB(const ShutterReleaseSettings& settings)
{
   // save filename for further processing
   CString cszFilename = settings.Filename();
   m_vecAEBFilenameList.push_back(cszFilename);

   m_host.OnTransferredImage(cszFilename);

   PostMessage(m_hWnd, WM_HDR_AEB_LAST, 0, 0);
}

//
// PanoramaPhotoModeManager
//

bool PanoramaPhotoModeManager::Init(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
{
   m_spRemoteReleaseControl = spRemoteReleaseControl;

   // set default release settings
   try
   {
      ShutterReleaseSettings& settings = m_host.GetReleaseSettings();

      CString cszFilename =
         m_host.GetImageFileManager().NextFilename(imageTypePano);
      settings.Filename(cszFilename);

      m_spRemoteReleaseControl->SetReleaseSettings(settings);
   }
   catch (CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Error while setting default shooting settings"), ex);
      dlg.DoModal(m_hWnd);
      return false;
   }

   return true;
}

void PanoramaPhotoModeManager::StartPanorama()
{
   HuginInterface hi(m_host.GetAppSettings().m_cszHuginPath);
   if (!hi.IsInstalled())
      AtlMessageBox(m_hWnd, _T("Hugin is not installed; panorama pictures will not be stitched at the end."), IDR_MAINFRAME, MB_OK);

   m_bStarted = true;
   m_vecPanoramaFilenameList.clear();
}

void PanoramaPhotoModeManager::StopPanorama()
{
   m_bStarted = false;
   m_vecPanoramaFilenameList.clear();
}

void PanoramaPhotoModeManager::StartHugin()
{
   HuginInterface hi(m_host.GetAppSettings().m_cszHuginPath);
   if (!hi.IsInstalled())
   {
      m_host.SetStatusText(_T("Finished taking panorama images"));
      return;
   }

   m_host.SetStatusText(_T("Starting Hugin..."));

   hi.RunUI(m_vecPanoramaFilenameList);
}

void PanoramaPhotoModeManager::ReleasePanorama()
{
   // called when shutter speed was changed successfully
   ShutterReleaseSettings& settings = m_host.GetReleaseSettings();

   settings.HandlerOnFinishedTransfer(
      std::bind(&PanoramaPhotoModeManager::OnFinishedTransfer, this, std::placeholders::_1));

   bool bNewPanorama = m_vecPanoramaFilenameList.empty();

   CString cszFilename =
      m_host.GetImageFileManager().NextFilename(imageTypePano, bNewPanorama);
   settings.Filename(cszFilename);

   m_spRemoteReleaseControl->SetReleaseSettings(settings);

   // set status text
   CString cszText;
   cszText.Format(_T("Taking picture; already took %Iu pictures."),
      m_vecPanoramaFilenameList.size() + 1);
   m_host.SetStatusText(cszText);

   // action mode is only unlocked when we receive an image
   if (m_host.GetReleaseSettings().SaveTarget() != ShutterReleaseSettings::saveToCamera)
      m_host.LockActionMode(true);

   try
   {
      m_spRemoteReleaseControl->Release();
   }
   catch (CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't release shutter"), ex);
      dlg.DoModal(m_hWnd);
   }
}

void PanoramaPhotoModeManager::OnFinishedTransfer(const ShutterReleaseSettings& settings)
{
   // save panorama filename
   CString cszFilename = settings.Filename();
   m_vecPanoramaFilenameList.push_back(cszFilename);

   m_host.OnTransferredImage(cszFilename);

   m_host.LockActionMode(false);
}
