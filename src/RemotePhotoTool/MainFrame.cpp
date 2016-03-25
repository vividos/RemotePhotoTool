//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file MainFrame.cpp Main application frame
//

// includes
#include "stdafx.h"
#include "MainFrame.hpp"
#include "AboutDlg.hpp"
#include "Exception.hpp"
#include "BlankView.hpp"
#include "ConnectCameraDlg.hpp"
#include "SettingsDlg.hpp"
#include "SourceDevice.hpp"
#include "ShutterReleaseSettings.hpp"
#include "ShootingMode.hpp"
#include "ImageFormat.hpp"
#include "Viewfinder.hpp"
#include "CameraException.hpp"
#include "CameraErrorDlg.hpp"
#include "Logging.hpp"
#include "Thread.hpp"

/// settings registry key (subkey "Ribbon" is used for menu band)
LPCTSTR c_pszSettingsRegkey = _T("Software\\RemotePhotoTool");

/// ctor
MainFrame::MainFrame()
:m_hWndView(nullptr),
 m_settings(c_pszSettingsRegkey),
 m_dwUIThreadId(Thread::CurrentId()),
 m_enCurrentViewType(viewBlank),
 m_enPrevImagesSavedView(viewBlank),
 m_iStateEventHandlerId(-1),
 m_iDownloadEventHandlerId(-1),
 m_iImagePropertyHandlerId(-1)
{
   m_settings.Load();

   SetupLogging();

   m_upImageFileManager.reset(new ImageFileManager(m_settings));

   m_releaseSettings.SaveTarget(ShutterReleaseSettings::saveToBoth);
}

MainFrame::~MainFrame() throw()
{
   LOG_TRACE(_T("RemotePhotoTool finished.\n"));
   LOG_TRACE(_T("************************************************************\n"));
}

BOOL MainFrame::PreTranslateMessage(MSG* pMsg)
{
   if (m_upView != nullptr && m_upView->PreTranslateMessage(pMsg))
      return TRUE;

   return BaseClass::PreTranslateMessage(pMsg);
}

BOOL MainFrame::OnIdle()
{
   Instance::OnIdle();

   UIUpdateToolBar();

   // enable contextual tabs
   bool bConnected =
      m_enCurrentViewType != viewBlank &&
      m_enCurrentViewType != viewPreviousImages &&
      m_spSourceDevice != nullptr;

   SetRibbonContextAvail(ID_TAB_GROUP_CONTEXT_CAMERA, bConnected ?
      UI_CONTEXTAVAILABILITY_AVAILABLE : UI_CONTEXTAVAILABILITY_NOTAVAILABLE);

   bool bViewfinderActive = m_upViewFinderView != nullptr;
   SetRibbonContextAvail(ID_TAB_GROUP_CONTEXT_VIEWFINDER, bViewfinderActive ?
      UI_CONTEXTAVAILABILITY_AVAILABLE : UI_CONTEXTAVAILABILITY_NOTAVAILABLE);

   return FALSE;
}

LRESULT MainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   SetupCmdBar();
   SetupRibbonBar();
   SetupToolbar();
   SetupStatusBar();

   // create view
   {
      m_hWndClient = m_splitter.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

      m_upView.reset(new BlankView);

      m_hWndView = m_upView->CreateView(m_splitter);

      m_splitter.SetSplitterPane(SPLIT_PANE_LEFT, m_hWndView, true);
      m_splitter.SetSinglePaneMode(SPLIT_PANE_LEFT);
   }

   // register object for message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != nullptr);
   pLoop->AddMessageFilter(this);
   pLoop->AddIdleHandler(this);

   {
      bool bRibbonUI = RunTimeHelper::IsRibbonUIAvailable();
      ShowRibbonUI(bRibbonUI);
      UISetCheck(ID_VIEW_RIBBON, bRibbonUI);
   }

   UIEnable(ID_VIEWFINDER_SHOW, false);

   UISetCheck(ID_CAMERA_SETTINGS_SAVETO_BOTH, true);

   EnablePhotoModes(false);
   EnableScriptingUI(false);
   EnableCameraUI(false);

   // show the connect dialog
   PostMessage(WM_COMMAND, MAKEWPARAM(ID_HOME_CONNECT, 0), 0);

   return 0;
}

LRESULT MainFrame::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (this->m_spRemoteReleaseControl != NULL)
   {
      int iRet = AtlMessageBox(m_hWnd, _T("You are currently connected to a camera. Really quit RemotePhotoTool?"),
         IDR_MAINFRAME, MB_YESNO | MB_ICONQUESTION);

      if (iRet != IDYES)
         return 0;
   }

   EnableCameraUI(false);

   bHandled = false; // let app process message

   if (RunTimeHelper::IsRibbonUIAvailable())
   {
      bool bRibbonUI = IsRibbonUI();
      if (bRibbonUI)
         SaveRibbonSettings();

      CRibbonPersist(c_pszSettingsRegkey).Save(bRibbonUI, m_hgRibbonSettings);
   }

   return 0;
}

LRESULT MainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   // unregister message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != nullptr);
   pLoop->RemoveMessageFilter(this);
   pLoop->RemoveIdleHandler(this);

   if (m_upView != nullptr)
   {
      m_upView->DestroyView();
      m_upView.reset();
   }

   bHandled = false;
   return 1;
}

LRESULT MainFrame::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   bHandled = false;

   if (m_downloadProgressBar.IsWindow())
   {
      CRect rcPane;
      m_statusBar.GetPaneRect(IDR_PANE_PROGRESS, &rcPane);

      m_downloadProgressBar.MoveWindow(&rcPane, true);
   }

   return 0;
}

LRESULT MainFrame::OnMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   bHandled = false;

   if (m_downloadProgressBar.IsWindow())
   {
      CRect rcPane;
      m_statusBar.GetPaneRect(IDR_PANE_PROGRESS, &rcPane);

      m_downloadProgressBar.MoveWindow(&rcPane, true);
   }

   return 0;
}

LRESULT MainFrame::OnLockActionMode(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   LockActionMode(wParam != 0);
   return 0;
}

LRESULT MainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   PostMessage(WM_CLOSE);
   return 0;
}

LRESULT MainFrame::OnToggleRibbon(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   ShowRibbonUI(!IsRibbonUI());
   UISetCheck(ID_VIEW_RIBBON, IsRibbonUI());
   return 0;
}

LRESULT MainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   AboutDlg dlg;
   dlg.DoModal(m_hWnd);

   return 0;
}

LRESULT MainFrame::OnHomeConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_upView != nullptr)
      m_upView->DestroyView();

   m_upView.reset(new BlankView);
   m_enCurrentViewType = viewBlank;

   ShowViewfinder(false);

   m_spRemoteReleaseControl.reset();
   m_spSourceDevice.reset();

   EnablePhotoModes(false);
   EnableScriptingUI(false);
   EnableCameraUI(false);

   m_hWndView = m_upView->CreateView(m_splitter);

   m_splitter.SetSplitterPane(SPLIT_PANE_LEFT, m_hWndView, true);
   m_splitter.SetSinglePaneMode(SPLIT_PANE_LEFT);

   UpdateLayout();

   ConnectCameraDlg dlg;
   if (IDOK != dlg.DoModal(m_hWnd))
      return 0;

   // set new device remote connection
   m_spSourceDevice = dlg.GetSourceDevice();
   if (m_spSourceDevice == nullptr)
      return 0;

   // start with standard view
   SetNewView(T_enViewType::viewStandard);

   // enable viewfinder button when source device is capable
   bool bViewFinderAvail = m_spSourceDevice->GetDeviceCapability(SourceDevice::capRemoteViewfinder);
   UIEnable(ID_VIEWFINDER_SHOW, bViewFinderAvail);

   UpdateTitle();

   return 0;
}

LRESULT MainFrame::OnHomeSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   bool bPrevLogging = m_settings.m_bLogging;

   SettingsDlg dlg(m_settings);
   int iRet = dlg.DoModal(m_hWnd);
   if (iRet == IDOK)
   {
      m_settings.Store();

      if (!bPrevLogging && m_settings.m_bLogging)
         SetupLogging();

      m_upImageFileManager.reset(new ImageFileManager(m_settings));
   }
   else
      m_settings.Load();

   return 0;
}

LRESULT MainFrame::OnPhotoMode(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   T_enViewType enViewType = T_enViewType::viewStandard;

   switch (LOWORD(wID))
   {
   case ID_PHOTO_MODE_NORMAL:       enViewType = T_enViewType::viewStandard; break;
   case ID_PHOTO_MODE_HDR:          enViewType = T_enViewType::viewHDR; break;
   case ID_PHOTO_MODE_PANO:         enViewType = T_enViewType::viewPanorama; break;
   case ID_PHOTO_MODE_HDR_PANO:     enViewType = T_enViewType::viewHDRPanorama; break;
   case ID_PHOTO_MODE_TIMELAPSE:    enViewType = T_enViewType::viewTimeLapse; break;
   case ID_PHOTO_MODE_PHOTOSTACK:   enViewType = T_enViewType::viewPhotoStacking; break;
   case ID_PHOTO_MODE_SCRIPTING:    enViewType = T_enViewType::viewScripting; break;
   case ID_PHOTO_MODE_DEVICE_PROPERTIES:  enViewType = T_enViewType::viewDeviceProperties; break;
   case ID_PHOTO_MODE_IMAGE_PROPERTIES:   enViewType = T_enViewType::viewImageProperties; break;
   default:
      ATLASSERT(false);
      break;
   }

   SetNewView(enViewType);

   return 0;
}

LRESULT MainFrame::OnViewfinderShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   bool bViewfinderActive = m_upViewFinderView != nullptr;
   ShowViewfinder(!bViewfinderActive);
   return 0;
}

LRESULT MainFrame::OnCameraSettingsSaveToSelChanged(UI_EXECUTIONVERB verb, WORD /*wID*/, UINT uSel, BOOL& /*bHandled*/)
{
   if (verb == UI_EXECUTIONVERB_EXECUTE &&
      uSel != UI_COLLECTION_INVALIDINDEX &&
      m_spRemoteReleaseControl != nullptr)
   {
      ShutterReleaseSettings::T_enSaveTarget enSaveTarget =
         static_cast<ShutterReleaseSettings::T_enSaveTarget>(uSel + 1);

      SetCameraSettingsSaveTo(enSaveTarget);
   }

   return 0;
}

static_assert(
   ID_CAMERA_SETTINGS_SAVETO_CAMERA - ID_CAMERA_SETTINGS_SAVETO_CAMERA + 1 ==
   ShutterReleaseSettings::saveToCamera, "ensure that ID value matches enum value in T_enSaveTarget");

static_assert(
   ID_CAMERA_SETTINGS_SAVETO_PC - ID_CAMERA_SETTINGS_SAVETO_CAMERA + 1 ==
   ShutterReleaseSettings::saveToHost, "ensure that ID value matches enum value in T_enSaveTarget");

static_assert(
   ID_CAMERA_SETTINGS_SAVETO_BOTH - ID_CAMERA_SETTINGS_SAVETO_CAMERA + 1 ==
   ShutterReleaseSettings::saveToBoth, "ensure that ID value matches enum value in T_enSaveTarget");

LRESULT MainFrame::OnCameraSettingsSaveToRange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   ShutterReleaseSettings::T_enSaveTarget enSaveTarget =
      static_cast<ShutterReleaseSettings::T_enSaveTarget>(wID - ID_CAMERA_SETTINGS_SAVETO_CAMERA + 1);

   SetCameraSettingsSaveTo(enSaveTarget);

   return 0;
}

void MainFrame::SetCameraSettingsSaveTo(ShutterReleaseSettings::T_enSaveTarget enSaveTarget)
{
   m_releaseSettings.SaveTarget(enSaveTarget);

   if (m_spRemoteReleaseControl == nullptr)
      return;

   try
   {
      m_spRemoteReleaseControl->SetReleaseSettings(m_releaseSettings);
   }
   catch (CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Error while setting shutter release settings"), ex);
      dlg.DoModal(m_hWnd);
      return;
   }

   UISetCheck(ID_CAMERA_SETTINGS_SAVETO_CAMERA, false);
   UISetCheck(ID_CAMERA_SETTINGS_SAVETO_PC, false);
   UISetCheck(ID_CAMERA_SETTINGS_SAVETO_BOTH, false);

   UINT uiId = static_cast<UINT>(enSaveTarget - ShutterReleaseSettings::saveToCamera) +
      ID_CAMERA_SETTINGS_SAVETO_CAMERA;

   UISetCheck(uiId, true);
}

LRESULT MainFrame::OnCameraSettingsImageFormatSelChanged(UI_EXECUTIONVERB verb, WORD /*wID*/, UINT uSel, BOOL& /*bHandled*/)
{
   if (verb == UI_EXECUTIONVERB_EXECUTE &&
      uSel != UI_COLLECTION_INVALIDINDEX &&
      !m_vecAllImageFormats.empty() &&
      m_spRemoteReleaseControl != nullptr)
   {
      size_t indexImageFormat = uSel;

      const ImageProperty& imageFormat = m_vecAllImageFormats[indexImageFormat];

      m_spRemoteReleaseControl->SetImageProperty(imageFormat);
   }

   return 0;
}

/// \note Normally the ideal place for this handler would be ViewFinderView, but there is a problem:
/// Some of the photo mode view classes have REFLECT_NOTIFICATIONS() in their message maps, and the
/// method MainFrame::OnForwardCommandMessage() forwards WM_COMMAND messages to the photo view first,
/// then to the viewfinder. The ribbon gallery control sends WM_COMMAND messages where lParam is set
/// to the selection in the gallery control, and the function called in REFLECT_NOTIFICATIONS() can't
/// handle lParam values that are not 0 or a valid window, and so gives an assertion.
/// For this reason we handle the messages here and select the appropriate lines mode from here.
LRESULT MainFrame::OnViewfinderLinesModeSelChanged(UI_EXECUTIONVERB verb, WORD /*wID*/, UINT uSel, BOOL& /*bHandled*/)
{
   if (verb == UI_EXECUTIONVERB_EXECUTE &&
      uSel != UI_COLLECTION_INVALIDINDEX &&
      m_upViewFinderView != nullptr)
   {
      ViewFinderImageWindow::T_enLinesMode enLinesMode =
         static_cast<ViewFinderImageWindow::T_enLinesMode>(uSel);

      m_upViewFinderView->SetLinesMode(enLinesMode);
   }

   return 0;
}

LRESULT MainFrame::OnViewfinderLinesModeRange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   ViewFinderImageWindow::T_enLinesMode enLinesMode =
      static_cast<ViewFinderImageWindow::T_enLinesMode>(wID - ID_VIEWFINDER_LINES_MODE_NONE);

   if (enLinesMode > ViewFinderImageWindow::linesModeMax)
      return 0;

   m_upViewFinderView->SetLinesMode(enLinesMode);

   return 0;
}

LRESULT MainFrame::OnViewfinderOutputTypeSelChanged(UI_EXECUTIONVERB verb, WORD /*wID*/, UINT uSel, BOOL& /*bHandled*/)
{
   if (verb == UI_EXECUTIONVERB_EXECUTE &&
      uSel != UI_COLLECTION_INVALIDINDEX &&
      m_upViewFinderView != nullptr)
   {
      // T_enOutputType starts with value 1
      Viewfinder::T_enOutputType enOutputType =
         static_cast<Viewfinder::T_enOutputType>(uSel + 1);

      m_upViewFinderView->SetOutputType(enOutputType);
   }

   return 0;
}


LRESULT MainFrame::OnViewfinderOutputTypeRange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   // T_enOutputType starts with value 1
   Viewfinder::T_enOutputType enOutputType =
      static_cast<Viewfinder::T_enOutputType>(wID - ID_VIEWFINDER_OUTPUT_TYPE + 1);

   m_upViewFinderView->SetOutputType(enOutputType);

   return 0;
}

LRESULT MainFrame::OnPrevImagesShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_enPrevImagesSavedView = m_enCurrentViewType;

   SetNewView(T_enViewType::viewPreviousImages);

   if (IsRibbonUI())
      SetRibbonModes(UI_MAKEAPPMODE(1));

   UIEnable(ID_PREV_IMAGES_EXIT, true);
   UIEnable(ID_PREV_IMAGES_SHOW, false);

   return 0;
}

LRESULT MainFrame::OnPrevImagesExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (IsRibbonUI())
      SetRibbonModes(UI_MAKEAPPMODE(0));

   SetNewView(m_enPrevImagesSavedView);

   UIEnable(ID_PREV_IMAGES_EXIT, false);
   UIEnable(ID_PREV_IMAGES_SHOW, true);

   UIEnable(ID_PREV_IMAGES_OPEN_FOLDER, false);
   UIEnable(ID_PREV_IMAGES_EDIT, false);
   UIEnable(ID_PREV_IMAGES_PREV, false);
   UIEnable(ID_PREV_IMAGES_NEXT, false);

   return 0;
}

LRESULT MainFrame::OnForwardCommandMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   ATLASSERT(uMsg == WM_COMMAND);

   LRESULT lResult = ::SendMessage(m_hWndView, uMsg, wParam, lParam);
   bHandled = false;

   if (m_upViewFinderView != nullptr)
      m_upViewFinderView->SendMessage(uMsg, wParam, lParam);

   return lResult;
}

LPCWSTR MainFrame::OnRibbonQueryCategoryText(UINT32 uCtrlID, UINT32 uCat)
{
   RibbonUI::ICtrl& ctrl = GetRibbonCtrl(uCtrlID);
   IRibbonCombobox* pCombo = dynamic_cast<IRibbonCombobox*>(&ctrl);
   if (pCombo == nullptr)
      return L"Category";

   return pCombo->OnRibbonQueryCategoryText(uCat);
}

UINT32 MainFrame::OnRibbonQueryItemCategory(UINT32 uCtrlID, UINT32 uItem)
{
   RibbonUI::ICtrl& ctrl = GetRibbonCtrl(uCtrlID);
   IRibbonCombobox* pCombo = dynamic_cast<IRibbonCombobox*>(&ctrl);
   if (pCombo == nullptr)
      return 0;

   return pCombo->OnRibbonQueryItemCategory(uItem);
}

LPCWSTR MainFrame::OnRibbonQueryItemText(UINT32 uCtrlID, UINT32 uItem)
{
   RibbonUI::ICtrl& ctrl = GetRibbonCtrl(uCtrlID);
   IRibbonCombobox* pCombo = dynamic_cast<IRibbonCombobox*>(&ctrl);
   if (pCombo == nullptr)
      return DefRibbonQueryItemText(uCtrlID, uItem);

   return pCombo->OnRibbonQueryItemText(uItem);
}

bool MainFrame::OnRibbonQuerySelectedItem(UINT32 uCtrlID, UINT32& uSel)
{
   RibbonUI::ICtrl& ctrl = GetRibbonCtrl(uCtrlID);
   IRibbonCombobox* pCombo = dynamic_cast<IRibbonCombobox*>(&ctrl);
   if (pCombo == nullptr)
      return false;

   return pCombo->OnRibbonQuerySelectedItem(uSel);
}

HBITMAP MainFrame::OnRibbonQueryItemImage(UINT32 uCtrlID, UINT32 uItem)
{
   RibbonUI::ICtrl& ctrl = GetRibbonCtrl(uCtrlID);
   IRibbonCombobox* pCombo = dynamic_cast<IRibbonCombobox*>(&ctrl);
   if (pCombo == nullptr)
      return DefRibbonQueryItemImage(uCtrlID, uItem);

   return pCombo->OnRibbonQueryItemImage(uItem);
}

void MainFrame::SetupLogging()
{
   // set up logging
   Instance::EnableLogging(m_settings.m_bLogging, m_settings.m_cszLogfilePath);

   LOG_TRACE(_T("************************************************************\n"));
   LOG_TRACE(_T("RemotePhotoTool started.\n"));
}

void MainFrame::UpdateTitle()
{
   CString cszTitle(MAKEINTRESOURCE(IDR_MAINFRAME));

   // connected?
   if (IsConnected())
   {
      // app title + camera model name
      cszTitle.AppendFormat(_T(" - %s"),
         m_spSourceDevice->ModelName().GetString());
   }

   // just app title
   SetWindowText(cszTitle);
}

void MainFrame::SetupCmdBar()
{
   // create command bar window
   m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);

   // move menu infos to command bar
   m_CmdBar.AttachMenu(GetMenu());

   m_CmdBar.SetImageMaskColor(RGB(253, 5, 255)); // pink
   m_CmdBar.LoadImages(IDR_MAINFRAME);

   SetMenu(NULL);
}

void MainFrame::SetupRibbonBar()
{
   // check if ribbon is available
   bool bRibbonUI = RunTimeHelper::IsRibbonUIAvailable();

   if (bRibbonUI)
   {
      UIAddMenu(m_CmdBar.GetMenu(), true);

      // also add texts for all menu commands that only appear on ribbon
      CMenu menuRibbonCommands;
      menuRibbonCommands.LoadMenu(IDR_RIBBON_COMMANDS);

      UIAddMenu(menuRibbonCommands, true);

      m_cbCameraSettingsSaveToMode.Select(2);
      m_cbCameraSettingsImageFormat.Select(0);

      m_cbViewfinderLinesMode.Select(0);
      m_cbViewfinderOutputType.Select(2);

      CRibbonPersist(c_pszSettingsRegkey).Restore(bRibbonUI, m_hgRibbonSettings);
   }
   else
      m_CmdBar.GetMenu().DeleteMenu(ID_VIEW_RIBBON, MF_BYCOMMAND);

   // remove photo modes that aren't implemented yet
   m_CmdBar.GetMenu().DeleteMenu(ID_PHOTO_MODE_TIMELAPSE, MF_BYCOMMAND);
   m_CmdBar.GetMenu().DeleteMenu(ID_PHOTO_MODE_PHOTOSTACK, MF_BYCOMMAND);
}

/// sets button text for toolbar button
void SetToolbarButtonText(CToolBarCtrl& tb, int iId, LPCTSTR pszText)
{
   TBBUTTONINFO info = { 0 };
   info.cbSize = sizeof(info);
   info.dwMask = TBIF_STYLE;

   BOOL bRet = tb.GetButtonInfo(iId, &info);
   ATLASSERT(TRUE == bRet); bRet;

   info.fsStyle |= BTNS_SHOWTEXT;

   info.dwMask |= TBIF_TEXT;
   info.pszText = const_cast<LPWSTR>(pszText);
   info.cchText = _tcslen(pszText);

   bRet = tb.SetButtonInfo(iId, &info);
   ATLASSERT(TRUE == bRet); bRet;
}

void MainFrame::SetupToolbar()
{
   CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
   AddSimpleReBarBand(m_CmdBar);

   {
      HWND hWndToolBar =
         CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE,
         ATL_SIMPLE_TOOLBAR_PANE_STYLE | BTNS_SHOWTEXT | TBSTYLE_LIST);

      AddSimpleReBarBand(hWndToolBar, NULL, TRUE);
      UIAddToolBar(hWndToolBar);

      CToolBarCtrl tb(hWndToolBar);
      tb.SetExtendedStyle(TBSTYLE_EX_MIXEDBUTTONS); // needed for BTNS_SHOWTEXT

      bool bRibbonUI = RunTimeHelper::IsRibbonUIAvailable();
      if (!bRibbonUI)
         tb.HideButton(ID_VIEW_RIBBON, true);

      //SetToolbarButtonText(tb, ID_HOME_CONNECT, _T("Connect"));
      //SetToolbarButtonText(tb, ID_HOME_SETTINGS, _T("Settings"));

      // remove photo modes that aren't implemented yet
      tb.HideButton(ID_PHOTO_MODE_TIMELAPSE);
      tb.HideButton(ID_PHOTO_MODE_PHOTOSTACK);
   }
}

/// \see http://www.codeproject.com/Articles/2948/How-to-use-the-WTL-multipane-status-bar-control
void MainFrame::SetupStatusBar()
{
   // added WS_CLIPCHILDREN to default styles
   CreateSimpleStatusBar(
      ATL_IDS_IDLEMESSAGE, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | SBARS_SIZEGRIP);

   m_statusBar.SubclassWindow(m_hWndStatusBar);

   // set status bar panes. ID_DEFAULT_PANE is defined by WTL
   int arrPanes[] = { ID_DEFAULT_PANE, IDR_PANE_PROGRESS };

   m_statusBar.SetPanes(arrPanes, sizeof(arrPanes) / sizeof(int), false);

   // set status bar pane widths using local workaround
   int arrWidths[] = { 0, 150 };
   SetPaneWidths(arrWidths, sizeof(arrWidths) / sizeof(int));
}

std::shared_ptr<RemoteReleaseControl> MainFrame::StartRemoteReleaseControl(bool bStart)
{
   ATLASSERT(m_spSourceDevice != nullptr); // must be connected to device!
   if (m_spSourceDevice == nullptr)
      return std::shared_ptr<RemoteReleaseControl>();

   if (bStart)
   {
      if (m_spRemoteReleaseControl == nullptr)
      {
         try
         {
            m_spRemoteReleaseControl = m_spSourceDevice->EnterReleaseControl();

            EnableCameraUI(true);

            // check viewfinder
            bool bAvailViewfinder = m_spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capViewfinder);

            UIEnable(ID_VIEWFINDER_SHOW, bAvailViewfinder, true);

            // add event handler
            m_iStateEventHandlerId = m_spRemoteReleaseControl->AddStateEventHandler(
               std::bind(&MainFrame::OnStateEvent, this, std::placeholders::_1, std::placeholders::_2));

            m_iDownloadEventHandlerId = m_spRemoteReleaseControl->AddDownloadEventHandler(
               std::bind(&MainFrame::OnDownloadEvent, this, std::placeholders::_1, std::placeholders::_2));
         }
         catch(CameraException& ex)
         {
            CameraErrorDlg dlg(_T("Couldn't start remote release connection"), ex);
            dlg.DoModal(m_hWnd);

            m_spRemoteReleaseControl.reset();

            UIEnable(ID_VIEWFINDER_SHOW, false, true);
            EnableCameraUI(false);
         }
      }
   }
   else
   {
      if (m_spRemoteReleaseControl != nullptr)
      {
         m_spRemoteReleaseControl->RemoveStateEventHandler(m_iStateEventHandlerId);
         m_iStateEventHandlerId = -1;

         m_spRemoteReleaseControl->RemoveDownloadEventHandler(m_iDownloadEventHandlerId);
         m_iDownloadEventHandlerId = -1;
      }

      m_spRemoteReleaseControl.reset();

      UIEnable(ID_VIEWFINDER_SHOW, false, true);
      EnableCameraUI(false);
   }

   return m_spRemoteReleaseControl;
}

void MainFrame::SetStatusText(const CString& cszText, unsigned int uiPane)
{
   if (uiPane == 0)
      ::SetWindowText(m_hWndStatusBar, cszText);
   else
      if (uiPane == 1)
         m_statusBar.SetPaneText(IDR_PANE_PROGRESS, cszText);
}

void MainFrame::LockActionMode(bool bLock)
{
   if (Thread::CurrentId() != m_dwUIThreadId)
   {
      PostMessage(WM_LOCK_ACTIONMODE, bLock ? 1 : 0);
      return;
   }

   EnablePhotoModes(!bLock);
}

void MainFrame::EnableUI(int nID, bool bEnable)
{
   UIEnable(nID, bEnable);
}

void MainFrame::OnTransferredImage(const CString& cszFilename)
{
   m_previousImagesManager.AddNewImage(cszFilename);
}

void MainFrame::SetNewView(T_enViewType enViewType)
{
   if (m_enCurrentViewType == enViewType)
      return; // already in view

   SetRibbonContextAvail(ID_TAB_GROUP_CONTEXT_SCRIPTING, UI_CONTEXTAVAILABILITY_NOTAVAILABLE);

   if (m_upView != nullptr)
   {
      if (!m_upView->CanClose())
         return; // view prevented close

      m_upView->DestroyView();
   }

   m_upView = ViewManager::CreateView(*this, enViewType);
   m_upView->SetSourceDevice(m_spSourceDevice);

   m_hWndView = m_upView->CreateView(m_splitter);
   if (m_hWndView == nullptr)
   {
      m_upView.reset();

      PostMessage(WM_COMMAND, MAKEWPARAM(ID_HOME_CONNECT, 0), 0);
      return;
   }

   m_splitter.SetSplitterPane(SPLIT_PANE_LEFT, m_hWndView, true);

   if (m_upViewFinderView != nullptr)
   {
      m_splitter.SetSplitterPanes(m_hWndView, *m_upViewFinderView);
      m_splitter.SetSinglePaneMode(SPLIT_PANE_NONE);
   }
   else
      m_splitter.SetSinglePaneMode(SPLIT_PANE_LEFT);

   bool bEnable =
      enViewType != viewBlank &&
      enViewType != viewPreviousImages &&
      enViewType != viewScripting;

   EnablePhotoModes(bEnable);

   bool bScripting = enViewType == viewScripting;
   EnableScriptingUI(bScripting);

   // force layout update so that new view gets resized properly
   UpdateLayout();

   m_enCurrentViewType = enViewType;
}

void MainFrame::ShowViewfinder(bool bShow)
{
   UIEnable(ID_VIEWFINDER_AUTO_FOCUS, bShow);
   UIEnable(ID_VIEWFINDER_AUTO_WB, bShow);
   UIEnable(ID_VIEWFINDER_ZOOM_IN, bShow);
   UIEnable(ID_VIEWFINDER_ZOOM_OUT, bShow);
   UIEnable(ID_VIEWFINDER_LINES_MODE, bShow);
   UIEnable(ID_VIEWFINDER_LINES_MODE_NONE, bShow);
   UIEnable(ID_VIEWFINDER_LINES_MODE_RULEOFTHIRDS, bShow);
   UIEnable(ID_VIEWFINDER_LINES_MODE_GOLDENRATIO, bShow);
   UIEnable(ID_VIEWFINDER_OUTPUT_TYPE, bShow);
   UIEnable(ID_VIEWFINDER_OUTPUT_TYPE_LCD, bShow);
   UIEnable(ID_VIEWFINDER_OUTPUT_TYPE_VIDEO_OUT, bShow);
   UIEnable(ID_VIEWFINDER_OUTPUT_TYPE_OFF, bShow);
   UIEnable(ID_VIEWFINDER_SHOW_OVEREXPOSED, bShow);
   UIEnable(ID_VIEWFINDER_SHOW_OVERLAY_IMAGE, bShow);
   UIEnable(ID_VIEWFINDER_HISTOGRAM, bShow);

   if (!bShow)
   {
      if (m_upViewFinderView == nullptr)
         return;

      m_splitter.SetSplitterPane(SPLIT_PANE_LEFT, m_hWndView, true);
      m_splitter.SetSinglePaneMode(SPLIT_PANE_LEFT);

      m_upViewFinderView->SetViewfinder(std::shared_ptr<Viewfinder>());

      m_upViewFinderView->DestroyWindow();

      m_upViewFinderView.reset();

      return;
   }

   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl = StartRemoteReleaseControl(true);
   if (spRemoteReleaseControl == nullptr)
      return; // couldn't start release control

   bool bAvailViewfinder = m_spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capViewfinder);
   if (!bAvailViewfinder)
      return; // viewfinder not avail; button should be disabled already, though

   if (m_upViewFinderView != nullptr)
      m_upViewFinderView->SetViewfinder(std::shared_ptr<Viewfinder>());

   m_upViewFinderView.reset(new ViewFinderView(m_spRemoteReleaseControl));
   m_upViewFinderView->Create(m_splitter);

   try
   {
      std::shared_ptr<Viewfinder> spViewfinder = m_spRemoteReleaseControl->StartViewfinder();

      m_upViewFinderView->SetViewfinder(spViewfinder);
   }
   catch(CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't start viewfinder"), ex);
      dlg.DoModal(m_hWnd);
   }

   m_splitter.SetSplitterPanes(m_hWndView, *m_upViewFinderView);
   m_splitter.SetSinglePaneMode(SPLIT_PANE_NONE);
   m_splitter.SetSplitterPosPct(30);

   SetRibbonContextAvail(ID_TAB_GROUP_CONTEXT_VIEWFINDER, UI_CONTEXTAVAILABILITY_ACTIVE);
}

void MainFrame::EnablePhotoModes(bool bEnable)
{
   UIEnable(ID_PHOTO_MODE_NORMAL, bEnable);
   UIEnable(ID_PHOTO_MODE_HDR, bEnable);
   UIEnable(ID_PHOTO_MODE_PANO, bEnable);
   UIEnable(ID_PHOTO_MODE_HDR_PANO, bEnable);
   UIEnable(ID_PHOTO_MODE_TIMELAPSE, bEnable);
   UIEnable(ID_PHOTO_MODE_PHOTOSTACK, bEnable);
   UIEnable(ID_PHOTO_MODE_SCRIPTING, true); // scripting is always enabled
   UIEnable(ID_PHOTO_MODE_DEVICE_PROPERTIES, bEnable);
   UIEnable(ID_PHOTO_MODE_IMAGE_PROPERTIES, bEnable);

   if (bEnable)
   {
      bool bViewFinderAvail =
         m_spSourceDevice != nullptr &&
         m_spSourceDevice->GetDeviceCapability(SourceDevice::capRemoteViewfinder);

      UIEnable(ID_VIEWFINDER_SHOW, bViewFinderAvail);
   }
   else
      UIEnable(ID_VIEWFINDER_SHOW, false);
}

void MainFrame::EnableScriptingUI(bool bScripting)
{
   SetRibbonContextAvail(ID_TAB_GROUP_CONTEXT_SCRIPTING,
      bScripting ? UI_CONTEXTAVAILABILITY_ACTIVE : UI_CONTEXTAVAILABILITY_NOTAVAILABLE);

   UIEnable(ID_SCRIPTING_OPEN, bScripting);
   UIEnable(ID_SCRIPTING_RELOAD, false); // enabled when a script is loaded
   UIEnable(ID_SCRIPTING_RUN, bScripting);
   UIEnable(ID_SCRIPTING_STOP, false); // always stopped
   UIEnable(ID_SCRIPTING_EDIT, false); // enabled when a script is loaded
}

void MainFrame::EnableCameraUI(bool bEnable)
{
   UIEnable(ID_CAMERA_RELEASE, bEnable);
   UIEnable(ID_CAMERA_SHOOTING_MODE, bEnable);
   UIEnable(ID_CAMERA_SETTINGS_AV, bEnable);
   UIEnable(ID_CAMERA_SETTINGS_TV, bEnable);
   UIEnable(ID_CAMERA_SETTINGS_EXPOSURE, bEnable);
   UIEnable(ID_CAMERA_SETTINGS_ISO, bEnable);
   UIEnable(ID_CAMERA_SETTINGS_SAVETO, bEnable);
   UIEnable(ID_CAMERA_SETTINGS_SAVETO_CAMERA, bEnable);
   UIEnable(ID_CAMERA_SETTINGS_SAVETO_PC, bEnable);
   UIEnable(ID_CAMERA_SETTINGS_SAVETO_BOTH, bEnable);
   UIEnable(ID_CAMERA_SETTINGS_IMAGE_FORMAT, bEnable);

   if (bEnable && m_spRemoteReleaseControl != nullptr)
   {
      SetupImagePropertyManager();

      // shooting mode change supported?
      bool bChangePossible = m_spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capChangeShootingMode);
      UIEnable(ID_CAMERA_SHOOTING_MODE, bChangePossible);

      // wait for changes in shooting mode property
      m_iImagePropertyHandlerId = m_spRemoteReleaseControl->AddPropertyEventHandler(
         std::bind(&MainFrame::OnUpdatedImageProperty, this, std::placeholders::_1, std::placeholders::_2));

      // update list of image formats
      unsigned int uiPropertyId = m_spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propImageFormat);
      m_spRemoteReleaseControl->EnumImagePropertyValues(uiPropertyId, m_vecAllImageFormats);

      size_t maxImageFormat = m_vecAllImageFormats.size();
      m_cbCameraSettingsImageFormat.Resize(maxImageFormat);

      for (size_t indexImageFormat = 0; indexImageFormat < maxImageFormat; indexImageFormat++)
      {
         ImageFormat imageFormat(m_vecAllImageFormats[indexImageFormat]);

         m_cbCameraSettingsImageFormat.SetItemText(indexImageFormat, imageFormat.ToString());
      }
   }

   if (!bEnable)
   {
      if (m_iImagePropertyHandlerId != -1 && m_spRemoteReleaseControl != nullptr)
         m_spRemoteReleaseControl->RemovePropertyEventHandler(m_iImagePropertyHandlerId);

      m_upImagePropertyValueManager.reset();

      m_vecAllImageFormats.clear();
   }
}

void MainFrame::OnUpdatedImageProperty(RemoteReleaseControl::T_enPropertyEvent enPropertyEvent, unsigned int uiValue)
{
   if (enPropertyEvent == RemoteReleaseControl::propEventPropertyChanged &&
      m_spRemoteReleaseControl != nullptr &&
      uiValue == m_spRemoteReleaseControl->MapImagePropertyTypeToId(propShootingMode))
   {
      // shooting mode has changed; update some fields
      UpdateShootingModeDependentValues();
   }
}

void MainFrame::SetupImagePropertyManager()
{
   m_upImagePropertyValueManager.reset(new ImagePropertyValueManager(*m_spRemoteReleaseControl));

   m_cbCameraSettingsShootingMode.SetRemoteReleaseControl(m_spRemoteReleaseControl);
   m_cbCameraSettingsAv.SetRemoteReleaseControl(m_spRemoteReleaseControl);
   m_cbCameraSettingsTv.SetRemoteReleaseControl(m_spRemoteReleaseControl);
   m_cbCameraSettingsExposure.SetRemoteReleaseControl(m_spRemoteReleaseControl);
   m_cbCameraSettingsIso.SetRemoteReleaseControl(m_spRemoteReleaseControl);

   m_upImagePropertyValueManager->AddControl(m_cbCameraSettingsShootingMode);
   m_upImagePropertyValueManager->AddControl(m_cbCameraSettingsAv);
   m_upImagePropertyValueManager->AddControl(m_cbCameraSettingsTv);
   m_upImagePropertyValueManager->AddControl(m_cbCameraSettingsExposure);
   m_upImagePropertyValueManager->AddControl(m_cbCameraSettingsIso);

   m_upImagePropertyValueManager->UpdateControls();

   m_upImagePropertyValueManager->UpdateProperty(
      m_spRemoteReleaseControl->MapImagePropertyTypeToId(propShootingMode));
}

void MainFrame::UpdateShootingModeDependentValues()
{
   ShootingMode shootingMode(m_spRemoteReleaseControl);

   ImageProperty currentMode = shootingMode.Current();

   bool bIsM = currentMode.Value() == shootingMode.Manual().Value();
   bool bIsAv = currentMode.Value() == shootingMode.Av().Value();
   bool bIsTv = currentMode.Value() == shootingMode.Tv().Value();
   bool bIsP = currentMode.Value() == shootingMode.Program().Value();

   bool bReadOnlyAv = !bIsM && (bIsTv || bIsP);
   bool bReadOnlyTv = !bIsM && (bIsAv || bIsP);
   bool bReadOnlyExp = bIsM;

   m_cbCameraSettingsAv.UpdateValuesList();
   m_cbCameraSettingsAv.UpdateValue();
   UIEnable(ID_CAMERA_SETTINGS_AV, !bReadOnlyAv);

   m_cbCameraSettingsTv.UpdateValuesList();
   m_cbCameraSettingsTv.UpdateValue();
   UIEnable(ID_CAMERA_SETTINGS_TV, !bReadOnlyTv);

   m_cbCameraSettingsExposure.UpdateValuesList();
   m_cbCameraSettingsExposure.UpdateValue();
   UIEnable(ID_CAMERA_SETTINGS_EXPOSURE, !bReadOnlyExp);
}

void MainFrame::SetPaneWidths(int* arrWidths, int nPanes)
{
   // find the size of the borders
   int arrBorders[3];
   m_statusBar.GetBorders(arrBorders);

   // calculate right edge of default pane (0)
   arrWidths[0] += arrBorders[2];
   for (int i = 1; i < nPanes; i++)
      arrWidths[0] += arrWidths[i];

   // calculate right edge of remaining panes (1 thru nPanes-1)
   for (int j = 1; j < nPanes; j++)
      arrWidths[j] += arrBorders[2] + arrWidths[j - 1];

   // set the pane widths
   m_statusBar.SetParts(m_statusBar.m_nPanes, arrWidths);
}

void MainFrame::OnStateEvent(RemoteReleaseControl::T_enStateEvent enStateEvent, unsigned int uiExtraData)
{
   if (enStateEvent == RemoteReleaseControl::stateEventCameraShutdown)
   {
      // quit viewfinder
      if (m_upViewFinderView != nullptr)
         ShowViewfinder(false);

      // quit remote release
      StartRemoteReleaseControl(false);

      // clear source device
      m_spSourceDevice.reset();

      EnablePhotoModes(false);
      EnableScriptingUI(false);
      EnableCameraUI(false);

      UpdateTitle();

      // show connect dialog
      PostMessage(WM_COMMAND, MAKEWPARAM(ID_HOME_CONNECT, 0));

      SetStatusText(_T("Camera was shut down"));
   }

   if (enStateEvent == RemoteReleaseControl::stateEventReleaseError)
   {
      LockActionMode(false);

      if (uiExtraData == 0x8d01) // EDSDK specific
         SetStatusText(_T("Couldn't focus"));
      else
         SetStatusText(_T("Error releasing shutter"));
   }
}

void MainFrame::OnDownloadEvent(RemoteReleaseControl::T_enDownloadEvent enDownloadEvent, unsigned int uiProgress)
{
   // multi status bar/progress bar handling, see here:
   // http://msdn.microsoft.com/en-us/magazine/cc188794.aspx

   switch(enDownloadEvent)
   {
   case RemoteReleaseControl::downloadEventStarted:
      {
         if (m_downloadProgressBar.m_hWnd != NULL)
            m_downloadProgressBar.DestroyWindow();

         CRect rcPane;
         m_statusBar.GetPaneRect(IDR_PANE_PROGRESS, &rcPane);

         m_downloadProgressBar.Create(m_statusBar, rcPane, NULL, WS_CHILD | WS_VISIBLE);
         m_downloadProgressBar.ShowWindow(SW_SHOW);

         m_downloadProgressBar.SetRange(0, 100);
      }
      break;

   case RemoteReleaseControl::downloadEventInProgress:
      if (m_downloadProgressBar.IsWindow())
      {
         m_downloadProgressBar.SetPos(uiProgress);
      }
      break;

   case RemoteReleaseControl::downloadEventFinished:
      m_downloadProgressBar.DestroyWindow();
      break;
   }
}
