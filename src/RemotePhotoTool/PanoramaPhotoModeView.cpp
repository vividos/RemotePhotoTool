//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PanoramaPhotoModeView.cpp View for taking panorama photos
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "PanoramaPhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"

PanoramaPhotoModeView::PanoramaPhotoModeView(IPhotoModeViewHost& host) throw()
:m_host(host),
m_manager(host, m_hWnd)
{
}

LRESULT PanoramaPhotoModeView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl = m_host.StartRemoteReleaseControl(true);
   if (spRemoteReleaseControl == nullptr)
   {
      AtlMessageBox(m_hWnd, _T("Couldn't start remote release control."), IDR_MAINFRAME, MB_OK);
      DestroyWindow();
      return 0;
   }

   if (!m_manager.Init(spRemoteReleaseControl))
      return FALSE;

   StopPanorama();

   return TRUE;
}


LRESULT PanoramaPhotoModeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // TODO ask to cancel panorama
   //if (m_manager.IsStarted())
   //   ;
   return 0;
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
