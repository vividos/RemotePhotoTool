//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file LuaScriptEditorView.cpp View for editing Lua scripts
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "LuaScriptEditorView.hpp"

/// file open filter for Lua scripting
LPCTSTR g_pszLuaScriptingFilter =
   _T("Lua Files (*.lua)\0*.lua\0")
   _T("All Files (*.*)\0*.*\0")
   _T("");

BOOL LuaScriptEditorView::PreTranslateMessage(MSG* pMsg)
{
   if (FindReplaceClass::PreTranslateMessage(pMsg))
      return TRUE;

   return FALSE;
}

void LuaScriptEditorView::SetupSourceEditor()
{
   StyleSetFont(STYLE_DEFAULT, "Courier New");
   StyleSetSize(STYLE_DEFAULT, 11);
   SetEdgeColumn(100);
   SetEdgeMode(EDGE_LINE);
   SetMarginWidthN(0, 50); // margin 0 displays line numbers, but has default width 0

   StyleSetFore(0,  RGB(0x80, 0x80, 0x80));
   StyleSetFore(1,  RGB(0x00, 0x7f, 0x00));
   StyleSetFore(2,  RGB(0x00, 0x7f, 0x00));
   StyleSetFore(3,  RGB(0x7f, 0x7f, 0x7f));
   StyleSetFore(4,  RGB(0x00, 0x7f, 0x7f));
   StyleSetFore(5,  RGB(0x00, 0x00, 0x7f));
   StyleSetFore(6,  RGB(0x7f, 0x00, 0x7f));
   StyleSetFore(7,  RGB(0x7f, 0x00, 0x7f));
   StyleSetFore(8,  RGB(0x00, 0x7f, 0x7f));
   StyleSetFore(9,  RGB(0x7f, 0x7f, 0x7f));
   StyleSetFore(10, RGB(0x00, 0x00, 0x00));
   StyleSetFore(11, RGB(0x00, 0x00, 0x00));
   StyleSetBold(5,  true);
   StyleSetBold(10, true);

   SetLexer(SCLEX_LUA);

   // set Lua keywords
   SetKeyWords(0,
      "and break do else elseif end for function if in "
      "local nil not or repeat return then until while");

   // add RemotePhotoTool specific classes
   SetKeyWords(1,
      "App Sys Constants RemoteReleaseControl Viewfinder");

   SetTabWidth(3);
}

void LuaScriptEditorView::Init(LPCTSTR lpstrFilePath, LPCTSTR lpstrFileTitle)
{
   m_cszFilePath = lpstrFilePath;
   m_cszFileTitle = lpstrFileTitle;
}

bool LuaScriptEditorView::DoFileSaveAs()
{
   CFileDialog dlg(FALSE, NULL, m_cszFilePath,
      OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      g_pszLuaScriptingFilter, m_hWnd);

   int iRet = dlg.DoModal(m_hWnd);
   if (iRet != IDOK)
      return false;

   bool bRet = Save(dlg.m_ofn.lpstrFile) != FALSE;
   if (bRet)
      Init(dlg.m_ofn.lpstrFile, dlg.m_ofn.lpstrFileTitle);
   else
      AtlMessageBox(m_hWnd, _T("Error writing file!\n"), IDR_MAINFRAME, MB_OK | MB_ICONERROR);

   return bRet;
}

bool LuaScriptEditorView::QueryClose()
{
   if (!GetModify())
      return true;

   CWindow wndMain(GetParent());

   CString cszText;
   cszText.Format(_T("Save changes to %s ?"), m_cszFileTitle.GetString());

   int iRet = AtlMessageBox(wndMain, cszText.GetString(), IDR_MAINFRAME, MB_YESNOCANCEL | MB_ICONQUESTION);

   if (iRet == IDCANCEL)
      return false;
   else if (iRet == IDYES)
   {
      if (!DoFileSaveAs())
         return false;
   }

   return true;
}
