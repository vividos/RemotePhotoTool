//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file RemotePhotoTool\AboutDlg.cpp About dialog for remote photo tool
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "AboutDlg.hpp"
#include "Instance.hpp"
#include "../version.h"
#include "Lua.hpp"

void AboutDlg::SetVersionText()
{
   DlgResize_Init(true, true);

   CWindow scVersion = GetDlgItem(IDC_STATIC_ABOUT_VERSION);

   CString cszVersion;
   scVersion.GetWindowText(cszVersion);

   CString cszAppVersion;
   cszAppVersion.Format(_T(""));

   cszVersion.Replace(_T("%VERSION%"), CString(VERSIONINFO_FILEVERSION_DISPLAYSTRING));
   cszVersion.Replace(_T("%COPYRIGHT%"), CString(VERSIONINFO_COPYRIGHT));

   scVersion.SetWindowText(cszVersion);
}

void AboutDlg::SetAboutInfo()
{
   CWindow scInfo = GetDlgItem(IDC_STATIC_ABOUT_INFO);

   CString cszInfo;
   scInfo.GetWindowText(cszInfo);

   cszInfo += Instance::Get().Version();

   cszInfo += _T("Lua version: ");
   cszInfo += Lua::State::GetVersion();

   scInfo.SetWindowText(cszInfo);
}

LRESULT AboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CenterWindow(GetParent());

   SetVersionText();
   SetAboutInfo();

   return TRUE;
}

LRESULT AboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   EndDialog(wID);
   return 0;
}
