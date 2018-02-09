//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
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
#include "TimeLapseScheduler.hpp"

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

      settings.HandlerOnFinishedTransfer(nullptr);

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
   // disable viewfinder while shooting, if supported by camera
   if (m_spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capViewfinder))
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

   if (m_spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capViewfinder) &&
      !m_bViewfinderActiveBeforeStart)
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

      settings.HandlerOnFinishedTransfer(nullptr);

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

   // action mode is only locked/unlocked when we receive an image
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

//
// TimeLapsePhotoModeManager
//

/// ctor
TimeLapsePhotoModeManager::TimeLapsePhotoModeManager(IPhotoModeViewHost& host, HWND& hWnd)
   :m_host(host),
   m_hWnd(hWnd),
   m_stateMachineState(notRunning),
   m_stateEventHandlerId(-1),
   m_usingMirrorLockup(false),
   m_isAEBInProgress(false),
   m_viewfinderActiveBeforeStart(false),
   m_currentAEBShutterSpeedIndex(0)
{
}

bool TimeLapsePhotoModeManager::Init(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, std::function<void()> fnFinished)
{
   m_spRemoteReleaseControl = spRemoteReleaseControl;
   m_fnFinished = fnFinished;

   return true;
}

bool TimeLapsePhotoModeManager::IsStarted() const
{
   return m_spScheduler != nullptr &&
      m_stateMachineState != T_enStateMachineState::notRunning &&
      m_stateMachineState != T_enStateMachineState::finished;
}

void TimeLapsePhotoModeManager::Start()
{
   ATLASSERT(m_spScheduler == nullptr); // must not have a running scheduler

   if (!SetReleaseSettings())
   {
      return;
   }

   // disable viewfinder while shooting, if supported by camera
   if (m_spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capViewfinder))
   {
      ViewFinderView* pViewfinder = m_host.GetViewFinderView();

      m_viewfinderActiveBeforeStart = pViewfinder != nullptr;

      if (m_viewfinderActiveBeforeStart)
         pViewfinder->EnableUpdate(false);
   }

   m_spScheduler = std::make_shared<TimeLapseScheduler>();

   m_stateMachineState = T_enStateMachineState::started;

   m_host.SetStatusText(_T("TimeLapse state: Started"));

   if (m_options.m_releaseTrigger == TimeLapseOptions::T_enReleaseTrigger::releaseAfterLastImage)
   {
      EnableMirrorLockup();
   }

   auto fnSchedule = std::bind(&TimeLapsePhotoModeManager::RunStateMachine, this);
   m_spScheduler->Schedule(fnSchedule);

   auto fnOnStateEvent = std::bind(&TimeLapsePhotoModeManager::OnStateEvent, this, std::placeholders::_1, std::placeholders::_2);

   m_stateEventHandlerId =
      m_spRemoteReleaseControl->AddStateEventHandler(fnOnStateEvent);
}

void TimeLapsePhotoModeManager::Stop()
{
   ATLASSERT(m_spScheduler != nullptr); // must have a running scheduler

   m_spRemoteReleaseControl->RemoveStateEventHandler(m_stateEventHandlerId);

   m_spScheduler->CancelAll();
   m_spScheduler.reset();

   m_host.SetStatusText(_T("TimeLapse state: Finished"));

   if (m_options.m_releaseTrigger == TimeLapseOptions::T_enReleaseTrigger::releaseAfterLastImage)
   {
      DisableMirrorLockup();
   }

   // enable viewfinder again when active
   if (m_viewfinderActiveBeforeStart)
   {
      ViewFinderView* pViewfinder = m_host.GetViewFinderView();
      if (pViewfinder != NULL)
         pViewfinder->EnableUpdate(true);

      m_viewfinderActiveBeforeStart = false;
   }

   m_isAEBInProgress = false;
   m_currentAEBShutterSpeedIndex = 0;
}

void TimeLapsePhotoModeManager::ManualRelease()
{
   ATLASSERT(m_spScheduler != nullptr); // must have a running scheduler
   ATLASSERT(m_options.m_releaseTrigger == TimeLapseOptions::releaseManually);

   if (m_stateMachineState == T_enStateMachineState::waitManualRelease)
   {
      m_stateMachineState = T_enStateMachineState::takePhoto;

      auto fnSchedule = std::bind(&TimeLapsePhotoModeManager::RunStateMachine, this);
      m_spScheduler->Schedule(fnSchedule);

      m_host.SetStatusText(_T("TimeLapse state: Taking photo..."));
   }
}

bool TimeLapsePhotoModeManager::CheckManualMode()
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

void TimeLapsePhotoModeManager::RecalcAEBShutterSpeedList(size_t numShots)
{
   m_shutterSpeedValues.clear();

   unsigned int shutterSpeedPropertyId =
      m_spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propTv);

   try
   {
      ImageProperty currentShutterSpeed = m_spRemoteReleaseControl->GetImageProperty(shutterSpeedPropertyId);

      ShutterSpeedValue shutterSpeedBase(currentShutterSpeed, m_spRemoteReleaseControl);
      ShutterSpeedValue shutterDown = shutterSpeedBase, shutterUp = shutterSpeedBase;

      for (size_t shot = 1; shot <= numShots; shot++)
      {
         if (shot == 1)
         {
            m_shutterSpeedValues.push_back(shutterSpeedBase.Value());
         }
         else
            if ((shot & 1) == 0) // even: 2, 4, 6, ...
            {
               ShutterSpeedValue prev = shutterDown;
               shutterDown.Subtract2EV();

               // limit
               if (prev != shutterDown)
                  m_shutterSpeedValues.push_back(shutterDown.Value());
            }
            else
               if ((shot & 1) == 1) // odd: 3, 5, 7, ...
               {
                  ShutterSpeedValue prev = shutterUp;
                  shutterUp.Add2EV();

                  // limit
                  if (prev != shutterUp)
                     m_shutterSpeedValues.push_back(shutterUp.Value());
               }
      }
   }
   catch (CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't get shutter speed values"), ex);
      dlg.DoModal(m_hWnd);
   }
}

bool TimeLapsePhotoModeManager::SetReleaseSettings()
{
   try
   {
      ShutterReleaseSettings& settings = m_host.GetReleaseSettings();

      CString filename =
         m_host.GetImageFileManager().NextFilename(imageTypeTimelapse, true);
      settings.Filename(filename);

      settings.HandlerOnFinishedTransfer(
         std::bind(&TimeLapsePhotoModeManager::OnFinishedTransfer, this, std::placeholders::_1));

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

void TimeLapsePhotoModeManager::RunStateMachine()
{
   if (m_spScheduler->IsFinished())
   {
      m_host.SetStatusText(_T("TimeLapse state: Finished"));

      m_stateMachineState = T_enStateMachineState::finished;

      if (m_fnFinished != nullptr)
         m_fnFinished();

      if (m_options.m_releaseTrigger == TimeLapseOptions::T_enReleaseTrigger::releaseAfterLastImage)
      {
         DisableMirrorLockup();
      }

      return;
   }

   bool exit = false;
   while (!exit)
   {
      switch (m_stateMachineState)
      {
      case T_enStateMachineState::notRunning:
         m_stateMachineState = T_enStateMachineState::started;
         break;

      case T_enStateMachineState::started:
         OnStateStart(exit);
         break;

      case T_enStateMachineState::waitManualRelease:
         exit = true; // next state is set by OnStateEvent()
         break;

      case T_enStateMachineState::takePhoto:
         OnStateTakePhoto(exit);
         break;

      case T_enStateMachineState::waitTransferFinished:
         OnStateWaitTransferFinished(exit);
         break;

      case T_enStateMachineState::finished:
         exit = true;

         if (m_fnFinished != nullptr)
            m_fnFinished();

         if (m_options.m_releaseTrigger == TimeLapseOptions::T_enReleaseTrigger::releaseAfterLastImage)
         {
            DisableMirrorLockup();
         }

         break;

      default:
         ATLASSERT(false); // invalid state machine state
         break;
      }
   }
}

void TimeLapsePhotoModeManager::OnStateStart(bool& exit)
{
   if (m_options.m_releaseTrigger == TimeLapseOptions::T_enReleaseTrigger::releaseManually)
   {
      m_stateMachineState = T_enStateMachineState::waitManualRelease;

      m_host.SetStatusText(_T("TimeLapse state: Waiting for manual release..."));

      exit = true; // need an external trigger
      return;
   }

   if (m_options.m_useStartTime)
   {
      auto fn = [&]() {
         m_stateMachineState = T_enStateMachineState::takePhoto;
         RunStateMachine();
      };

      m_spScheduler->Schedule(m_options.m_startTime, fn);

      m_host.SetStatusText(_T("TimeLapse state: Waiting for start time..."));

      exit = true; // need to wait for start time to occur
   }
   else
   {
      m_stateMachineState = T_enStateMachineState::takePhoto;
   }
}

void TimeLapsePhotoModeManager::OnStateTakePhoto(bool& exit)
{
   if (m_options.m_useHDR &&
      !m_isAEBInProgress)
   {
      m_lastTriggerTime = COleDateTime::GetCurrentTime();

      m_isAEBInProgress = true;
      //m_vecAEBFilenameList.clear();
      m_currentAEBShutterSpeedIndex = 0;

      if (m_options.m_releaseTrigger != TimeLapseOptions::T_enReleaseTrigger::releaseAfterLastImage)
         EnableMirrorLockup();
   }

   // action mode is only locked/unlocked when we receive an image
   if (m_host.GetReleaseSettings().SaveTarget() != ShutterReleaseSettings::saveToCamera)
      m_host.LockActionMode(true);

   if (m_options.m_useHDR)
   {
      // set status text
      CString cszText;
      cszText.Format(_T("TimeLapse state: Taking HDR photo %Iu of %Iu with shutter speed %s..."),
         m_currentAEBShutterSpeedIndex + 1,
         m_shutterSpeedValues.size(),
         m_shutterSpeedValues[m_currentAEBShutterSpeedIndex].AsString().GetString());
      m_host.SetStatusText(cszText);

      try
      {
         m_spRemoteReleaseControl->SetImageProperty(m_shutterSpeedValues[m_currentAEBShutterSpeedIndex]);

         m_currentAEBShutterSpeedIndex++;
      }
      catch (const CameraException& ex)
      {
         UNUSED(ex);
         m_host.SetStatusText(_T("TimeLapse error - Couldn't set new shutter speed"));
      }
   }
   else
   {
      m_host.SetStatusText(_T("TimeLapse state: Taking photo..."));

      m_lastTriggerTime = COleDateTime::GetCurrentTime();
   }

   try
   {
      m_spRemoteReleaseControl->Release();
   }
   catch (const CameraException& ex)
   {
      UNUSED(ex);
      m_host.SetStatusText(_T("TimeLapse error - Couldn't release shutter"));
   }

   exit = true; // need finished transfer to continue state machine
}

void TimeLapsePhotoModeManager::OnStateWaitTransferFinished(bool& exit)
{
   if (m_options.m_releaseTrigger != TimeLapseOptions::releaseManually &&
      m_options.m_useEndTime)
   {
      COleDateTime now = COleDateTime::GetCurrentTime();
      if (now >= m_options.m_endTime)
      {
         m_host.SetStatusText(_T("TimeLapse state: Finished"));

         m_stateMachineState = T_enStateMachineState::finished;

         if (m_fnFinished != nullptr)
            m_fnFinished();

         exit = true;
         return;
      }
   }

   switch (m_options.m_releaseTrigger)
   {
   case TimeLapseOptions::releaseAfterLastImage:
      m_stateMachineState = T_enStateMachineState::takePhoto;
      break;

   case TimeLapseOptions::releaseTriggerInterval:
   {
      COleDateTime nextTriggerTime = m_lastTriggerTime + m_options.m_intervalTime;

      m_stateMachineState = T_enStateMachineState::takePhoto;

      auto fnSchedule = std::bind(&TimeLapsePhotoModeManager::RunStateMachine, this);
      m_spScheduler->Schedule(nextTriggerTime, fnSchedule);

      m_host.SetStatusText(_T("TimeLapse state: Waiting next interval..."));

      exit = true;
   }
   break;

   case TimeLapseOptions::releaseManually:
      m_stateMachineState = T_enStateMachineState::waitManualRelease;

      m_host.SetStatusText(_T("TimeLapse state: Waiting for manual release..."));

      exit = true;
      break;

   default:
      ATLASSERT(false); // invalid release trigger value
      break;
   }
}

void TimeLapsePhotoModeManager::OnStateEvent(RemoteReleaseControl::T_enStateEvent stateEvent, unsigned int extraData)
{
   UNUSED(extraData);

   if (stateEvent == RemoteReleaseControl::stateEventCameraShutdown)
   {
      m_stateMachineState = T_enStateMachineState::waitTransferFinished;
   }
}

void TimeLapsePhotoModeManager::OnFinishedTransfer(const ShutterReleaseSettings& settings)
{
   // save timelapse filename
   CString filename = settings.Filename();
   m_timelapseFilenameList.push_back(filename);

   m_host.OnTransferredImage(filename);

   m_host.LockActionMode(false);

   if (m_spScheduler == nullptr)
      return; // stop button was pressed already

   if (m_options.m_useHDR)
   {
      if (m_currentAEBShutterSpeedIndex < m_shutterSpeedValues.size())
      {
         m_stateMachineState = T_enStateMachineState::takePhoto;
      }
      else
      {
         if (m_options.m_releaseTrigger != TimeLapseOptions::T_enReleaseTrigger::releaseAfterLastImage)
            DisableMirrorLockup();

         m_isAEBInProgress = false;
         m_currentAEBShutterSpeedIndex = 0;

         m_stateMachineState = T_enStateMachineState::waitTransferFinished;
      }
   }
   else
   {
      m_stateMachineState = T_enStateMachineState::waitTransferFinished;
   }

   auto fn = std::bind(&TimeLapsePhotoModeManager::RunStateMachine, this);
   m_spScheduler->Schedule(fn);
}

/// mirror lockup property ID on Canon EOS cameras
unsigned int c_propertyIdMirrorLockup = 0x00000009 | (0x0007 << 16);

void TimeLapsePhotoModeManager::EnableMirrorLockup()
{
   // check if camera has EOS mirror lockup custom function
   try
   {
      ImageProperty mirrorLockup = m_spRemoteReleaseControl->GetImageProperty(c_propertyIdMirrorLockup);

      mirrorLockup.Value().Set<unsigned int>(1);

      m_usingMirrorLockup = true;

      return;
   }
   catch (...)
   {
      m_usingMirrorLockup = false;
   }

   if (m_spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capViewfinder) &&
      !m_viewfinderActiveBeforeStart)
   {
      try
      {
         // init viewfinder; this is done to speed up taking images;
         // in future it could be done with mirror lockup
         m_spViewfinder = m_spRemoteReleaseControl->StartViewfinder();
      }
      catch (CameraException& ex)
      {
         UNUSED(ex);
         m_host.SetStatusText("Timelapse error: Couldn't start viewfinder");
      }
   }
}

void TimeLapsePhotoModeManager::DisableMirrorLockup()
{
   if (m_usingMirrorLockup)
   {
      // check if camera has EOS mirror lockup custom function
      try
      {
         ImageProperty mirrorLockup = m_spRemoteReleaseControl->GetImageProperty(c_propertyIdMirrorLockup);

         mirrorLockup.Value().Set<unsigned int>(0);
      }
      catch (...)
      {
      }

      m_usingMirrorLockup = false;
   }

   // close viewfinder, if used at all
   m_spViewfinder.reset();
}
