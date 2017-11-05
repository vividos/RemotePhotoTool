//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file PanoramaPhotoModeView.cpp View for taking panorama photos
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "PanoramaPhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"

PanoramaPhotoModeView::PanoramaPhotoModeView(IPhotoModeViewHost& host)
:m_host(host),
m_manager(host, m_hWnd)
{
}

bool PanoramaPhotoModeView::CanClose() const
{
   if (!m_manager.IsStarted())
      return true;

   // ask to cancel panorama
   int iRet = AtlMessageBox(m_hWnd, _T("A panorama shooting is in progress. Do you want to abort taking the panorama?"),
      IDR_MAINFRAME, MB_YESNO | MB_ICONQUESTION);

   if (iRet == IDNO)
      return false;

   return true;
}

LRESULT PanoramaPhotoModeView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl = m_host.GetRemoteReleaseControl();

   if (!m_manager.Init(spRemoteReleaseControl))
      return FALSE;

   StopPanorama();

   return TRUE;
}

LRESULT PanoramaPhotoModeView::OnButtonStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (!m_manager.IsStarted())
      StartPanorama();

   m_manager.ReleasePanorama();

   return 0;
}

LRESULT PanoramaPhotoModeView::OnButtonStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (!m_manager.IsStarted())
      return 0;

   m_manager.StartHugin();

   StopPanorama();

   return 0;
}

LRESULT PanoramaPhotoModeView::OnButtonCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (!m_manager.IsStarted())
      return 0;

   StopPanorama();

   return 0;
}

void PanoramaPhotoModeView::StartPanorama()
{
   m_manager.StartPanorama();

   m_btnStart.SetWindowText(_T("&Next"));
   m_btnStop.EnableWindow(true);
   m_btnCancel.EnableWindow(true);
}

void PanoramaPhotoModeView::StopPanorama()
{
   m_manager.StopPanorama();

   m_btnStart.SetWindowText(_T("&Start"));
   m_btnStop.EnableWindow(false);
   m_btnCancel.EnableWindow(false);
}
