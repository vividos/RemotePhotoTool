//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ScriptingPhotoModeView.cpp Scripting photo mode view
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "ScriptingPhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"
#include "Lua.hpp"
#include "Path.hpp"
#include "Filesystem.hpp"
#include "Process.hpp"

/// file open filter for Lua scripting
LPCTSTR g_pszLuaScriptingFilter =
_T("Lua Files (*.lua)\0*.lua\0")
_T("All Files (*.*)\0*.*\0")
_T("");


ScriptingPhotoModeView::ScriptingPhotoModeView(IPhotoModeViewHost& host) throw()
:CSplitterWindowImpl<ScriptingPhotoModeView>(false), // horizontal
m_host(host)
{
   m_processor.SetOutputDebugStringHandler(
      std::bind(&ScriptingPhotoModeView::OnOutputDebugString, this, std::placeholders::_1));
}

HWND ScriptingPhotoModeView::CreateView(HWND hWndParent)
{
   HWND hWndClient = Create(hWndParent, CWindow::rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   m_view.Create(hWndClient, CWindow::rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);

   m_view.SetupSourceEditor();
   m_view.SetReadOnly(true);
   m_view.HideCaret();

   SetupOutputPane();

   SetSplitterPanes(m_view, m_pane);
   SetSinglePaneMode(SPLIT_PANE_NONE);

   SetSplitterPosPct(75);

   return hWndClient;
}

void ScriptingPhotoModeView::SetupOutputPane()
{
   m_pane.SetPaneContainerExtendedStyle(PANECNT_FLATBORDER | PANECNT_NOCLOSEBUTTON);
   m_pane.Create(m_hWnd, _T("Output"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   m_ecOutputWindow.Create(CScintillaWindow::GetWndClassName(), m_pane, CWindow::rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);

   m_pane.SetClient(m_ecOutputWindow);

   m_ecOutputWindow.StyleSetFont(STYLE_DEFAULT, "Courier New");
   m_ecOutputWindow.StyleSetSize(STYLE_DEFAULT, 11);
   m_ecOutputWindow.SetLexer(SCLEX_LUA);

   m_ecOutputWindow.SetText("Ready.\n");
}

void ScriptingPhotoModeView::OnOutputDebugString(const CString& cszText)
{
   CStringA cszaText(cszText);
   m_ecOutputWindow.AppendText(cszaText.GetString(), cszaText.GetLength());

   m_ecOutputWindow.ScrollToLine(m_ecOutputWindow.GetLineCount());
}

void ScriptingPhotoModeView::OpenScript()
{
   CFileDialog dlg(TRUE, NULL, _T(""), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, g_pszLuaScriptingFilter);
   int iRet = dlg.DoModal(m_hWnd);

   if (iRet != IDOK)
      return;

   m_view.SetReadOnly(false);

   bool bRet = m_view.Load(dlg.m_ofn.lpstrFile) != FALSE;
   if (!bRet)
      return;

   m_view.Init(dlg.m_ofn.lpstrFile);
   m_view.SetReadOnly(true);
   m_view.HideCaret();

   m_host.EnableUI(ID_SCRIPTING_RELOAD, true);
   m_host.EnableUI(ID_SCRIPTING_EDIT, true);
}

void ScriptingPhotoModeView::EditScript(const CString& cszFilename)
{
   if (cszFilename.IsEmpty())
      return;

   CString cszCommandLine;
   cszCommandLine.Format(_T("\"%s\" \"%s\""),
      Path::Combine(Path(App_GetFilename()).DirectoryName(), _T("RemoteScriptingEditor")),
      cszFilename);

   Process process;
   process.WorkingDirectory(Path(cszFilename).DirectoryName());
   bool bRet = process.Create(cszCommandLine);

   if (!bRet)
      AtlMessageBox(m_hWnd, _T("Couldn't start Remote Scripting Editor."), IDR_MAINFRAME, MB_OK);

   return;
}

LRESULT ScriptingPhotoModeView::OnScriptingOpenScript(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   OpenScript();

   return 0;
}

LRESULT ScriptingPhotoModeView::OnScriptingReload(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CString cszFilename = m_view.GetFilePath();
   m_view.Load(cszFilename);

   return 0;
}

LRESULT ScriptingPhotoModeView::OnScriptingRun(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_host.EnableUI(ID_SCRIPTING_RUN, false);
   m_host.EnableUI(ID_SCRIPTING_STOP, true);

   {
      CString cszText;
      cszText.Format(_T("Start executing file %s...\n\n"),
         m_view.GetFilePath().GetString());

      m_ecOutputWindow.SetText(CStringA(cszText));
   }

   m_processor.Stop();

   try
   {
      m_processor.LoadScript(m_view.GetFilePath());

      m_processor.Run();
   }
   catch (const Lua::Exception& ex)
   {
      CString cszText;
      cszText.Format(_T("%s(%u): %s"),
         ex.LuaSourceFile().GetString(),
         ex.LuaLineNumber(),
         ex.LuaErrorMessage().GetString());

      OnOutputDebugString(cszText);

      m_host.EnableUI(ID_SCRIPTING_RUN, TRUE);
      m_host.EnableUI(ID_SCRIPTING_STOP, FALSE);
   }

   return 0;
}

LRESULT ScriptingPhotoModeView::OnScriptingStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_host.EnableUI(ID_SCRIPTING_RUN, TRUE);
   m_host.EnableUI(ID_SCRIPTING_STOP, FALSE);

   m_processor.Stop();

   return 0;
}

LRESULT ScriptingPhotoModeView::OnScriptingEditScript(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CString cszFilename = m_view.GetFilePath();

   EditScript(cszFilename);

   m_view.Load(cszFilename);

   return 0;
}
