//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file RemotePhotoTool/MainFrame.cpp Main application frame
//

// includes
#include "stdafx.h"
#include "MainFrame.hpp"
#include "AboutDlg.hpp"
#include <ulib/Exception.hpp>
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
#include "TimelapseVideoOptionsDlg.hpp"
#include "FfmpegInterface.hpp"
#include <ulib/Path.hpp>
#include <ulib/thread/Thread.hpp>

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

MainFrame::~MainFrame()
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
   bool hasReleaseControl =
      m_enCurrentViewType != viewBlank &&
      m_enCurrentViewType != viewPreviousImages &&
      m_connection.HasReleaseControl();

   SetRibbonContextAvail(ID_TAB_GROUP_CONTEXT_CAMERA, hasReleaseControl ?
      UI_CONTEXTAVAILABILITY_AVAILABLE : UI_CONTEXTAVAILABILITY_NOTAVAILABLE);

   bool isViewfinderActive = m_upViewFinderView != nullptr;
   SetRibbonContextAvail(ID_TAB_GROUP_CONTEXT_VIEWFINDER, isViewfinderActive ?
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

   UISetCheck(ID_CAMERA_SETTINGS_SAVETO_BOTH, true);

   DisconnectCamera(); // to init UI states

   // show the connect dialog
   PostMessage(WM_COMMAND, MAKEWPARAM(ID_HOME_CONNECT, 0), 0);

   return 0;
}

LRESULT MainFrame::OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   static bool s_inited = false;

   if (!s_inited)
   {
      s_inited = true;
      RestoreWindowPosition();
   }

   return 0;
}

LRESULT MainFrame::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   bHandled = false; // let app process message

   if (m_connection.IsConnected())
   {
      int iRet = AtlMessageBox(m_hWnd, _T("You are currently connected to a camera. Really quit RemotePhotoTool?"),
         IDR_MAINFRAME, MB_YESNO | MB_ICONQUESTION);

      if (iRet != IDYES)
         return 0;
   }

   DisconnectCamera();

   if (RunTimeHelper::IsRibbonUIAvailable())
   {
      bool bRibbonUI = IsRibbonUI();
      if (bRibbonUI)
         SaveRibbonSettings();

      CRibbonPersist(c_pszSettingsRegkey).Save(bRibbonUI, m_hgRibbonSettings);
   }

   StoreWindowPosition();

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

   DisconnectCamera();

   m_hWndView = m_upView->CreateView(m_splitter);

   m_splitter.SetSplitterPane(SPLIT_PANE_LEFT, m_hWndView, true);
   m_splitter.SetSinglePaneMode(SPLIT_PANE_LEFT);

   UpdateLayout();

   try
   {
      ConnectCameraDlg dlg;
      if (IDOK != dlg.DoModal(m_hWnd))
         return 0;

      // set new device remote connection
      std::shared_ptr<SourceDevice> spSourceDevice = dlg.GetSourceDevice();

      if (spSourceDevice == nullptr)
         return 0;

      if (m_connection.Connect(m_hWnd, spSourceDevice))
      {
         if (m_connection.HasReleaseControl())
         {
            EnableCameraUI(true);
            EnableViewfinder(true);

            // add event handler
            m_iStateEventHandlerId = m_connection.GetRemoteReleaseControl()->AddStateEventHandler(
               std::bind(&MainFrame::OnStateEvent, this, std::placeholders::_1, std::placeholders::_2));

            m_iDownloadEventHandlerId = m_connection.GetRemoteReleaseControl()->AddDownloadEventHandler(
               std::bind(&MainFrame::OnDownloadEvent, this, std::placeholders::_1, std::placeholders::_2));
         }
      }
   }
   catch (...)
   {
      return 0;
   }

   // start with standard view or file system, depending on release control
   SetNewView(m_connection.HasReleaseControl() ? T_enViewType::viewStandard : T_enViewType::viewCameraFileSystem);

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
   case ID_PHOTO_MODE_CAMERA_FILE_SYSTEM: enViewType = T_enViewType::viewCameraFileSystem; break;
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
      m_connection.HasReleaseControl())
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

   if (!m_connection.HasReleaseControl())
      return;

   try
   {
      m_connection.GetRemoteReleaseControl()->SetReleaseSettings(m_releaseSettings);
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
      m_connection.HasReleaseControl())
   {
      size_t indexImageFormat = uSel;

      const ImageProperty& imageFormat = m_vecAllImageFormats[indexImageFormat];

      m_connection.GetRemoteReleaseControl()->SetImageProperty(imageFormat);
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

/// file open filter for timelapse images
LPCTSTR g_timelapseImageFilter =
_T("Image Files (*.jpg; *.tiff; *.png)\0*.jpg;*.tiff;*.png\0")
_T("All Files (*.*)\0*.*\0")
_T("");

/// file save filter for timelapse movie
LPCTSTR g_timelapseMovieFilter =
_T("Image Files (*.mp4; *.wmv)\0*.mp4;*.wmv\0")
_T("All Files (*.*)\0*.*\0")
_T("");

LRESULT MainFrame::OnExtraCreateTimelapseFromFiles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CMultiFileDialog imageFilesDlg(NULL, NULL, OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST, g_timelapseImageFilter);
   int ret = imageFilesDlg.DoModal(m_hWnd);

   if (ret != IDOK)
      return 0;

   // get list of image files
   std::vector<CString> imageFilenamesList;

   CString path;
   if (imageFilesDlg.GetFirstPathName(path))
   {
      do
      {
         imageFilenamesList.push_back(path);

      } while (imageFilesDlg.GetNextPathName(path));
   }

   TimelapseVideoOptionsDlg optionsDlg(m_settings.m_ffmpegCommandLineOptions);
   ret = optionsDlg.DoModal(m_hWnd);

   if (ret != IDOK)
      return 0;

   m_settings.m_ffmpegCommandLineOptions = optionsDlg.GetCommandLine();

   CString defaultOutputFilename = Path::Combine(Path::FolderName(path), _T("output.mp4"));

   CFileDialog saveMovieDlg(FALSE, _T("mp4"), defaultOutputFilename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, g_timelapseMovieFilter);
   ret = saveMovieDlg.DoModal(m_hWnd);
   if (ret != IDOK)
      return 0;

   CString outputFilename = saveMovieDlg.m_szFileName;

   // start ffmpeg
   FfmpegInterface ffmpeg(m_settings.m_ffmpegPath);

   if (!ffmpeg.IsInstalled())
   {
      AtlMessageBox(m_hWnd, _T("ffmpeg is not configured. Please configure ffmpeg in the Settings dialog"),
         IDR_MAINFRAME, MB_OK | MB_ICONERROR);

      return 0;
   }

   ffmpeg.Run(imageFilenamesList, optionsDlg.GetCommandLine(), outputFilename);

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
   {
      if (uCtrlID == ID_CAMERA_SETTINGS_IMAGE_FORMAT)
         return DefRibbonQueryItemImage(uCtrlID - 1, 0);

      return DefRibbonQueryItemImage(uCtrlID, uItem);
   }

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
   if (m_connection.IsConnected())
   {
      // app title + camera model name
      cszTitle.AppendFormat(_T(" - %s"),
         m_connection.GetSourceDevice()->ModelName().GetString());
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
   m_CmdBar.GetMenu().DeleteMenu(ID_PHOTO_MODE_PHOTOSTACK, MF_BYCOMMAND);

   // remove functions not implemented yet
   m_CmdBar.GetMenu().DeleteMenu(ID_VIEWFINDER_SHOW_OVERLAY_IMAGE, MF_BYCOMMAND);
   m_CmdBar.GetMenu().DeleteMenu(ID_VIEWFINDER_HISTOGRAM, MF_BYCOMMAND);

   // TODO remove when camera file system is fully implemented
   m_CmdBar.GetMenu().DeleteMenu(ID_PHOTO_MODE_CAMERA_FILE_SYSTEM, MF_BYCOMMAND);
}

/// sets button text for toolbar button
void SetToolbarButtonText(CToolBarCtrl& tb, int iId, LPCTSTR pszText)
{
   TBBUTTONINFO info = { 0 };
   info.cbSize = sizeof(info);
   info.dwMask = TBIF_STYLE;

   ATLVERIFY(TRUE == tb.GetButtonInfo(iId, &info));

   info.fsStyle |= BTNS_SHOWTEXT;

   info.dwMask |= TBIF_TEXT;
   info.pszText = const_cast<LPWSTR>(pszText);
   info.cchText = _tcslen(pszText);

   ATLVERIFY(TRUE == tb.SetButtonInfo(iId, &info));
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
      tb.HideButton(ID_PHOTO_MODE_PHOTOSTACK);

      // remove functions not implemented yet
      tb.HideButton(ID_VIEWFINDER_SHOW_OVERLAY_IMAGE);
      tb.HideButton(ID_VIEWFINDER_HISTOGRAM);

      // TODO remove when camera file system is fully implemented
      tb.HideButton(ID_PHOTO_MODE_CAMERA_FILE_SYSTEM);
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

std::shared_ptr<SourceDevice> MainFrame::GetSourceDevice()
{
   return m_connection.GetSourceDevice();
}

std::shared_ptr<RemoteReleaseControl> MainFrame::GetRemoteReleaseControl()
{
   return m_connection.GetRemoteReleaseControl();
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

   bool isConnected = !bLock && m_connection.IsConnected();
   bool hasReleaseControl = !bLock && m_connection.HasReleaseControl();

   EnablePhotoModes(isConnected, hasReleaseControl);
   EnableViewfinder(hasReleaseControl);
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
   SetRibbonContextAvail(ID_TAB_GROUP_CONTEXT_FILESYSTEM, UI_CONTEXTAVAILABILITY_NOTAVAILABLE);

   if (m_upView != nullptr)
   {
      if (!m_upView->CanClose())
         return; // view prevented close

      m_upView->DestroyView();
   }

   m_upView = ViewManager::CreateView(*this, enViewType);

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

   bool isConnected =
      enViewType != viewBlank &&
      enViewType != viewPreviousImages &&
      m_connection.IsConnected();

   bool hasReleaseControl =
      enViewType != viewBlank &&
      enViewType != viewPreviousImages &&
      m_connection.HasReleaseControl();

   EnablePhotoModes(isConnected, hasReleaseControl);
   EnableViewfinder(hasReleaseControl);

   bool isScriptingView = enViewType == viewScripting;
   EnableScriptingUI(isScriptingView);

   bool isFileSystemView = enViewType == viewCameraFileSystem;
   SetRibbonContextAvail(ID_TAB_GROUP_CONTEXT_FILESYSTEM,
      isFileSystemView ? UI_CONTEXTAVAILABILITY_ACTIVE : UI_CONTEXTAVAILABILITY_NOTAVAILABLE);
   EnableUI(ID_FILESYSTEM_DOWNLOAD, false);

   // force layout update so that new view gets resized properly
   UpdateLayout();

   m_enCurrentViewType = enViewType;
}

void MainFrame::ShowViewfinder(bool bShow)
{
   if (!bShow)
   {
      EnableViewfinderCommands(false);

      if (m_upViewFinderView == nullptr)
         return;

      m_splitter.SetSplitterPane(SPLIT_PANE_LEFT, m_hWndView, true);
      m_splitter.SetSinglePaneMode(SPLIT_PANE_LEFT);

      m_upViewFinderView->SetViewfinder(std::shared_ptr<Viewfinder>());

      m_upViewFinderView->DestroyWindow();

      m_upViewFinderView.reset();

      return;
   }

   std::shared_ptr<Viewfinder> spViewfinder = m_connection.StartViewfinder(m_hWnd);

   if (spViewfinder != nullptr)
   {
      EnableViewfinderCommands(true);

      if (m_upViewFinderView != nullptr)
         m_upViewFinderView->SetViewfinder(std::shared_ptr<Viewfinder>());

      m_upViewFinderView.reset(new ViewFinderView(*this, m_connection.GetRemoteReleaseControl()));
      m_upViewFinderView->Create(m_splitter);
      m_upViewFinderView->SetViewfinder(spViewfinder);

      m_splitter.SetSplitterPanes(m_hWndView, *m_upViewFinderView);
      m_splitter.SetSinglePaneMode(SPLIT_PANE_NONE);
      m_splitter.SetSplitterPosPct(30);

      SetRibbonContextAvail(ID_TAB_GROUP_CONTEXT_VIEWFINDER, UI_CONTEXTAVAILABILITY_ACTIVE);
   }
   else
   {
      EnableViewfinderCommands(true);
   }
}

void MainFrame::DisconnectCamera()
{
   ShowViewfinder(false);

   if (m_connection.HasReleaseControl())
   {
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl =
         m_connection.GetRemoteReleaseControl();

      spRemoteReleaseControl->RemoveStateEventHandler(m_iStateEventHandlerId);
      m_iStateEventHandlerId = -1;

      spRemoteReleaseControl->RemoveDownloadEventHandler(m_iDownloadEventHandlerId);
      m_iDownloadEventHandlerId = -1;

      CleanupImagePropertyManager();

      m_connection.Disconnect();
   }

   EnablePhotoModes(false, false);
   EnableViewfinder(false);
   EnableScriptingUI(false);
   EnableCameraUI(false);

   UpdateTitle();
}

void MainFrame::EnablePhotoModes(bool isConnected, bool hasReleaseControl)
{
   UIEnable(ID_PHOTO_MODE_NORMAL, hasReleaseControl);
   UIEnable(ID_PHOTO_MODE_HDR, hasReleaseControl);
   UIEnable(ID_PHOTO_MODE_PANO, hasReleaseControl);
   UIEnable(ID_PHOTO_MODE_HDR_PANO, hasReleaseControl);
   UIEnable(ID_PHOTO_MODE_TIMELAPSE, hasReleaseControl);
   UIEnable(ID_PHOTO_MODE_PHOTOSTACK, hasReleaseControl);
   UIEnable(ID_PHOTO_MODE_SCRIPTING, true); // scripting is always enabled
   UIEnable(ID_PHOTO_MODE_DEVICE_PROPERTIES, isConnected);
   UIEnable(ID_PHOTO_MODE_IMAGE_PROPERTIES, hasReleaseControl);

   bool enableCameraFileSystem =
      isConnected &&
      m_connection.GetSourceDevice()->GetDeviceCapability(SourceDevice::capCameraFileSystem);

   UIEnable(ID_PHOTO_MODE_CAMERA_FILE_SYSTEM, enableCameraFileSystem);
}

void MainFrame::EnableViewfinder(bool bEnable)
{
   if (bEnable)
   {
      bool bViewFinderAvail =
         m_connection.HasReleaseControl() &&
         m_connection.GetRemoteReleaseControl()->GetCapability(RemoteReleaseControl::capViewfinder);

      UIEnable(ID_VIEWFINDER_SHOW, bViewFinderAvail);
   }
   else
      UIEnable(ID_VIEWFINDER_SHOW, false);
}

void MainFrame::EnableViewfinderCommands(bool bEnable)
{
   UIEnable(ID_VIEWFINDER_AUTO_FOCUS, bEnable);
   UIEnable(ID_VIEWFINDER_AUTO_WB, bEnable);
   UIEnable(ID_VIEWFINDER_ZOOM_IN, bEnable);
   UIEnable(ID_VIEWFINDER_ZOOM_OUT, bEnable);
   UIEnable(ID_VIEWFINDER_LINES_MODE, bEnable);
   UIEnable(ID_VIEWFINDER_LINES_MODE_NONE, bEnable);
   UIEnable(ID_VIEWFINDER_LINES_MODE_RULEOFTHIRDS, bEnable);
   UIEnable(ID_VIEWFINDER_LINES_MODE_GOLDENRATIO, bEnable);
   UIEnable(ID_VIEWFINDER_OUTPUT_TYPE, bEnable);
   UIEnable(ID_VIEWFINDER_OUTPUT_TYPE_LCD, bEnable);
   UIEnable(ID_VIEWFINDER_OUTPUT_TYPE_VIDEO_OUT, bEnable);
   UIEnable(ID_VIEWFINDER_OUTPUT_TYPE_OFF, bEnable);
   UIEnable(ID_VIEWFINDER_SHOW_OVEREXPOSED, bEnable);
   UIEnable(ID_VIEWFINDER_SHOW_OVERLAY_IMAGE, bEnable);
   UIEnable(ID_VIEWFINDER_HISTOGRAM, bEnable);
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

   if (bEnable && m_connection.HasReleaseControl())
   {
      SetupImagePropertyManager();

      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl =
         m_connection.GetRemoteReleaseControl();

      // shooting mode change supported?
      bool bChangePossible = spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capChangeShootingMode);
      UIEnable(ID_CAMERA_SHOOTING_MODE, bChangePossible);

      // wait for changes in shooting mode property
      m_iImagePropertyHandlerId = spRemoteReleaseControl->AddPropertyEventHandler(
         std::bind(&MainFrame::OnUpdatedImageProperty, this, std::placeholders::_1, std::placeholders::_2));

      // update list of image formats
      unsigned int uiPropertyId = spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propImageFormat);
      spRemoteReleaseControl->EnumImagePropertyValues(uiPropertyId, m_vecAllImageFormats);

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
      if (m_iImagePropertyHandlerId != -1 &&
         m_connection.HasReleaseControl())
         m_connection.GetRemoteReleaseControl()->RemovePropertyEventHandler(m_iImagePropertyHandlerId);

      m_vecAllImageFormats.clear();
   }
}

void MainFrame::StoreWindowPosition()
{
   m_settings.m_windowPlacementMainFrame.Get(m_hWnd);
   m_settings.Store();
}

void MainFrame::RestoreWindowPosition()
{
   if (m_settings.m_windowPlacementMainFrame.showCmd != 0)
   {
      m_settings.m_windowPlacementMainFrame.Set(m_hWnd);

      ShowWindow(m_settings.m_windowPlacementMainFrame.showCmd);

      if (MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONULL) == NULL)
      {
         // center on desktop instead
         SetWindowPos(HWND_TOP, 0, 0, 800, 600, SWP_SHOWWINDOW);
         CenterWindow(GetDesktopWindow());
      }
   }
}

void MainFrame::OnUpdatedImageProperty(RemoteReleaseControl::T_enPropertyEvent enPropertyEvent, unsigned int uiValue)
{
   if (enPropertyEvent == RemoteReleaseControl::propEventPropertyChanged &&
      m_connection.HasReleaseControl() &&
      uiValue == m_connection.GetRemoteReleaseControl()->MapImagePropertyTypeToId(propShootingMode))
   {
      // shooting mode has changed; update some fields
      UpdateShootingModeDependentValues();
   }
}

void MainFrame::SetupImagePropertyManager()
{
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl = m_connection.GetRemoteReleaseControl();

   m_upImagePropertyValueManager.reset(new ImagePropertyValueManager(*spRemoteReleaseControl));

   m_cbCameraSettingsShootingMode.SetRemoteReleaseControl(spRemoteReleaseControl);
   m_cbCameraSettingsAv.SetRemoteReleaseControl(spRemoteReleaseControl);
   m_cbCameraSettingsTv.SetRemoteReleaseControl(spRemoteReleaseControl);
   m_cbCameraSettingsExposure.SetRemoteReleaseControl(spRemoteReleaseControl);
   m_cbCameraSettingsIso.SetRemoteReleaseControl(spRemoteReleaseControl);

   m_upImagePropertyValueManager->AddControl(m_cbCameraSettingsShootingMode);
   m_upImagePropertyValueManager->AddControl(m_cbCameraSettingsAv);
   m_upImagePropertyValueManager->AddControl(m_cbCameraSettingsTv);
   m_upImagePropertyValueManager->AddControl(m_cbCameraSettingsExposure);
   m_upImagePropertyValueManager->AddControl(m_cbCameraSettingsIso);

   m_upImagePropertyValueManager->UpdateControls();

   m_upImagePropertyValueManager->UpdateProperty(
      spRemoteReleaseControl->MapImagePropertyTypeToId(propShootingMode));
}

void MainFrame::CleanupImagePropertyManager()
{
   m_cbCameraSettingsShootingMode.SetRemoteReleaseControl(nullptr);
   m_cbCameraSettingsAv.SetRemoteReleaseControl(nullptr);
   m_cbCameraSettingsTv.SetRemoteReleaseControl(nullptr);
   m_cbCameraSettingsExposure.SetRemoteReleaseControl(nullptr);
   m_cbCameraSettingsIso.SetRemoteReleaseControl(nullptr);

   m_upImagePropertyValueManager.reset();
}

void MainFrame::UpdateShootingModeDependentValues()
{
   ShootingMode shootingMode(m_connection.GetRemoteReleaseControl());

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
      // don't disconnect, only disable all buttons, since we're in the
      // state event handler
      EnablePhotoModes(false, false);
      EnableViewfinder(false);
      EnableScriptingUI(false);
      EnableCameraUI(false);

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

   switch (enDownloadEvent)
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
