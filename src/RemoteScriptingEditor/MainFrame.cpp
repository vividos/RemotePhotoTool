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

extern LPCTSTR g_pszLuaScriptingFilter;

/// specifies the position of the File menu in IDR_TOOLBAR_SCRIPTING
#define FILE_MENU_POSITION    0

/// specifies the position of the Recent Files menu item in the File menu in IDR_TOOLBAR_SCRIPTING
#define RECENT_MENU_POSITION  5

/// settings registry key (subkey "Ribbon" is used for menu band)
LPCTSTR c_pszSettingsRegkey = _T("Software\\RemoteScriptingEditor");

MainFrame::MainFrame() throw()
:m_bScriptingFileModified(false)
{
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
   CreateSimpleStatusBar();
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

   DoFileNew();

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

   return 0;
}

LRESULT MainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
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

LRESULT MainFrame::OnScriptRun(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (!DoFileSave())
      return 0;

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
   }

   return 0;
}

LRESULT MainFrame::OnScriptStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_processor.Stop();
   return 0;
}

void MainFrame::DoFileNew()
{
   if (m_view.QueryClose())
   {
      m_view.Init(_T(""), _T("Untitled.lua"));
      m_view.SetText(""); // TODO load template
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
      MessageBox(_T("Error reading file!\n"));

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
         MessageBox(_T("Error writing file!\n"));
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
   HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

   CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
   AddSimpleReBarBand(hWndToolBar, NULL, TRUE);
   AddSimpleReBarBand(m_CmdBar);

   UIAddToolBar(hWndToolBar);
   UISetCheck(ID_VIEW_TOOLBAR, 1);
   UISetCheck(ID_VIEW_STATUS_BAR, 1);
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
}

void MainFrame::SetupOutputPane()
{
   m_pane.SetPaneContainerExtendedStyle(PANECNT_FLATBORDER | PANECNT_NOCLOSEBUTTON);
   m_pane.Create(m_hWndClient, _T("Output"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   m_ecOutputWindow.Create(CScintillaWindow::GetWndClassName(), m_pane, rcDefault, NULL,
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);

   m_pane.SetClient(m_ecOutputWindow);

   m_ecOutputWindow.StyleSetFont(STYLE_DEFAULT, "Courier New");
   m_ecOutputWindow.StyleSetSize(STYLE_DEFAULT, 11);
   m_ecOutputWindow.SetLexer(SCLEX_LUA);

   m_ecOutputWindow.SetText("Ready.\n");
}

void MainFrame::OnOutputDebugString(const CString& cszText)
{
   CStringA cszaText(cszText);
   m_ecOutputWindow.AppendText(cszaText.GetString(), cszaText.GetLength());

   m_ecOutputWindow.ScrollToLine(m_ecOutputWindow.GetLineCount());
}
