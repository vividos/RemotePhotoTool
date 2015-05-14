//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file RemotePhotoTool\MainFrame.hpp Main application frame
//
#pragma once

// includes
#include "resource.h"
#include "res/MainFrameRibbon.h"
#include "IPhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"
#include "ViewManager.hpp"
#include "ViewFinderView.hpp"
#include "AppSettings.hpp"
#include "ImageFileManager.hpp"
#include "PreviousImagesManager.hpp"
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
   MainFrame();
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

private:
   friend CRibbonImpl<MainFrame>;

   BEGIN_RIBBON_CONTROL_MAP(MainFrame)
   END_RIBBON_CONTROL_MAP()

   BEGIN_UPDATE_UI_MAP(MainFrame)
      UPDATE_ELEMENT(ID_HOME_CONNECT, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_HOME_SETTINGS, UPDUI_MENUPOPUP | UPDUI_RIBBON)

      UPDATE_ELEMENT(ID_PHOTO_MODE_NORMAL, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PHOTO_MODE_HDR, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PHOTO_MODE_PANO, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PHOTO_MODE_HDR_PANO, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PHOTO_MODE_TIMELAPSE, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PHOTO_MODE_PHOTOSTACK, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PHOTO_MODE_SCRIPTING, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PHOTO_MODE_DEVICE_PROPERTIES, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PHOTO_MODE_IMAGE_PROPERTIES, UPDUI_MENUPOPUP | UPDUI_RIBBON)

      UPDATE_ELEMENT(ID_VIEWFINDER_SHOW, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PREV_IMAGES_SHOW, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEW_RIBBON, UPDUI_MENUPOPUP | UPDUI_RIBBON)
   END_UPDATE_UI_MAP()

   BEGIN_MSG_MAP(MainFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_CLOSE, OnClose)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_MOVE, OnMove)
      MESSAGE_HANDLER(WM_LOCK_ACTIONMODE, OnLockActionMode)
      COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
      COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
      COMMAND_ID_HANDLER(ID_VIEW_RIBBON, OnToggleRibbon)
      COMMAND_ID_HANDLER(ID_HOME_CONNECT, OnHomeConnect)
      COMMAND_ID_HANDLER(ID_HOME_SETTINGS, OnHomeSettings)
      COMMAND_RANGE_HANDLER(ID_PHOTO_MODE_NORMAL, ID_PHOTO_MODE_IMAGE_PROPERTIES, OnPhotoMode)
      COMMAND_ID_HANDLER(ID_VIEWFINDER_SHOW, OnViewfinderShow)
      COMMAND_ID_HANDLER(ID_PREV_IMAGES_SHOW, OnPrevImagesShow)
      COMMAND_ID_HANDLER(ID_PREV_IMAGES_EXIT, OnPrevImagesExit)
      MESSAGE_HANDLER(WM_COMMAND, OnForwardCommandMessage)
      CHAIN_MSG_MAP(BaseClass)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   /// called when main frame is being created
   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   /// called when main frame is being closed by [x]
   LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   /// called when main frame is being destroyed
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
   /// called when main frame is being resized
   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   /// called when main frame is being moved
   LRESULT OnMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   /// called when custom message "lock action mode" is received
   LRESULT OnLockActionMode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   /// called when "File | Exit" menu entry is being selected
   LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when "View | Menu band" menu entry is being selected
   LRESULT OnToggleRibbon(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when "Help | About" menu entry is being selected
   LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when "Home | Connect camera" menu entry is being selected
   LRESULT OnHomeConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when "Home | Settings" menu entry is being selected
   LRESULT OnHomeSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when a photo mode menu entry is being selected
   LRESULT OnPhotoMode(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when "Viewfinder | Show" menu entry is being selected
   LRESULT OnViewfinderShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when "Viewfinder | Previous images" menu entry is being selected
   LRESULT OnPrevImagesShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when "Previous images | Exit" menu entry is being selected
   LRESULT OnPrevImagesExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called for every WM_COMMAND message; forwards it to the photo view
   LRESULT OnForwardCommandMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
   // virtual methods from IPhotoModeViewHost

   virtual std::shared_ptr<RemoteReleaseControl> StartRemoteReleaseControl(bool bStart) override;

   virtual AppSettings& GetAppSettings() throw() override { return m_settings; }

   virtual ImageFileManager& GetImageFileManager() throw() override { return *m_upImageFileManager; }

   virtual PreviousImagesManager& GetPreviousImagesManager() throw() override { return m_previousImagesManager; }

   virtual ViewFinderView* GetViewFinderView() throw() override { return m_upViewFinderView.get(); }

   virtual void SetStatusText(const CString& cszText) override;

   virtual void LockActionMode(bool bLock) override;

   virtual void EnableUI(int nID, bool bEnable) override;

   virtual void OnTransferredImage(const CString& cszFilename) override;

   /// sets up logging
   void SetupLogging();

   /// sets up command bar
   void SetupCmdBar();

   /// sets up ribbon bar
   void SetupRibbonBar();

   /// sets up toolbar
   void SetupToolbar();

   /// sets up status bar
   void SetupStatusBar();

   /// sets new photo mode view
   void SetNewView(T_enViewType enViewType);

   /// shows or hides viewfinder
   void ShowViewfinder(bool bShow);

   /// enables or disables photo modes (and live view)
   void EnablePhotoModes(bool bEnable);

   /// enables or disables scripting related UI elements
   void EnableScriptingUI(bool bScripting);

   /// sets status bar pane widths
   void SetPaneWidths(int* arrWidths, int nPanes);

   /// updates window title
   void UpdateTitle();

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

   /// viewfinder view
   std::unique_ptr<ViewFinderView> m_upViewFinderView;

   /// thread id of UI thread
   DWORD m_dwUIThreadId;

   // model

   /// application settings
   AppSettings m_settings;

   /// image file manager
   std::unique_ptr<ImageFileManager> m_upImageFileManager;

   /// previous images manager
   PreviousImagesManager m_previousImagesManager;

   /// current source device
   std::shared_ptr<SourceDevice> m_spSourceDevice;

   /// current remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;

   /// current view type
   T_enViewType m_enCurrentViewType;

   /// view type saved when opening "previous images" view
   T_enViewType m_enPrevImagesSavedView;
};
