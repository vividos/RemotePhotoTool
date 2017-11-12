//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
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
#include "ImagePropertyValueManager.hpp"
#include "ImagePropertyRibbonCombobox.hpp"
#include "AppSettings.hpp"
#include "ImageFileManager.hpp"
#include "ShutterReleaseSettings.hpp"
#include "PreviousImagesManager.hpp"
#include "RemoteReleaseControl.hpp"
#include "CameraConnectionManager.hpp"

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
   virtual ~MainFrame();

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
      RIBBON_CONTROL(m_cbCameraSettingsSaveToMode)
      RIBBON_CONTROL(m_cbCameraSettingsShootingMode)
      RIBBON_CONTROL(m_cbCameraSettingsAv)
      RIBBON_CONTROL(m_cbCameraSettingsTv)
      RIBBON_CONTROL(m_cbCameraSettingsExposure)
      RIBBON_CONTROL(m_cbCameraSettingsIso)
      RIBBON_CONTROL(m_cbCameraSettingsImageFormat)
      RIBBON_CONTROL(m_cbViewfinderLinesMode)
      RIBBON_CONTROL(m_cbViewfinderOutputType)
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
      UPDATE_ELEMENT(ID_PHOTO_MODE_CAMERA_FILE_SYSTEM, UPDUI_MENUPOPUP | UPDUI_RIBBON)

      UPDATE_ELEMENT(ID_CAMERA_RELEASE, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_CAMERA_SHOOTING_MODE, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_CAMERA_SETTINGS_AV, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_CAMERA_SETTINGS_TV, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_CAMERA_SETTINGS_EXPOSURE, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_CAMERA_SETTINGS_ISO, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_CAMERA_SETTINGS_SAVETO, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_CAMERA_SETTINGS_IMAGE_FORMAT, UPDUI_MENUPOPUP | UPDUI_RIBBON)

      UPDATE_ELEMENT(ID_PREV_IMAGES_SHOW, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PREV_IMAGES_EXIT, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PREV_IMAGES_OPEN_FOLDER, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PREV_IMAGES_EDIT, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PREV_IMAGES_PREV, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_PREV_IMAGES_NEXT, UPDUI_MENUPOPUP | UPDUI_RIBBON)

      UPDATE_ELEMENT(ID_VIEWFINDER_SHOW, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_AUTO_FOCUS, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_AUTO_WB, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_ZOOM_IN, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_ZOOM_OUT, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_LINES_MODE, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_LINES_MODE_NONE, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_LINES_MODE_RULEOFTHIRDS, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_LINES_MODE_GOLDENRATIO, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_OUTPUT_TYPE, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_OUTPUT_TYPE_LCD, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_OUTPUT_TYPE_VIDEO_OUT, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_OUTPUT_TYPE_OFF, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_SHOW_OVEREXPOSED, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_SHOW_OVERLAY_IMAGE, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_VIEWFINDER_HISTOGRAM, UPDUI_MENUPOPUP | UPDUI_RIBBON)

      UPDATE_ELEMENT(ID_SCRIPTING_OPEN, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_SCRIPTING_RELOAD, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_SCRIPTING_RUN, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_SCRIPTING_STOP, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_SCRIPTING_EDIT, UPDUI_MENUPOPUP | UPDUI_RIBBON)

      UPDATE_ELEMENT(ID_VIEW_RIBBON, UPDUI_MENUPOPUP | UPDUI_RIBBON)
   END_UPDATE_UI_MAP()

   BEGIN_MSG_MAP(MainFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
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
      COMMAND_RANGE_HANDLER(ID_PHOTO_MODE_NORMAL, ID_PHOTO_MODE_CAMERA_FILE_SYSTEM, OnPhotoMode)
      COMMAND_ID_HANDLER(ID_VIEWFINDER_SHOW, OnViewfinderShow)
      RIBBON_GALLERY_CONTROL_HANDLER(ID_CAMERA_SETTINGS_SAVETO, OnCameraSettingsSaveToSelChanged)
      COMMAND_RANGE_HANDLER(ID_CAMERA_SETTINGS_SAVETO_CAMERA, ID_CAMERA_SETTINGS_SAVETO_BOTH, OnCameraSettingsSaveToRange)
      RIBBON_GALLERY_CONTROL_HANDLER(ID_CAMERA_SETTINGS_IMAGE_FORMAT, OnCameraSettingsImageFormatSelChanged)
      RIBBON_GALLERY_CONTROL_HANDLER(ID_VIEWFINDER_LINES_MODE, OnViewfinderLinesModeSelChanged)
      RIBBON_GALLERY_CONTROL_HANDLER(ID_VIEWFINDER_OUTPUT_TYPE, OnViewfinderOutputTypeSelChanged)
      RIBBON_COMBOBOX_IMAGE_PROPERTY(ID_CAMERA_SHOOTING_MODE, m_cbCameraSettingsShootingMode)
      RIBBON_COMBOBOX_IMAGE_PROPERTY(ID_CAMERA_SETTINGS_AV, m_cbCameraSettingsAv)
      RIBBON_COMBOBOX_IMAGE_PROPERTY(ID_CAMERA_SETTINGS_TV, m_cbCameraSettingsTv)
      RIBBON_COMBOBOX_IMAGE_PROPERTY(ID_CAMERA_SETTINGS_EXPOSURE, m_cbCameraSettingsExposure)
      RIBBON_COMBOBOX_IMAGE_PROPERTY(ID_CAMERA_SETTINGS_ISO, m_cbCameraSettingsIso)
      COMMAND_RANGE_HANDLER(ID_VIEWFINDER_LINES_MODE_NONE, ID_VIEWFINDER_LINES_MODE_GOLDENRATIO, OnViewfinderLinesModeRange)
      COMMAND_RANGE_HANDLER(ID_VIEWFINDER_OUTPUT_TYPE_LCD, ID_VIEWFINDER_OUTPUT_TYPE_OFF, OnViewfinderOutputTypeRange)
      COMMAND_ID_HANDLER(ID_PREV_IMAGES_SHOW, OnPrevImagesShow)
      COMMAND_ID_HANDLER(ID_PREV_IMAGES_EXIT, OnPrevImagesExit)
      MESSAGE_HANDLER(WM_COMMAND, OnForwardCommandMessage)
      CHAIN_MSG_MAP(BaseClass)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
// LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)

   /// called when main frame is being created
   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called when window is about to being shown
   LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called when main frame is being closed by [x]
   LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called when main frame is being destroyed
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called when main frame is being resized
   LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called when main frame is being moved
   LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called when custom message "lock action mode" is received
   LRESULT OnLockActionMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when "File | Exit" menu entry is being selected
   LRESULT OnFileExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when "View | Menu band" menu entry is being selected
   LRESULT OnToggleRibbon(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when "Help | About" menu entry is being selected
   LRESULT OnAppAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when "Home | Connect camera" menu entry is being selected
   LRESULT OnHomeConnect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when "Home | Settings" menu entry is being selected
   LRESULT OnHomeSettings(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when a photo mode menu entry is being selected
   LRESULT OnPhotoMode(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when "Viewfinder | Show" menu entry is being selected
   LRESULT OnViewfinderShow(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when a selection in the ribbon combobox for "save to" camera settings was made
   LRESULT OnCameraSettingsSaveToSelChanged(UI_EXECUTIONVERB verb, WORD wID, UINT uSel, BOOL& bHandled);
   /// called when an entry in "Camera | Save to" submenu entry is being selected
   LRESULT OnCameraSettingsSaveToRange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when a selection in the ribbon combobox for "image format" camera settings was made
   LRESULT OnCameraSettingsImageFormatSelChanged(UI_EXECUTIONVERB verb, WORD wID, UINT uSel, BOOL& bHandled);

   /// called when a selection in the ribbon combobox for viewfinder lines mode was made
   LRESULT OnViewfinderLinesModeSelChanged(UI_EXECUTIONVERB verb, WORD wID, UINT uSel, BOOL& bHandled);
   /// called when a lines mode in "Viewfinder | Lines mode" submenu entry is being selected
   LRESULT OnViewfinderLinesModeRange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when a selection in the ribbon combobox for viewfinder output type was made
   LRESULT OnViewfinderOutputTypeSelChanged(UI_EXECUTIONVERB verb, WORD wID, UINT uSel, BOOL& bHandled);
   /// called when output type in "Viewfinder | Output type" submenu entry is being selected
   LRESULT OnViewfinderOutputTypeRange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when "View | Previous images" menu entry is being selected
   LRESULT OnPrevImagesShow(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when "Previous images | Exit" menu entry is being selected
   LRESULT OnPrevImagesExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called for every WM_COMMAND message; forwards it to the photo view
   LRESULT OnForwardCommandMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   // ribbon framework methods

   /// called to query category text for category number
   LPCWSTR OnRibbonQueryCategoryText(UINT32 uCtrlID, UINT32 uCat);

   /// called to query item category for item
   UINT32 OnRibbonQueryItemCategory(UINT32 uCtrlID, UINT32 uItem);

   /// called to query item text for item
   LPCWSTR OnRibbonQueryItemText(UINT32 uCtrlID, UINT32 uItem);

   /// called to query currently selected item
   bool OnRibbonQuerySelectedItem(UINT32 uCtrlID, UINT32& uSel);

   /// called to query item image for item
   HBITMAP OnRibbonQueryItemImage(UINT32 uCtrlID, UINT32 uItem);

private:
   // virtual methods from IPhotoModeViewHost

   virtual std::shared_ptr<SourceDevice> GetSourceDevice() override;

   virtual std::shared_ptr<RemoteReleaseControl> GetRemoteReleaseControl() override;

   virtual AppSettings& GetAppSettings() override { return m_settings; }

   virtual ImageFileManager& GetImageFileManager() override { return *m_upImageFileManager; }

   virtual PreviousImagesManager& GetPreviousImagesManager() override { return m_previousImagesManager; }

   virtual ViewFinderView* GetViewFinderView() override { return m_upViewFinderView.get(); }

   virtual ShutterReleaseSettings& GetReleaseSettings() override { return m_releaseSettings; }

   virtual void SetStatusText(const CString& cszText, unsigned int uiPane = 0) override;

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

   /// disconnects camera and updates UI
   void DisconnectCamera();

   /// shows or hides viewfinder
   void ShowViewfinder(bool bShow);

   /// enables or disables photo modes (and live view)
   void EnablePhotoModes(bool bEnable);

   /// enables or disables viewfinder UI element, depending on if it's available
   void EnableViewfinder(bool bEnable);

   /// enables or disables viewfinder ribbon commands
   void EnableViewfinderCommands(bool bEnable);

   /// enables or disables scripting related UI elements
   void EnableScriptingUI(bool bScripting);

   /// enables or disables camera related UI elements
   void EnableCameraUI(bool bEnable);

   /// stores window position of main frame
   void StoreWindowPosition();

   /// restores window position of main frame from stored settings
   void RestoreWindowPosition();

   /// called when image property has been updated
   void OnUpdatedImageProperty(RemoteReleaseControl::T_enPropertyEvent enPropertyEvent, unsigned int uiValue);

   /// sets up image property manager
   void SetupImagePropertyManager();

   /// cleans up image property manager
   void CleanupImagePropertyManager();

   /// updates values that depend on shooting mode changes
   void UpdateShootingModeDependentValues();

   /// sets new "Save to" camera settings value
   void SetCameraSettingsSaveTo(ShutterReleaseSettings::T_enSaveTarget enSaveTarget);

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

   /// ribbon item gallery for camera settings "save to" mode
   CRibbonItemGalleryCtrl<ID_CAMERA_SETTINGS_SAVETO, 3> m_cbCameraSettingsSaveToMode;

   /// ribbon item gallery for image format
   CRibbonItemGalleryCtrl<ID_CAMERA_SETTINGS_IMAGE_FORMAT, 100> m_cbCameraSettingsImageFormat;

   /// handler id for image property changes
   int m_iImagePropertyHandlerId;

   /// image property value manager
   std::unique_ptr<ImagePropertyValueManager> m_upImagePropertyValueManager;

   /// shooting mode combobox
   ImagePropertyRibbonCombobox<ID_CAMERA_SHOOTING_MODE, T_enImagePropertyType::propShootingMode> m_cbCameraSettingsShootingMode;

   /// aperture ribbon combobox
   ImagePropertyRibbonCombobox<ID_CAMERA_SETTINGS_AV, T_enImagePropertyType::propAv> m_cbCameraSettingsAv;

   /// shutter speed ribbon combobox
   ImagePropertyRibbonCombobox<ID_CAMERA_SETTINGS_TV, T_enImagePropertyType::propTv> m_cbCameraSettingsTv;

   /// exposure value ribbon combobox
   ImagePropertyRibbonCombobox<ID_CAMERA_SETTINGS_EXPOSURE, T_enImagePropertyType::propExposureCompensation> m_cbCameraSettingsExposure;

   /// ISO setting ribbon combobox
   ImagePropertyRibbonCombobox<ID_CAMERA_SETTINGS_ISO, T_enImagePropertyType::propISOSpeed> m_cbCameraSettingsIso;

   /// ribbon item gallery for viewfinder lines mode
   CRibbonItemGalleryCtrl<ID_VIEWFINDER_LINES_MODE, 3> m_cbViewfinderLinesMode;

   /// ribbon item gallery for viewfinder output type
   CRibbonItemGalleryCtrl<ID_VIEWFINDER_OUTPUT_TYPE, 3> m_cbViewfinderOutputType;

   // model

   /// application settings
   AppSettings m_settings;

   /// manager for camera connection
   CameraConnectionManager m_connection;

   /// image file manager
   std::unique_ptr<ImageFileManager> m_upImageFileManager;

   /// previous images manager
   PreviousImagesManager m_previousImagesManager;

   /// handler id for state event handler
   int m_iStateEventHandlerId;

   /// handler id for download event handler
   int m_iDownloadEventHandlerId;

   /// current release settings
   ShutterReleaseSettings m_releaseSettings;

   /// all image formats currently available for selection
   std::vector<ImageProperty> m_vecAllImageFormats;

   /// current view type
   T_enViewType m_enCurrentViewType;

   /// view type saved when opening "previous images" view
   T_enViewType m_enPrevImagesSavedView;
};
