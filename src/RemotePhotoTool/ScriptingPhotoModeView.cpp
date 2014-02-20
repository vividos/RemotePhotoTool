//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ScriptingPhotoModeView.cpp View for scripting camera controls
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "ScriptingPhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"
#include <SciLexer.h>

/// file open filter for Lua scripting
LPCTSTR g_pszLuaScriptingFilter =
   _T("Lua Files (*.lua)\0*.lua\0")
   _T("All Files (*.*)\0*.*\0")
   _T("");

/// auto-registering of scintilla window class
CScintillaAutoRegister s_ScintillaAutoRegister;

ScriptingPhotoModeView::ScriptingPhotoModeView(IPhotoModeViewHost& host) throw()
:m_host(host)
{
}

HWND ScriptingPhotoModeView::CreateView(HWND hWndParent)
{
   CRect rc;
   CWindow(hWndParent).GetClientRect(rc);

   HWND hWndSplitter = BaseClass::Create(hWndParent, rc, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   m_sourceEditor.Create(CScintillaWindow::GetWndClassName(), hWndSplitter, rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);

   SetupOutputPane();

   SetSplitterPanes(m_sourceEditor, m_pane);
   SetSinglePaneMode(SPLIT_PANE_NONE);

   SetSplitterPosPct(75);

   SetupSourceEditor();

   return hWndSplitter;
}

BOOL ScriptingPhotoModeView::PreTranslateMessage(MSG* /*pMsg*/)
{
   return FALSE;
}

void ScriptingPhotoModeView::SetupOutputPane()
{
   m_pane.SetPaneContainerExtendedStyle(PANECNT_FLATBORDER | PANECNT_NOCLOSEBUTTON);
   m_pane.Create(m_hWnd, _T("Output"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   m_ecOutputWindow.Create(m_pane, rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | ES_READONLY | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOVSCROLL, 0);

   CLogFont logfont(::AtlGetDefaultGuiFont());
   logfont.SetHeight(11);
   ATLVERIFY(0 == _tcscpy_s(logfont.lfFaceName, _T("Courier New")));

   m_ecOutputWindow.SetFont(logfont.CreateFontIndirect());

   m_ecOutputWindow.SetWindowText(_T("Ready."));

   m_pane.SetClient(m_ecOutputWindow);
}

void ScriptingPhotoModeView::SetupSourceEditor()
{
   m_sourceEditor.StyleSetFont(STYLE_DEFAULT, "Courier New");
   m_sourceEditor.StyleSetSize(STYLE_DEFAULT, 11);
   m_sourceEditor.SetEdgeColumn(100);
   m_sourceEditor.SetEdgeMode(EDGE_LINE);
   m_sourceEditor.SetMarginWidthN(0, 50); // margin 0 displays line numbers, but has default width 0

   m_sourceEditor.StyleSetFore(0,  RGB(0x80, 0x80, 0x80));
   m_sourceEditor.StyleSetFore(1,  RGB(0x00, 0x7f, 0x00));
   m_sourceEditor.StyleSetFore(2,  RGB(0x00, 0x7f, 0x00));
   m_sourceEditor.StyleSetFore(3,  RGB(0x7f, 0x7f, 0x7f));
   m_sourceEditor.StyleSetFore(4,  RGB(0x00, 0x7f, 0x7f));
   m_sourceEditor.StyleSetFore(5,  RGB(0x00, 0x00, 0x7f));
   m_sourceEditor.StyleSetFore(6,  RGB(0x7f, 0x00, 0x7f));
   m_sourceEditor.StyleSetFore(7,  RGB(0x7f, 0x00, 0x7f));
   m_sourceEditor.StyleSetFore(8,  RGB(0x00, 0x7f, 0x7f));
   m_sourceEditor.StyleSetFore(9,  RGB(0x7f, 0x7f, 0x7f));
   m_sourceEditor.StyleSetFore(10, RGB(0x00, 0x00, 0x00));
   m_sourceEditor.StyleSetFore(11, RGB(0x00, 0x00, 0x00));
   m_sourceEditor.StyleSetBold(5,  true);
   m_sourceEditor.StyleSetBold(10, true);

   m_sourceEditor.SetLexer(SCLEX_LUA);

   // set Lua keywords
   m_sourceEditor.SetKeyWords(0,
      "and break do else elseif end for function if in " // official keywords
      "local nil not or repeat return then until while "
      ""); // TODO add RemotePhotoTool specific classes

   m_sourceEditor.SetTabWidth(3);
}

bool ScriptingPhotoModeView::LoadFile(LPCTSTR lpstrFilePath)
{
   return m_sourceEditor.Load(lpstrFilePath) != FALSE;
}

bool ScriptingPhotoModeView::SaveFile(LPCTSTR lpstrFilePath)
{
   return m_sourceEditor.Save(lpstrFilePath) != FALSE;
}

void ScriptingPhotoModeView::Init(LPCTSTR lpstrFilePath, LPCTSTR lpstrFileTitle)
{
   m_cszFilePath = lpstrFilePath;
   m_cszFileTitle = lpstrFileTitle;
}

void ScriptingPhotoModeView::SetContent(const CString& cszText)
{
   m_sourceEditor.SetText(CStringA(cszText));
}

bool ScriptingPhotoModeView::IsModified() const
{
   return m_sourceEditor.GetModify();
}

bool ScriptingPhotoModeView::DoFileSaveAs()
{
   CFileDialog dlg(FALSE, NULL, m_cszFilePath,
      OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      g_pszLuaScriptingFilter, m_hWnd);

   int iRet = dlg.DoModal();
   if (iRet != IDOK)
      return false;

   bool bRet = SaveFile(dlg.m_ofn.lpstrFile);
   if (bRet)
      Init(dlg.m_ofn.lpstrFile, dlg.m_ofn.lpstrFileTitle);
   else
      MessageBox(_T("Error writing file!\n"));

   return bRet;
}

bool ScriptingPhotoModeView::QueryClose()
{
   if (!m_sourceEditor.GetModify())
      return true;

   CWindow wndMain(GetParent());

   CString cszText;
   cszText.Format(_T("Save changes to %s ?"), m_cszFileTitle);

   int iRet = wndMain.MessageBox(cszText, _T("RemotePhotoTool"), MB_YESNOCANCEL | MB_ICONEXCLAMATION);

   if (iRet == IDCANCEL)
      return false;
   else if (iRet == IDYES)
   {
      if (!DoFileSaveAs())
         return false;
   }

   return true;
}

bool ScriptingPhotoModeView::CanPaste() const
{
   return m_sourceEditor.CanPaste();
}

bool ScriptingPhotoModeView::CanUndo() const
{
   return m_sourceEditor.CanUndo();
}

bool ScriptingPhotoModeView::CanRedo() const
{
   return m_sourceEditor.CanRedo();
}

bool ScriptingPhotoModeView::IsTextSelected() const
{
   return m_sourceEditor.GetSelectionStart() < m_sourceEditor.GetSelectionEnd();
}
