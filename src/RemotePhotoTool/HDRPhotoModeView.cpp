//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file HDRPhotoModeView.cpp View for taking HDR photos
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "HDRPhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"
#include "ImageFileManager.hpp"
#include "ShutterReleaseSettings.hpp"
#include "ShutterSpeedValue.hpp"
#include "ShootingMode.hpp"
#include "CameraErrorDlg.hpp"
#include "ViewFinderImageWindow.hpp"
#include "PhotomatixInterface.hpp"

HDRPhotoModeView::HDRPhotoModeView(IPhotoModeViewHost& host) throw()
:m_host(host),
 m_cbShutterSpeed(propTv),
 m_iPropertyHandlerId(-1),
 m_bAEBInProgress(false),
 m_bViewfinderActiveBeforeStart(false),
 m_uiCurrentAEBShutterSpeed(0)
{
}

LRESULT HDRPhotoModeView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   m_spRemoteReleaseControl = m_host.StartRemoteReleaseControl(true);

   // init shutter speed combobox
   {
      m_cbShutterSpeed.SetRemoteReleaseControl(m_spRemoteReleaseControl);

      m_cbShutterSpeed.UpdateValuesList();
      m_cbShutterSpeed.UpdateValue();
   }

   // init combobox with number of bracketed shots
   {
      for (size_t i=3; i<13; i+= 2)
      {
         CString cszText;
         cszText.Format(_T("%u shots"), i);
         int iItem = m_cbAEBBracketedShots.AddString(cszText);
         m_cbAEBBracketedShots.SetItemData(iItem, i);
      }

      m_cbAEBBracketedShots.SetCurSel(0);

      RecalcAEBShutterSpeedList();
      UpdateAEBShutterSpeedList();
   }

   // init list with shutter speed values
   m_lcAEBShutterSpeedValues.InsertColumn(0, _T("Shutter speed"), LVCFMT_LEFT, 80);
   
   // add handler for changing shutter speed
   m_iPropertyHandlerId = m_spRemoteReleaseControl->AddPropertyEventHandler(
      std::bind(&HDRPhotoModeView::OnUpdatedProperty, this, std::placeholders::_1, std::placeholders::_2));

   // set default release settings
   try
   {
      ShutterReleaseSettings settings(ShutterReleaseSettings::saveToBoth);

      CString cszFilename =
         m_host.GetImageFileManager().NextFilename(imageTypeHDR);
      settings.Filename(cszFilename);

      m_spRemoteReleaseControl->SetDefaultReleaseSettings(settings);
   }
   catch(CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Error while setting default shooting settings"), ex);
      dlg.DoModal();
      return FALSE;
   }

   return TRUE;
}

LRESULT HDRPhotoModeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_spRemoteReleaseControl->RemovePropertyEventHandler(m_iPropertyHandlerId);

   return 0;
}

LRESULT HDRPhotoModeView::OnComboShutterSpeedSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   RecalcAEBShutterSpeedList();
   UpdateAEBShutterSpeedList();

   return 0;
}

LRESULT HDRPhotoModeView::OnComboAEBBracketShotsSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   RecalcAEBShutterSpeedList();
   UpdateAEBShutterSpeedList();

   return 0;
}

void HDRPhotoModeView::OnUpdatedProperty(RemoteReleaseControl::T_enPropertyEvent enPropertyEvent, unsigned int uiValue)
{
   if (m_bAEBInProgress)
      return;

   unsigned int uiShutterSpeedPropertyId =
      m_spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propTv);

   if (uiShutterSpeedPropertyId == uiValue)
   {
      if (enPropertyEvent == RemoteReleaseControl::propEventPropertyDescChanged)
         m_cbShutterSpeed.UpdateValuesList();
      else
         m_cbShutterSpeed.UpdateValue();
   }

   if (enPropertyEvent == RemoteReleaseControl::propEventPropertyChanged &&
       uiShutterSpeedPropertyId == uiValue)
   {
      RecalcAEBShutterSpeedList();
      UpdateAEBShutterSpeedList();
   }
}

LRESULT HDRPhotoModeView::OnButtonAEB(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_vecAEBShutterSpeedValues.empty())
      return 0;

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
         return 0;
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
      catch(CameraException& ex)
      {
         CameraErrorDlg dlg(_T("Couldn't switch to Manual mode"), ex);
         dlg.DoModal();
         return 0;
      }
   }

   // disable viewfinder while shooting
   {
      ViewFinderImageWindow* pViewfinder = m_host.GetViewfinderWindow();

      m_bViewfinderActiveBeforeStart = pViewfinder != NULL;

      if (m_bViewfinderActiveBeforeStart)
         pViewfinder->EnableUpdate(false);
   }

   ReleaseAEBFirst();

   return 0;
}

LRESULT HDRPhotoModeView::OnMessageHDRAEBNext(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   ReleaseAEBNext();
   return 0;
}

LRESULT HDRPhotoModeView::OnMessageHDRAEBLast(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_host.LockActionMode(false);

   // close viewfinder, if used at all
   m_spViewfinder.reset();

   m_bAEBInProgress = false;

   // now send images to Photomatix
   PhotomatixInterface pi(m_host.GetAppSettings().m_cszPhotomatixPath);
   if (!pi.IsInstalled())
   {
      AtlMessageBox(m_hWnd, _T("Photomatix is not installed"), IDR_MAINFRAME, MB_OK);
      return 0;
   }

   m_host.SetStatusText(_T("Starting Photomatix..."));

   pi.RunUI(m_vecAEBFilenameList);

   // enable viewfinder again when active
   if (m_bViewfinderActiveBeforeStart)
   {
      ViewFinderImageWindow* pViewfinder = m_host.GetViewfinderWindow();
      if (pViewfinder != NULL)
         pViewfinder->EnableUpdate(true);

      m_bViewfinderActiveBeforeStart = false;
   }

   return 0;
}

void HDRPhotoModeView::RecalcAEBShutterSpeedList()
{
   int iItem = m_cbAEBBracketedShots.GetCurSel();
   if (iItem == CB_ERR)
      return;

   size_t uiNumShots = m_cbAEBBracketedShots.GetItemData(iItem);
   
   m_vecAEBShutterSpeedValues.clear();

   unsigned int uiShutterSpeedPropertyId =
      m_spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propTv);

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

void HDRPhotoModeView::UpdateAEBShutterSpeedList()
{
   m_lcAEBShutterSpeedValues.SetRedraw(FALSE);
   m_lcAEBShutterSpeedValues.DeleteAllItems();

   for (size_t i = 0, iMax = m_vecAEBShutterSpeedValues.size(); i<iMax; i++)
   {
      m_lcAEBShutterSpeedValues.InsertItem(m_lcAEBShutterSpeedValues.GetItemCount(),
         m_vecAEBShutterSpeedValues[i].AsString());
   }

   m_lcAEBShutterSpeedValues.SetRedraw(TRUE);
}

void HDRPhotoModeView::ReleaseAEBFirst()
{
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
      catch(CameraException& ex)
      {
         CameraErrorDlg dlg(_T("Couldn't start viewfinder"), ex);
         dlg.DoModal();
      }
   }

   ReleaseAEBNext();
}

void HDRPhotoModeView::ReleaseAEBNext()
{
   // called when shutter speed was changed successfully
   bool bLastShot = m_uiCurrentAEBShutterSpeed + 1 == m_vecAEBShutterSpeedValues.size();

   ShutterReleaseSettings settings(ShutterReleaseSettings::saveToBoth,
      bLastShot ?
         std::bind(&HDRPhotoModeView::OnFinishedTransferLastAEB, this, std::placeholders::_1) :
         std::bind(&HDRPhotoModeView::OnFinishedTransferNextAEB, this, std::placeholders::_1));

   CString cszFilename =
      m_host.GetImageFileManager().NextFilename(imageTypeHDR, m_uiCurrentAEBShutterSpeed == 0);
   settings.Filename(cszFilename);

   // set status text
   CString cszText;
   cszText.Format(_T("Taking picture %u of %u with shutter speed %s"),
      m_uiCurrentAEBShutterSpeed + 1,
      m_vecAEBShutterSpeedValues.size(),
      m_vecAEBShutterSpeedValues[m_uiCurrentAEBShutterSpeed].AsString());
   m_host.SetStatusText(cszText);

   try
   {
      m_spRemoteReleaseControl->SetImageProperty(m_vecAEBShutterSpeedValues[m_uiCurrentAEBShutterSpeed]);

      m_spRemoteReleaseControl->Release(settings);

      m_uiCurrentAEBShutterSpeed++;
   }
   catch(CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't release AEB shutter"), ex);
      dlg.DoModal();
   }
}

void HDRPhotoModeView::OnFinishedTransferNextAEB(const ShutterReleaseSettings& settings)
{
   // save filename for further processing
   CString cszFilename = settings.Filename();
   m_vecAEBFilenameList.push_back(cszFilename);

   ATLASSERT(m_uiCurrentAEBShutterSpeed < m_vecAEBShutterSpeedValues.size());

   PostMessage(WM_HDR_AEB_NEXT);
}

void HDRPhotoModeView::OnFinishedTransferLastAEB(const ShutterReleaseSettings& settings)
{
   // save filename for further processing
   CString cszFilename = settings.Filename();
   m_vecAEBFilenameList.push_back(cszFilename);

   PostMessage(WM_HDR_AEB_LAST);
}
