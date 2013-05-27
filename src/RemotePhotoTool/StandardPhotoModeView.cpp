//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file StandardPhotoModeView.cpp View for taking standard photos
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "StandardPhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"
#include "ImageFileManager.hpp"
#include "CameraErrorDlg.hpp"
#include "ShutterReleaseSettings.hpp"
#include <boost/bind.hpp>

StandardPhotoModeView::StandardPhotoModeView(IPhotoModeViewHost& host) throw()
:m_host(host),
 m_cbShootingMode(propShootingMode),
 m_cbAperture(propAv),
 m_cbShutterSpeed(propTv),
 m_cbExposureComp(propExposureCompensation),
 m_cbWhiteBalance(propWhiteBalance),
 m_iPropertyHandlerId(-1)
{
}

LRESULT StandardPhotoModeView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   m_spRemoteReleaseControl = m_host.StartRemoteReleaseControl(true);

   SetupImagePropertyManager();

   // shooting mode change supported?
   if (!m_spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capChangeShootingMode))
   {
      // no: disable shooting mode combobox
      m_cbShootingMode.EnableWindow(FALSE);
   }
   else
   {
      // yes: wait for changes in shooting mode property
      m_iPropertyHandlerId = m_spRemoteReleaseControl->AddPropertyEventHandler(
         boost::bind(&StandardPhotoModeView::OnUpdatedProperty, this, _1, _2));
   }

   // set default release settings
   try
   {
      ShutterReleaseSettings settings(ShutterReleaseSettings::saveToBoth,
         boost::bind(&StandardPhotoModeView::OnFinishedTransfer, this, _1));

      CString cszFilename =
         m_host.GetImageFileManager().NextFilename(imageTypeNormal);
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

LRESULT StandardPhotoModeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // reset default release settings
   if (m_spRemoteReleaseControl != nullptr)
   {
      ShutterReleaseSettings settings(ShutterReleaseSettings::saveToBoth);
      try
      {
         m_spRemoteReleaseControl->SetDefaultReleaseSettings(settings);
      }
      catch(CameraException& /*ex*/)
      {
      }
   }

   if (m_iPropertyHandlerId != -1)
      m_spRemoteReleaseControl->RemovePropertyEventHandler(m_iPropertyHandlerId);

   m_upImagePropertyValueManager.reset();
   return 0;
}

LRESULT StandardPhotoModeView::OnShootingModeSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   UpdateShootingModeDependentValues();

   return 0;
}

LRESULT StandardPhotoModeView::OnButtonRelease(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   ShutterReleaseSettings settings(ShutterReleaseSettings::saveToBoth,
      boost::bind(&StandardPhotoModeView::OnFinishedTransfer, this, _1));

   CString cszFilename =
      m_host.GetImageFileManager().NextFilename(imageTypeNormal);
   settings.Filename(cszFilename);

   try
   {
      m_spRemoteReleaseControl->Release(settings);
   }
   catch(CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't release shutter"), ex);
      dlg.DoModal();
   }
   return 0;
}

void StandardPhotoModeView::OnUpdatedProperty(RemoteReleaseControl::T_enPropertyEvent enPropertyEvent, unsigned int uiValue)
{
   if (enPropertyEvent == RemoteReleaseControl::propEventPropertyChanged &&
      uiValue == m_spRemoteReleaseControl->MapImagePropertyTypeToId(propShootingMode))
   {
      // shooting mode has changed; update some fields
      UpdateShootingModeDependentValues();
   }
}

void StandardPhotoModeView::OnFinishedTransfer(const ShutterReleaseSettings& settings)
{
   CString cszText;
   cszText.Format(_T("Finished transfer: %s"), settings.Filename());

   m_host.SetStatusText(cszText);
}

void StandardPhotoModeView::SetupImagePropertyManager()
{
   m_upImagePropertyValueManager.reset(new ImagePropertyValueManager(*m_spRemoteReleaseControl));

   m_cbShootingMode.SetRemoteReleaseControl(m_spRemoteReleaseControl);
   m_cbAperture.SetRemoteReleaseControl(m_spRemoteReleaseControl);
   m_cbShutterSpeed.SetRemoteReleaseControl(m_spRemoteReleaseControl);
   m_cbExposureComp.SetRemoteReleaseControl(m_spRemoteReleaseControl);
   m_cbWhiteBalance.SetRemoteReleaseControl(m_spRemoteReleaseControl);

   m_upImagePropertyValueManager->AddControl(m_cbShootingMode);
   m_upImagePropertyValueManager->AddControl(m_cbAperture);
   m_upImagePropertyValueManager->AddControl(m_cbShutterSpeed);
   m_upImagePropertyValueManager->AddControl(m_cbExposureComp);
   m_upImagePropertyValueManager->AddControl(m_cbWhiteBalance);

   m_upImagePropertyValueManager->UpdateControls();

   m_upImagePropertyValueManager->UpdateProperty(
      m_spRemoteReleaseControl->MapImagePropertyTypeToId(propShootingMode));
}

void StandardPhotoModeView::UpdateShootingModeDependentValues()
{
   unsigned int uiShootingModeId = m_spRemoteReleaseControl->MapImagePropertyTypeToId(propShootingMode);
   ImageProperty shootingMode = m_spRemoteReleaseControl->GetImageProperty(uiShootingModeId);
   unsigned int uiShootingMode = shootingMode.Value().Get<unsigned int>();

   unsigned int uiM = m_spRemoteReleaseControl->MapShootingModeToImagePropertyValue(RemoteReleaseControl::shootingModeM).Value().Get<unsigned int>();
   unsigned int uiAv = m_spRemoteReleaseControl->MapShootingModeToImagePropertyValue(RemoteReleaseControl::shootingModeAv).Value().Get<unsigned int>();
   unsigned int uiTv = m_spRemoteReleaseControl->MapShootingModeToImagePropertyValue(RemoteReleaseControl::shootingModeTv).Value().Get<unsigned int>();
   unsigned int uiP = m_spRemoteReleaseControl->MapShootingModeToImagePropertyValue(RemoteReleaseControl::shootingModeP).Value().Get<unsigned int>();

   bool bReadOnlyAv = (uiShootingMode != uiM) && (uiShootingMode == uiTv || uiShootingMode == uiP);
   bool bReadOnlyTv = (uiShootingMode != uiM) && (uiShootingMode == uiAv || uiShootingMode == uiP);
   bool bReadOnlyExp = uiShootingMode == uiM;

   m_cbAperture.UpdateValuesList();
   m_cbAperture.UpdateValue();
   m_cbAperture.EnableWindow(!bReadOnlyAv);

   m_cbShutterSpeed.UpdateValuesList();
   m_cbShutterSpeed.UpdateValue();
   m_cbShutterSpeed.EnableWindow(!bReadOnlyTv);

   m_cbExposureComp.UpdateValuesList();
   m_cbExposureComp.UpdateValue();
   m_cbExposureComp.EnableWindow(!bReadOnlyExp);
}
