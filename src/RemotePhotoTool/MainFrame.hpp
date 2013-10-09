//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file MainFrame.hpp Main application frame
//
#pragma once

// includes
#include "resource.h"
#include "res/MainFrameRibbon.h"
#include "IPhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"
#include "ViewManager.hpp"
#include "ViewFinderImageWindow.hpp"
#include "AppSettings.hpp"
#include "ImageFileManager.hpp"
#include "RemoteReleaseControl.hpp"
#include <atlsplit.h>
#include <atlctrlx.h>

// forward references
class SourceDevice;

/// \brief application main frame
/// \details uses ribbon for commands
/// \see http://www.codeproject.com/Articles/54116/Relook-your-Old-and-New-Native-Applications-with-a
class MainFrame :
   public CRibbonFrameWindowImpl<MainFrame>,
   public CMessageFilter,
   public CIdleHandler,
   public IPhotoModeViewHost
{
   /// base class typedef
   typedef CRibbonFrameWindowImpl<MainFrame> BaseClass;

public:
   /// ctor
   MainFrame() throw();
   /// dtor
   virtual ~MainFrame() throw();

   /// returns if a camera is connected
   bool IsConnected() const throw() { return m_spSourceDevice != nullptr; }

   DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

private:
   friend CDynamicUpdateUI;
   friend CRibbonFrameWindowImplBase;

   /// pretranslates message
   virtual BOOL PreTranslateMessage(MSG* pMsg);

   /// called when message loop is idle
   virtual BOOL OnIdle();

   BEGIN_UPDATE_UI_MAP(MainFrame)
      UPDATE_ELEMENT(ID_VIEW_RIBBON, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_SHOW, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_TAB_HOME, UPDUI_RIBBON)
   END_UPDATE_UI_MAP()

   BEGIN_MSG_MAP(MainFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_CLOSE, OnClose)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_MOVE, OnMove)
      MESSAGE_HANDLER(WM_LOCK_ACTIONMODE, OnLockActionMode)
      COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
      COMMAND_ID_HANDLER(ID_VIEW_RIBBON, OnToggleRibbon)
      COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
      COMMAND_ID_HANDLER(ID_HOME_CONNECT, OnHomeConnect)
      COMMAND_ID_HANDLER(ID_HOME_SETTINGS, OnHomeSettings)
      COMMAND_RANGE_HANDLER(ID_PHOTO_MODE_NORMAL, ID_PHOTO_MODE_IMAGE_PROPERTIES, OnPhotoMode)
      COMMAND_ID_HANDLER(ID_VIEWFINDER_SHOW, OnViewfinderShow)
      CHAIN_MSG_MAP(BaseClass)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnLockActionMode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnToggleRibbon(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnHomeConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnHomeSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnPhotoMode(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewfinderShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
   // virtual methods from IPhotoModeViewHost

   virtual std::shared_ptr<RemoteReleaseControl> StartRemoteReleaseControl(bool bStart) override;

   virtual AppSettings& GetAppSettings() throw() override { return m_settings; }

   virtual ImageFileManager& GetImageFileManager() throw() override { return *m_upImageFileManager; }

   virtual ViewFinderImageWindow* GetViewfinderWindow() throw() override { return m_upViewFinderWindow.get(); }

   virtual void SetStatusText(const CString& cszText) override;

   virtual void LockActionMode(bool bLock) override;

   /// sets up toolbar
   void SetupToolbar(HWND hWndCmdBar);

   /// sets up status bar
   void SetupStatusBar();

   /// sets new photo mode view
   void SetNewView(T_enViewType enViewType);

   /// shows or hides viewfinder
   void ShowViewfinder(bool bShow);

   /// enables or disables photo modes (and live view)
   void EnablePhotoModes(bool bEnable);

   /// sets status bar pane widths
   void SetPaneWidths(int* arrWidths, int nPanes);

   /// state event handler
   void OnStateEvent(RemoteReleaseControl::T_enStateEvent enStateEvent, unsigned int uiExtraData);

   /// download event handler
   void OnDownloadEvent(RemoteReleaseControl::T_enDownloadEvent enDownloadEvent, unsigned int uiProgress);

private:
   // UI

   /// command bar
   CCommandBarCtrl m_CmdBar;

   /// splitter to separate photo mode views from viewfinder
   CSplitterWindow m_splitter;

   /// status bar
   CMultiPaneStatusBarCtrl m_statusBar;

   /// download progress bar
   CProgressBarCtrl m_downloadProgressBar;

   /// current view
   std::unique_ptr<IPhotoModeView> m_upView;

   /// view window handle
   HWND m_hWndView;

   /// viewfinder window
   std::unique_ptr<ViewFinderImageWindow> m_upViewFinderWindow;

   /// thread id of UI thread
   DWORD m_dwUIThreadId;

   // model

   /// application settings
   AppSettings m_settings;

   /// image file manager
   std::unique_ptr<ImageFileManager> m_upImageFileManager;

   /// current source device
   std::shared_ptr<SourceDevice> m_spSourceDevice;

   /// current remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;
};
