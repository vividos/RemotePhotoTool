//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file SettingsDlg.cpp Settings dialog
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "SettingsDlg.hpp"
#include "HuginInterface.hpp"
#include "PhotomatixInterface.hpp"
#include "Filesystem.hpp"
#include <ulib/Path.hpp>

SettingsDlg::SettingsDlg(AppSettings& settings)
:m_settings(settings)
{
}

LRESULT SettingsDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   return TRUE;
}

LRESULT SettingsDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (wID == IDOK)
   {
      DoDataExchange(DDX_SAVE);
   }

   EndDialog(wID);
   return 0;
}

LRESULT SettingsDlg::OnBtnProjectsFolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CFolderDialog dlg(m_hWnd, _T("Select project folder..."), BIF_RETURNONLYFSDIRS | BIF_USENEWUI);
   dlg.SetInitialFolder(m_settings.m_cszProjectsFolder);

   if (IDOK == dlg.DoModal(m_hWnd))
   {
      m_settings.m_cszProjectsFolder = dlg.GetFolderPath();
      DoDataExchange(DDX_LOAD, IDC_EDIT_SETTING_PROJECTS_FOLDER);
   }

   return 0;
}

LRESULT SettingsDlg::OnCheckLogFolder(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_SAVE, wID);

   if (m_settings.m_bLogging && m_settings.m_cszLogfilePath.IsEmpty())
      SelectLogFolder();

   return 0;
}

LRESULT SettingsDlg::OnBtnLogFolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   SelectLogFolder();

   return 0;
}

LRESULT SettingsDlg::OnBtnDetectPhotomatix(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   PhotomatixInterface hi(m_settings.m_cszPhotomatixPath);
   if (hi.IsInstalled())
   {
      m_settings.m_cszPhotomatixPath = hi.InstallPath();
      DoDataExchange(DDX_LOAD, IDC_EDIT_SETTINGS_PATH_PHOTOMATIX);
   }

   return 0;
}

LRESULT SettingsDlg::OnBtnDetectHugin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   HuginInterface hi(m_settings.m_cszHuginPath);
   if (hi.IsInstalled())
   {
      m_settings.m_cszHuginPath = hi.InstallPath();
      DoDataExchange(DDX_LOAD, IDC_EDIT_SETTINGS_PATH_HUGIN);
   }

   return 0;
}

LRESULT SettingsDlg::OnBtnFfmpegFolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CFolderDialog dlg(m_hWnd, _T("Select ffmpeg bin folder..."), BIF_RETURNONLYFSDIRS | BIF_USENEWUI);
   dlg.SetInitialFolder(m_settings.m_ffmpegPath);

   if (IDOK == dlg.DoModal(m_hWnd))
   {
      m_settings.m_ffmpegPath = dlg.GetFolderPath();
      DoDataExchange(DDX_LOAD, IDC_EDIT_SETTINGS_PATH_FFMPEG);
   }

   return 0;
}

void SettingsDlg::SelectLogFolder()
{
   if (m_settings.m_cszLogfilePath.IsEmpty())
   {
      // get non-roaming app data folder
      CString cszFolder;
      ATLVERIFY(TRUE == SHGetSpecialFolderPath(m_hWnd, cszFolder.GetBuffer(MAX_PATH), CSIDL_LOCAL_APPDATA, TRUE));
      cszFolder.ReleaseBuffer();

      cszFolder += _T("\\RemotePhotoTool\\");

      if (!Path(cszFolder).FolderExists())
         CreateDirectory(cszFolder, NULL);

      m_settings.m_cszLogfilePath = cszFolder;

      DoDataExchange(DDX_LOAD, IDC_EDIT_SETTING_LOG_FOLDER);
   }

   CFolderDialog dlg(m_hWnd, _T("Select log folder..."), BIF_RETURNONLYFSDIRS | BIF_USENEWUI);
   dlg.SetInitialFolder(m_settings.m_cszLogfilePath);

   if (IDOK == dlg.DoModal(m_hWnd))
   {
      m_settings.m_cszLogfilePath = dlg.GetFolderPath();
      DoDataExchange(DDX_LOAD, IDC_EDIT_SETTING_LOG_FOLDER);
   }
}
