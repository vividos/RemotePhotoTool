//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file LuaScriptEditorView.cpp View for editing Lua scripts
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "LuaScriptEditorView.hpp"
#include "Lua.hpp"

/// file open filter for Lua scripting
LPCTSTR g_pszLuaScriptingFilter =
   _T("Lua Files (*.lua)\0*.lua\0")
   _T("All Files (*.*)\0*.*\0")
   _T("");

/// delay time in ms, after which a syntax check of the text in the view occurs
const UINT c_uiSyntaxCheckDelayTimeInMs = 1000;

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

   // set Lua keywords and add RemotePhotoTool specific classes
   SetKeyWords(0,
      "and break do else elseif end for function if in "
      "local nil not or repeat return then until while "
      "App Sys Constants RemoteReleaseControl SourceDevice "
      "ImageProperty DeviceProperty ShootingMode Viewfinder "
      "BulbReleaseControl");

   SetTabWidth(3);
   SetUseTabs(true);

   // only notify about text edits in SCEN_CHANGE messages
   SetModEventMask(SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT);
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

LRESULT LuaScriptEditorView::OnChangedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
   bHandled = false;

   RestartSyntaxCheckTimer();

   return 0;
}

LRESULT LuaScriptEditorView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   KillTimer(IDT_TIMER_SYNTAX_CHECK);

   return 0;
}

LRESULT LuaScriptEditorView::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if (wParam == IDT_TIMER_SYNTAX_CHECK)
   {
      KillTimer(IDT_TIMER_SYNTAX_CHECK);

      CheckSyntax();
   }

   return 0;
}

void LuaScriptEditorView::RestartSyntaxCheckTimer()
{
   KillTimer(IDT_TIMER_SYNTAX_CHECK);
   SetTimer(IDT_TIMER_SYNTAX_CHECK, c_uiSyntaxCheckDelayTimeInMs);
}

void LuaScriptEditorView::CheckSyntax()
{
   CStringA text;
   GetText(text);

   int indicatorNumber = INDIC_CONTAINER;

   AnnotationClearAll();

   Lua::State state;
   std::vector<CString> errorMessages;
   if (state.CheckSyntax(CString(text), errorMessages))
   {
      IndicSetStyle(indicatorNumber, INDIC_HIDDEN);
      AnnotationSetVisible(ANNOTATION_HIDDEN);
      return;
   }

   IndicSetStyle(indicatorNumber, INDIC_SQUIGGLE);
   IndicSetFore(indicatorNumber, RGB(255, 0, 0)); // red

   SetIndicatorCurrent(indicatorNumber);

   for (size_t index = 0, maxIndex = errorMessages.size(); index < maxIndex; index++)
   {
      CString errorMessage = errorMessages[0];

      int pos = errorMessage.Find(_T("]:"));
      int pos2 = errorMessage.Find(_T(':'), pos + 2);

      int lineNumber = _ttoi(errorMessage.Mid(pos + 2, pos2 - (pos + 2)));
      CString error = errorMessage.Mid(pos2 + 1).Trim();

      SetIndicatorValue(index);

      int textStart = static_cast<int>(PositionFromLine(lineNumber - 1));
      int textEnd = GetLineEndPosition(lineNumber - 1);

      IndicatorFillRange(textStart, textEnd - textStart);

      AnnotationSetText(lineNumber - 1, CStringA(error).GetString());
      AnnotationSetVisible(ANNOTATION_BOXED);
   }
}
