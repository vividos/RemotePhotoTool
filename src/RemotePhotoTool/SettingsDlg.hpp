//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file SettingsDlg.hpp Settings dialog
//
#pragma once

// includes
#include "AppSettings.hpp"

/// settings dialog
class SettingsDlg :
   public CDialogImpl<SettingsDlg>,
   public CWinDataExchange<SettingsDlg>,
   public boost::noncopyable
{
public:
   /// ctor
   SettingsDlg(AppSettings& settings);

   /// dialog id
   enum { IDD = IDD_SETTINGS };

private:
   BEGIN_DDX_MAP(SettingsDlg)
      DDX_TEXT(IDC_EDIT_SETTING_PROJECTS_FOLDER, m_settings.m_cszProjectsFolder)
      DDX_CHECK(IDC_CHECK_SETTING_CURRENT_DATE_FOLDER, m_settings.m_bCurrentDateSubfolder)
      DDX_CHECK(IDC_CHECK_SETTING_IMAGE_TYPE_FOLDER, m_settings.m_bImageTypeSubfolder)
      DDX_CHECK(IDC_CHECK_SETTING_LOG_FOLDER, m_settings.m_bLogging)
      DDX_TEXT(IDC_EDIT_SETTING_LOG_FOLDER, m_settings.m_cszLogfilePath)
      DDX_TEXT(IDC_EDIT_SETTINGS_PATH_PHOTOMATIX, m_settings.m_cszPhotomatixPath)
      DDX_TEXT(IDC_EDIT_SETTINGS_PATH_HUGIN, m_settings.m_cszHuginPath)
      DDX_TEXT(IDC_EDIT_SETTINGS_PATH_FFMPEG, m_settings.m_ffmpegPath)
   END_DDX_MAP()

   BEGIN_MSG_MAP(SettingsDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
      COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
      COMMAND_ID_HANDLER(IDC_BUTTON_SETTING_PROJECTS_FOLDER, OnBtnProjectsFolder)
      COMMAND_ID_HANDLER(IDC_CHECK_SETTING_LOG_FOLDER, OnCheckLogFolder)
      COMMAND_ID_HANDLER(IDC_BUTTON_SETTINGS_LOG_FOLDER, OnBtnLogFolder)
      COMMAND_ID_HANDLER(IDC_BUTTON_SETTINGS_DETECT_PHOTOMATIX, OnBtnDetectPhotomatix)
      COMMAND_ID_HANDLER(IDC_BUTTON_SETTINGS_DETECT_HUGIN, OnBtnDetectHugin)
      COMMAND_ID_HANDLER(IDC_BUTTON_SETTINGS_FFMPEG_FOLDER, OnBtnFfmpegFolder)
   END_MSG_MAP()

   /// called when dialog is being shown
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called when OK or Cancel button is pressed
   LRESULT OnCloseCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when button to select Projects folder is pressed
   LRESULT OnBtnProjectsFolder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when checkbox "Logging" is changed
   LRESULT OnCheckLogFolder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when button to select Log folder is pressed
   LRESULT OnBtnLogFolder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when button "Detect" for Photomatix is pressed
   LRESULT OnBtnDetectPhotomatix(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when button "Detect" for Hugin is pressed
   LRESULT OnBtnDetectHugin(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when button "..." for ffmpeg is pressed
   LRESULT OnBtnFfmpegFolder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// selects new log folder
   void SelectLogFolder();

private:
   /// app settings
   AppSettings& m_settings;
};
