//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file PanoramaPhotoModeView.cpp View for taking panorama photos
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "PanoramaPhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"
#include "ImageFileManager.hpp"
#include "ShutterReleaseSettings.hpp"
#include "RemoteReleaseControl.hpp"
#include "CameraErrorDlg.hpp"
#include "HuginInterface.hpp"
#include <boost/bind.hpp>

PanoramaPhotoModeView::PanoramaPhotoModeView(IPhotoModeViewHost& host) throw()
:m_host(host),
 m_bStarted(false)
{
}

LRESULT PanoramaPhotoModeView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   m_spRemoteReleaseControl = m_host.StartRemoteReleaseControl(true);

   // set default release settings
   try
   {
      ShutterReleaseSettings settings(ShutterReleaseSettings::saveToBoth);

      CString cszFilename =
         m_host.GetImageFileManager().NextFilename(imageTypePano);
      settings.Filename(cszFilename);

      m_spRemoteReleaseControl->SetDefaultReleaseSettings(settings);
   }
   catch(CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Error while setting default shooting settings"), ex);
      dlg.DoModal();
      return FALSE;
   }

   StopPanorama();

   return TRUE;
}

LRESULT PanoramaPhotoModeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // TODO ask to cancel panorama
   //if (!m_bStarted)
   //   ;
   return 0;
}

LRESULT PanoramaPhotoModeView::OnButtonStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (!m_bStarted)
   {
      StartPanorama();
   }

   ReleasePanorama();

   return 0;
}

LRESULT PanoramaPhotoModeView::OnButtonStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (!m_bStarted)
      return 0;

   StartHugin();

   StopPanorama();

   return 0;
}

LRESULT PanoramaPhotoModeView::OnButtonCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (!m_bStarted)
      return 0;

   StopPanorama();

   return 0;
}

void PanoramaPhotoModeView::StartPanorama()
{
   m_bStarted = true;
   m_vecPanoramaFilenameList.clear();

   m_btnStart.SetWindowText(_T("&Next"));
   m_btnStop.EnableWindow(true);
   m_btnCancel.EnableWindow(true);
}

void PanoramaPhotoModeView::StopPanorama()
{
   m_bStarted = false;
   m_vecPanoramaFilenameList.clear();

   m_btnStart.SetWindowText(_T("&Start"));
   m_btnStop.EnableWindow(false);
   m_btnCancel.EnableWindow(false);
}

void PanoramaPhotoModeView::StartHugin()
{
   HuginInterface hi(m_host.GetAppSettings().m_cszHuginPath);
   if (!hi.IsInstalled())
   {
      AtlMessageBox(m_hWnd, _T("Hugin is not installed"), IDR_MAINFRAME, MB_OK);
      return;
   }

   m_host.SetStatusText(_T("Starting Hugin..."));

   hi.RunUI(m_vecPanoramaFilenameList);
}

void PanoramaPhotoModeView::ReleasePanorama()
{
   // called when shutter speed was changed successfully
   ShutterReleaseSettings settings(ShutterReleaseSettings::saveToBoth,
      boost::bind(&PanoramaPhotoModeView::OnFinishedTransfer, this, _1));

   bool bNewPanorama = m_vecPanoramaFilenameList.empty();

   CString cszFilename =
      m_host.GetImageFileManager().NextFilename(imageTypePano, bNewPanorama);
   settings.Filename(cszFilename);

   // set status text
   CString cszText;
   cszText.Format(_T("Taking picture; already took %u pictures."),
      m_vecPanoramaFilenameList.size() + 1);
   m_host.SetStatusText(cszText);

   try
   {
      m_spRemoteReleaseControl->Release(settings);
   }
   catch(CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't release shutter"), ex);
      dlg.DoModal();
   }
}

void PanoramaPhotoModeView::OnFinishedTransfer(const ShutterReleaseSettings& settings)
{
   // save panorama filename
   CString cszFilename = settings.Filename();
   m_vecPanoramaFilenameList.push_back(cszFilename);
}
