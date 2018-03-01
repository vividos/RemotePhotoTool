//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file GeoTagTool\MainFrame.hpp GeoTagTool main frame
//
#pragma once

#include "GPS/Receiver.hpp"
#include "GPS/Track.hpp"

/// main frame for GeoTagTool
class MainFrame :
   public CRibbonFrameWindowImpl<MainFrame>,
   public CMessageFilter, public CIdleHandler
{
public:
   DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

   // Ribbon control map
   BEGIN_RIBBON_CONTROL_MAP(MainFrame)
   END_RIBBON_CONTROL_MAP()

   virtual BOOL PreTranslateMessage(MSG* pMsg);
   virtual BOOL OnIdle();

private:
   BEGIN_MSG_MAP(MainFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
      COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
      COMMAND_ID_HANDLER(ID_DATASOURCE_OPEN_GPS_RECEIVER, OnDataSourceOpenGPSReceiver)
      COMMAND_ID_HANDLER(ID_DATASOURCE_IMPORT_TRACK, OnDataSourceImportTrack)
      COMMAND_ID_HANDLER(ID_ACTIONS_TAG_IMAGES, OnActionsTagImages)
      COMMAND_ID_HANDLER(ID_ACTIONS_SAVE_LIVE_TRACK, OnActionsSaveLiveTrack)
      CHAIN_MSG_MAP(CRibbonFrameWindowImpl<MainFrame>)
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   // LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
   LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   /// called when "Open GPS Receiver" button has been pressed
   LRESULT OnDataSourceOpenGPSReceiver(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   /// called when "Import Track" button has been pressed
   LRESULT OnDataSourceImportTrack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   /// called when "Tag images" button has been pressed
   LRESULT OnActionsTagImages(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   /// called when "Save Live Track" button has been pressed
   LRESULT OnActionsSaveLiveTrack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
   /// the view
   GeoTagToolView m_view;

   /// command bar
   CCommandBarCtrl m_cmdBar;

   // model

   /// GPS receiver
   std::unique_ptr<GPS::Receiver> m_gpsReceiver;

   /// current live track
   std::unique_ptr<GPS::Track> m_liveTrack;
};
