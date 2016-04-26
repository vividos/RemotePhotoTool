//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file RemoteScriptingEditor\MainFrame.cpp Main application frame
//

// includes
#include "stdafx.h"
#include "res\Ribbon.h"
#include "resource.h"
#include "AboutDlg.hpp"
#include "LuaScriptEditorView.hpp"
#include "MainFrame.hpp"
#include "Lua.hpp"
#include "Path.hpp"
#include "Instance.hpp"

extern LPCTSTR g_pszLuaScriptingFilter;

/// specifies the position of the File menu in IDR_TOOLBAR_SCRIPTING
#define FILE_MENU_POSITION    0

/// specifies the position of the Recent Files menu item in the File menu in IDR_TOOLBAR_SCRIPTING
#define RECENT_MENU_POSITION  5

/// settings registry key (subkey "Ribbon" is used for menu band)
LPCTSTR c_pszSettingsRegkey = _T("Software\\RemoteScriptingEditor");

MainFrame::MainFrame() throw()
:m_bScriptingFileModified(false),
 m_settings(c_pszSettingsRegkey)
{
   m_settings.Load();

   m_processor.SetOutputDebugStringHandler(
      std::bind(&MainFrame::OnOutputDebugString, this, std::placeholders::_1));
}

BOOL MainFrame::PreTranslateMessage(MSG* pMsg)
{
   if (CRibbonFrameWindowImpl<MainFrame>::PreTranslateMessage(pMsg))
      return TRUE;

   return m_view.PreTranslateMessage(pMsg);
}

BOOL MainFrame::OnIdle()
{
   Instance::OnIdle();

   UIUpdateToolBar();
   UIUpdateAll();
   return FALSE;
}

LRESULT MainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   SetupCmdBar();
   SetupRibbonBar();
   SetupToolbar();
   SetupMRUList();
   SetupStatusBar();
   SetupView();

   // register object for message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->AddMessageFilter(this);
   pLoop->AddIdleHandler(this);

   {
      bool bRibbonUI = RunTimeHelper::IsRibbonUIAvailable();
      ShowRibbonUI(bRibbonUI);
      UISetCheck(ID_VIEW_RIBBON, bRibbonUI);
   }

   if (m_cszFilenameOpenAtStart.IsEmpty())
      DoFileNew();
   else
   {
      CString cszTitle = Path(m_cszFilenameOpenAtStart).FilenameAndExt();
      if (DoFileOpen(m_cszFilenameOpenAtStart, cszTitle))
      {
         m_mru.AddToList(m_cszFilenameOpenAtStart);
         m_mru.WriteToRegistry(c_pszSettingsRegkey);
      }
   }

   UIEnable(ID_SCRIPT_RUN, true);
   UIEnable(ID_SCRIPT_STOP, false);

   RestoreWindowPosition();

   return 0;
}

LRESULT MainFrame::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (!m_view.QueryClose())
      return 0;

   bHandled = false; // let app process message

   if (RunTimeHelper::IsRibbonUIAvailable())
   {
      bool bRibbonUI = IsRibbonUI();
      if (bRibbonUI)
         SaveRibbonSettings();

      CRibbonPersist(c_pszSettingsRegkey).Save(bRibbonUI, m_hgRibbonSettings);
   }

   StoreWindowPosition();

   return 0;
}

LRESULT MainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   m_processor.SetOutputDebugStringHandler(nullptr);

   m_processor.GetScheduler().SetExecutionStateChangedHandler(
      LuaScheduler::T_fnOnExecutionStateChanged());

   m_processor.Stop();

   // unregister message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != nullptr);
   pLoop->RemoveMessageFilter(this);
   pLoop->RemoveIdleHandler(this);

   bHandled = false;
   return 1;
}

LRESULT MainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   PostMessage(WM_CLOSE);
   return 0;
}

LRESULT MainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   AboutDlg dlg;
   dlg.DoModal(m_hWnd);

   return 0;
}

LRESULT MainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   DoFileNew();

   return 0;
}

LRESULT MainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CFileDialog dlg(TRUE, NULL, _T(""), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, g_pszLuaScriptingFilter);
   int iRet = dlg.DoModal(m_hWnd);

   if (iRet == IDOK)
   {
      bool bRet = m_view.QueryClose();
      if (!bRet)
      {
         if (!DoFileSaveAs())
            return 0;
      }

      if (DoFileOpen(dlg.m_ofn.lpstrFile, dlg.m_ofn.lpstrFileTitle))
      {
         m_mru.AddToList(dlg.m_ofn.lpstrFile);
         m_mru.WriteToRegistry(c_pszSettingsRegkey);
      }
   }
   return 0;
}

LRESULT MainFrame::OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   DoFileSave();

   return 0;
}

LRESULT MainFrame::OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   DoFileSaveAs();

   return 0;
}

LRESULT MainFrame::OnFileRecent(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   // check if we have to save the current one
   bool bRet = !m_view.QueryClose();
   if (bRet)
   {
      if (!m_view.DoFileSaveAs())
         return 0;
   }

   // get file name from the MRU list
   CString cszFile;
   if (m_mru.GetFromList(wID, cszFile))
   {
      CString cszFileName = Path(cszFile).FilenameAndExt();

      // open file
      if (DoFileOpen(cszFile, cszFileName))
         m_mru.MoveToTop(wID);
      else
         m_mru.RemoveFromList(wID);
      m_mru.WriteToRegistry(c_pszSettingsRegkey);
   }
   else
   {
      ::MessageBeep(MB_ICONERROR);
   }

   return 0;
}

LRESULT MainFrame::OnViewRibbon(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   ShowRibbonUI(!IsRibbonUI());
   UISetCheck(ID_VIEW_RIBBON, IsRibbonUI());
   return 0;
}

LRESULT MainFrame::OnViewOutput(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   // in single pane mode?
   bool bOutputPaneVisible = m_splitter.GetSinglePaneMode() == SPLIT_PANE_LEFT;

   m_splitter.SetSinglePaneMode(bOutputPaneVisible ? SPLIT_PANE_NONE : SPLIT_PANE_LEFT);

   UISetCheck(ID_VIEW_OUTPUT, bOutputPaneVisible);

   return 0;
}

LRESULT MainFrame::OnScriptRun(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_view.GetModify())
   {
      if (!DoFileSave())
         return 0;
   }

   CString cszFilename = m_view.GetFilePath();
   if (cszFilename.IsEmpty())
      return 0;

   {
      CString cszText;
      cszText.Format(_T("Start executing file %s...\n\n"),
         cszFilename.GetString());

      m_ecOutputWindow.OutputText(cszText);
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

      UIEnable(ID_SCRIPT_RUN, true);
      UIEnable(ID_SCRIPT_STOP, false);
   }

   return 0;
}

LRESULT MainFrame::OnScriptStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_processor.Stop();

   return 0;
}

void MainFrame::StoreWindowPosition()
{
   m_settings.m_windowPlacementMainFrame.Get(m_hWnd);
   m_settings.Store();
}

void MainFrame::RestoreWindowPosition()
{
   if (m_settings.m_windowPlacementMainFrame.showCmd != 0)
   {
      m_settings.m_windowPlacementMainFrame.Set(m_hWnd);

      ShowWindow(m_settings.m_windowPlacementMainFrame.showCmd);

      if (MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONULL) == NULL)
      {
         // center on desktop instead
         SetWindowPos(HWND_TOP, 0, 0, 800, 600, SWP_SHOWWINDOW);
         CenterWindow(GetDesktopWindow());
      }
   }
}

void MainFrame::DoFileNew()
{
   if (m_view.QueryClose())
   {
      m_view.Init(_T(""), _T("Untitled.lua"));
      m_view.SetText("");
      m_view.SetSavePoint();
      UpdateTitle();
   }
}

bool MainFrame::DoFileOpen(LPCTSTR lpstrFileName, LPCTSTR lpstrFileTitle)
{
   bool bRet = m_view.Load(lpstrFileName) != FALSE;
   if (bRet)
   {
      m_view.Init(lpstrFileName, lpstrFileTitle);
      UpdateTitle();
   }
   else
   {
      CString cszText;
      cszText.Format(_T("Error reading file!\nFilename: %s"), lpstrFileName);
      AtlMessageBox(m_hWnd, cszText.GetString(), IDR_MAINFRAME, MB_OK | MB_ICONERROR);
   }

   return bRet;
}

bool MainFrame::DoFileSave()
{
   if (!m_view.GetFilePath().IsEmpty())
   {
      if (m_view.Save(m_view.GetFilePath()))
      {
         m_mru.AddToList(m_view.GetFilePath());
         m_mru.WriteToRegistry(c_pszSettingsRegkey);

         UpdateTitle();
      }
      else
      {
         AtlMessageBox(m_hWnd, _T("Error writing file!\n"), IDR_MAINFRAME, MB_OK | MB_ICONERROR);
         return false;
      }
   }
   else
   {
      return DoFileSaveAs();
   }

   return true;
}

bool MainFrame::DoFileSaveAs()
{
   bool bRet = m_view.DoFileSaveAs();
   if (bRet)
   {
      UpdateTitle();

      m_mru.AddToList(m_view.GetFilePath());
      m_mru.WriteToRegistry(c_pszSettingsRegkey);
   }

   return bRet;
}

void MainFrame::UpdateTitle()
{
   CString cszTitle(MAKEINTRESOURCE(IDR_MAINFRAME));

   m_bScriptingFileModified = m_view.GetModify();

   // app title + scripting file title
   cszTitle.AppendFormat(_T(" - [%s%s]"),
      m_view.GetFilePath().GetString(),
      m_bScriptingFileModified ? _T("*") : _T(""));

   SetWindowText(cszTitle);
}

void MainFrame::UIUpdateAll()
{
   UIEnable(ID_EDIT_PASTE, m_view.CanPaste());
   UIEnable(ID_EDIT_UNDO, m_view.CanUndo());
   UIEnable(ID_EDIT_REDO, m_view.CanRedo());

   bool bSel = m_view.IsTextSelected();
   UIEnable(ID_EDIT_CUT, bSel);
   UIEnable(ID_EDIT_COPY, bSel);
   UIEnable(ID_EDIT_CLEAR, bSel);

   bool bModified = m_view.GetModify();

   UIEnable(ID_FILE_SAVE, bModified);
   UIEnable(ID_FILE_SAVE_AS, bModified);

   if (bModified != m_bScriptingFileModified)
   {
      m_bScriptingFileModified = bModified;
      UpdateTitle();
   }
}

void MainFrame::SetupCmdBar()
{
   // create command bar window
   m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
   // attach menu
   m_CmdBar.AttachMenu(GetMenu());
   // load command bar images
   m_CmdBar.LoadImages(IDR_MAINFRAME);

   // remove old menu
   SetMenu(NULL);
}

void MainFrame::SetupRibbonBar()
{
   // check if ribbon is available
   bool bRibbonUI = RunTimeHelper::IsRibbonUIAvailable();

   if (bRibbonUI)
   {
      UIAddMenu(m_CmdBar.GetMenu(), true);

      UIRemoveUpdateElement(ID_FILE_MRU_FIRST);

      CRibbonPersist(c_pszSettingsRegkey).Restore(bRibbonUI, m_hgRibbonSettings);
   }
   else
      CMenuHandle(m_CmdBar.GetMenu()).DeleteMenu(ID_VIEW_RIBBON, MF_BYCOMMAND);
}

void MainFrame::SetupToolbar()
{
   CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
   AddSimpleReBarBand(m_CmdBar);

   HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
   AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

   UIAddToolBar(hWndToolBar);
}

void MainFrame::SetupMRUList()
{
   CMenuHandle menu = m_CmdBar.GetMenu();
   CMenuHandle fileMenu = menu.GetSubMenu(FILE_MENU_POSITION);

#ifdef _DEBUG
   CString cszMenuString;
   fileMenu.GetMenuString(RECENT_MENU_POSITION, cszMenuString, MF_BYPOSITION);

   // when this asserts, RECENT_MENU_POSITION isn't correct anymore
   ATLASSERT(cszMenuString.CompareNoCase(_T("Recent &Files")) == 0);
#endif

   CMenuHandle mruMenu = fileMenu.GetSubMenu(RECENT_MENU_POSITION);
   m_mru.SetMenuHandle(mruMenu);

   m_mru.ReadFromRegistry(c_pszSettingsRegkey);
}

/// \see http://www.codeproject.com/Articles/2948/How-to-use-the-WTL-multipane-status-bar-control
void MainFrame::SetupStatusBar()
{
   // added WS_CLIPCHILDREN to default styles
   CreateSimpleStatusBar(
      ATL_IDS_IDLEMESSAGE, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | SBARS_SIZEGRIP);

   m_statusBar.SubclassWindow(m_hWndStatusBar);

   // set status bar panes. ID_DEFAULT_PANE is defined by WTL
   int arrPanes[] = { ID_DEFAULT_PANE, IDR_PANE_SCRIPT_STATUS };

   m_statusBar.SetPanes(arrPanes, sizeof(arrPanes) / sizeof(int), false);

   m_statusBar.SetPaneWidth(IDR_PANE_SCRIPT_STATUS, 150);

   OnExecutionStateChanged(LuaScheduler::stateIdle);

   m_processor.GetScheduler().SetExecutionStateChangedHandler(
      std::bind(&MainFrame::OnExecutionStateChanged, this, std::placeholders::_1));
}

void MainFrame::SetupView()
{
   m_hWndClient = m_splitter.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   m_view.Create(m_hWndClient, rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);

   m_view.SetupSourceEditor();

   SetupOutputPane();

   m_splitter.SetSplitterPanes(m_view, m_pane);
   m_splitter.SetSinglePaneMode(SPLIT_PANE_NONE);

   m_splitter.SetSplitterPosPct(75);

   UISetCheck(ID_VIEW_OUTPUT, true);
}

void MainFrame::SetupOutputPane()
{
   m_pane.SetPaneContainerExtendedStyle(PANECNT_FLATBORDER | PANECNT_NOCLOSEBUTTON);
   m_pane.Create(m_hWndClient, _T("Output"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   m_ecOutputWindow.Create(m_pane);

   m_pane.SetClient(m_ecOutputWindow);

   m_ecOutputWindow.OutputText(_T("Ready.\n"));
}

void MainFrame::OnOutputDebugString(const CString& cszText)
{
   m_ecOutputWindow.OutputText(cszText);
}

void MainFrame::OnExecutionStateChanged(LuaScheduler::T_enExecutionState enExecutionState)
{
   PostMessage(WM_EXECUTION_STATE_CHANGED, static_cast<WPARAM>(enExecutionState));
}

LRESULT MainFrame::OnMessageExecutionStateChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   LuaScheduler::T_enExecutionState enExecutionState =
      static_cast<LuaScheduler::T_enExecutionState>(wParam);

   LPCTSTR pszText = nullptr;

   switch (enExecutionState)
   {
   case LuaScheduler::stateIdle:
      UIEnable(ID_SCRIPT_RUN, true);
      UIEnable(ID_SCRIPT_STOP, false);
      pszText = _T("Idle.");
      break;

   case LuaScheduler::stateRunning:
      pszText = _T("Running...");
      UIEnable(ID_SCRIPT_RUN, false);
      UIEnable(ID_SCRIPT_STOP, true);
      break;

   case LuaScheduler::stateYield:
      pszText = _T("Waiting...");
      break;

   case LuaScheduler::stateDebug:
      break;

   case LuaScheduler::stateError:
      pszText = _T("Error.");
      UIEnable(ID_SCRIPT_RUN, false);
      UIEnable(ID_SCRIPT_STOP, true);
      break;

   default:
      ATLASSERT(false); // unknown state
      break;
   }

   if (pszText != nullptr)
      m_statusBar.SetPaneText(IDR_PANE_SCRIPT_STATUS, pszText);

   return 0;
}
