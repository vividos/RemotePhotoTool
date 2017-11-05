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
#include <ulib/Path.hpp>
#include "Filesystem.hpp"
#include <ulib/win32/Process.hpp>

/// file open filter for Lua scripting
LPCTSTR g_pszLuaScriptingFilter =
_T("Lua Files (*.lua)\0*.lua\0")
_T("All Files (*.*)\0*.*\0")
_T("");


ScriptingPhotoModeView::ScriptingPhotoModeView(IPhotoModeViewHost& host)
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

   // set up scheduler state handling
   OnExecutionStateChanged(LuaScheduler::stateIdle);

   m_processor.GetScheduler().SetExecutionStateChangedHandler(
      std::bind(&ScriptingPhotoModeView::OnExecutionStateChanged, this, std::placeholders::_1));

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

   m_view.SetReadOnly(true);
   m_view.HideCaret();

   if (!bRet)
      return;

   m_view.Init(dlg.m_ofn.lpstrFile);

   m_host.EnableUI(ID_SCRIPTING_RELOAD, true);
   m_host.EnableUI(ID_SCRIPTING_EDIT, true);
}

void ScriptingPhotoModeView::EditScript(const CString& cszFilename)
{
   if (cszFilename.IsEmpty())
      return;

   CString cszEditorFilename =
      Path::Combine(Path(App_GetFilename()).FolderName(), _T("RemoteScriptingEditor.exe"));

   CString cszCommandLine;
   cszCommandLine.Format(_T("\"%s\" \"%s\""),
      cszEditorFilename.GetString(),
      cszFilename.GetString());

   Win32::Process process;
   process.WorkingDirectory(Path(cszFilename).FolderName());
   bool bRet = process.Create(cszCommandLine);

   if (bRet)
      WaitForSingleObject(process.ProcessHandle(), INFINITE);
   else
      AtlMessageBox(m_hWnd, _T("Couldn't start Remote Scripting Editor."), IDR_MAINFRAME, MB_OK);

   return;
}

LRESULT ScriptingPhotoModeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   m_processor.GetScheduler().SetExecutionStateChangedHandler(
      LuaScheduler::T_fnOnExecutionStateChanged());

   m_processor.Stop();

   m_host.SetStatusText(_T(""), 1);

   bHandled = false;
   return 1;
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

   m_view.SetReadOnly(true);
   m_view.HideCaret();

   return 0;
}

LRESULT ScriptingPhotoModeView::OnScriptingRun(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CString cszFilename = m_view.GetFilePath();
   if (cszFilename.IsEmpty())
      return 0;

   {
      CString cszText;
      cszText.Format(_T("Start executing file %s...\n\n"),
         cszFilename.GetString());

      m_ecOutputWindow.SetText(CStringA(cszText));
   }

   m_processor.Stop();

   try
   {
      m_processor.LoadScript(cszFilename);

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
   }

   return 0;
}

LRESULT ScriptingPhotoModeView::OnScriptingStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_processor.Stop();

   return 0;
}

LRESULT ScriptingPhotoModeView::OnScriptingEditScript(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CString cszFilename = m_view.GetFilePath();

   EditScript(cszFilename);

   m_view.Load(cszFilename);

   m_view.SetReadOnly(true);
   m_view.HideCaret();

   return 0;
}

void ScriptingPhotoModeView::OnExecutionStateChanged(LuaScheduler::T_enExecutionState enExecutionState)
{
   PostMessage(WM_EXECUTION_STATE_CHANGED, static_cast<WPARAM>(enExecutionState));
}

LRESULT ScriptingPhotoModeView::OnMessageExecutionStateChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   LuaScheduler::T_enExecutionState enExecutionState =
      static_cast<LuaScheduler::T_enExecutionState>(wParam);

   LPCTSTR pszText = nullptr;

   switch (enExecutionState)
   {
   case LuaScheduler::stateIdle:
      pszText = _T("Idle.");
      m_host.EnableUI(ID_SCRIPTING_RUN, true);
      m_host.EnableUI(ID_SCRIPTING_STOP, false);
      m_host.LockActionMode(false);
      break;

   case LuaScheduler::stateRunning:
      pszText = _T("Running...");
      m_host.EnableUI(ID_SCRIPTING_RUN, false);
      m_host.EnableUI(ID_SCRIPTING_STOP, true);
      m_host.LockActionMode(true);
      break;

   case LuaScheduler::stateYield:
      pszText = _T("Waiting...");
      break;

   case LuaScheduler::stateDebug:
      break;

   case LuaScheduler::stateError:
      pszText = _T("Error.");
      m_host.EnableUI(ID_SCRIPTING_RUN, true);
      m_host.EnableUI(ID_SCRIPTING_STOP, false);
      m_host.LockActionMode(false);
      break;

   default:
      ATLASSERT(false); // unknown state
      break;
   }

   if (pszText != nullptr)
      m_host.SetStatusText(pszText, 1);

   return 0;
}
