//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemoteScriptingEditor\AboutDlg.hpp About dialog for remote scripting editor
//
#pragma once

/// about dialog
class AboutDlg : public CDialogImpl<AboutDlg>
{
public:
   /// dialog id
   enum { IDD = IDD_ABOUTBOX };

private:
   // message map
   BEGIN_MSG_MAP(AboutDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
      COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
   END_MSG_MAP()

private:
   /// sets version text
   void SetVersionText();

   /// sets about info
   void SetAboutInfo();

   /// called at start of dialog
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called to close dialog
   LRESULT OnCloseCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};
