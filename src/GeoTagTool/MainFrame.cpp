//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file GeoTagTool\MainFrame.cpp GeoTagTool main frame
//
#include "stdafx.h"
#include "res/Ribbon.h"
#include "resource.h"
#include "AboutDlg.hpp"
#include "GeoTagToolView.hpp"
#include "MainFrame.hpp"
#include "SerialPortDlg.hpp"

BOOL MainFrame::PreTranslateMessage(MSG* pMsg)
{
   if (CRibbonFrameWindowImpl<MainFrame>::PreTranslateMessage(pMsg))
      return TRUE;

   return m_view.PreTranslateMessage(pMsg);
}

BOOL MainFrame::OnIdle()
{
   return FALSE;
}

LRESULT MainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_cmdBar.Create(m_hWnd, rcDefault, NULL, WS_CHILD);
   m_cmdBar.AttachMenu(GetMenu());
   m_cmdBar.LoadImages(IDR_MAINFRAME);

   CreateSimpleStatusBar();

   m_hWndClient = m_view.Create(m_hWnd);
   UISetCheck(ID_VIEW_STATUS_BAR, 1);

   // register object for message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->AddMessageFilter(this);
   pLoop->AddIdleHandler(this);

   ShowRibbonUI(true);

   return 0;
}

LRESULT MainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   // unregister message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->RemoveMessageFilter(this);
   pLoop->RemoveIdleHandler(this);

   bHandled = FALSE;
   return 1;
}

LRESULT MainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   PostMessage(WM_CLOSE);
   return 0;
}

LRESULT MainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   AboutDlg dlg;
   dlg.DoModal();

   return 0;
}

LRESULT MainFrame::OnDataSourceOpenGPSReceiver(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_liveTrack == nullptr)
   {
      m_liveTrack = std::make_unique<GPS::Track>();
   }

   SerialPortDlg dlg;
   if (IDOK != dlg.DoModal(m_hWnd))
      return 0;

   m_gpsReceiver = std::make_unique<GPS::Receiver>();
   m_gpsReceiver->Configure(dlg.GetSerialPortDeviceName());

   try
   {
      m_gpsReceiver->Start(*m_liveTrack.get());
   }
   catch (const std::exception& ex)
   {
      CString text;
      text.Format(_T("Error while opening serial port: %hs"), ex.what());
      AtlMessageBox(m_hWnd, text.GetString());

      return 0;
   }

   return 0;
}

LRESULT MainFrame::OnDataSourceImportTrack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

   return 0;
}

LRESULT MainFrame::OnActionsTagImages(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   return 0;
}

LRESULT MainFrame::OnActionsSaveLiveTrack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   return 0;
}
