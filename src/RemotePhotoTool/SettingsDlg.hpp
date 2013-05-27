//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file SettingsDlg.hpp Settings dialog
//
#pragma once

// includes
#include "AppSettings.hpp"

/// settings dialog
class SettingsDlg :
   public CDialogImpl<SettingsDlg>,
   public CWinDataExchange<SettingsDlg>
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
   END_MSG_MAP()

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnBtnProjectsFolder(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnCheckLogFolder(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnBtnLogFolder(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnBtnDetectPhotomatix(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnBtnDetectHugin(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   /// selects new log folder
   void SelectLogFolder();

private:
   /// app settings
   AppSettings& m_settings;
};
