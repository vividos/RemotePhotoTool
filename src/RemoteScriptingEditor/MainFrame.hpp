//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file RemoteScriptingEditor\MainFrame.hpp Main application frame
//
#pragma once

// includes
#include "LuaScriptEditorView.hpp"
#include "OutputWindow.hpp"
#include "CameraScriptProcessor.hpp"
#include "LuaScheduler.hpp"

/// \brief application main frame
/// \details uses ribbon for commands
/// \see http://www.codeproject.com/Articles/54116/Relook-your-Old-and-New-Native-Applications-with-a
class MainFrame :
   public CRibbonFrameWindowImpl<MainFrame>,
   public CMessageFilter,
   public CIdleHandler
{
   /// base class typedef
   typedef CRibbonFrameWindowImpl<MainFrame> BaseClass;

public:
   /// ctor
   MainFrame() throw();

   /// sets filename of file to open at start
   void OpenFileAtStart(const CString& cszFilename)
   {
      m_cszFilenameOpenAtStart = cszFilename;
   }

   DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

private:
   /// pretranslates message
   virtual BOOL PreTranslateMessage(MSG* pMsg);

   /// called when message loop is idle
   virtual BOOL OnIdle();

   /// updates all controls
   void UIUpdateAll();

private:
   friend CDynamicUpdateUI;
   friend CRibbonImpl<MainFrame>;
   friend CRibbonFrameWindowImplBase;

   // ribbon control map
   BEGIN_RIBBON_CONTROL_MAP(MainFrame)
      RIBBON_CONTROL(m_mru)
   END_RIBBON_CONTROL_MAP()

   BEGIN_UPDATE_UI_MAP(MainFrame)
      UPDATE_ELEMENT(ID_FILE_NEW, UPDUI_TOOLBAR | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_FILE_OPEN, UPDUI_TOOLBAR | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_FILE_SAVE, UPDUI_TOOLBAR | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_FILE_SAVE_AS, UPDUI_TOOLBAR | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_EDIT_UNDO, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_EDIT_CUT, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_EDIT_COPY, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_EDIT_PASTE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_EDIT_CLEAR, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_EDIT_SELECT_ALL, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_EDIT_FIND, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_EDIT_REPEAT, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_EDIT_REPLACE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
      UPDATE_ELEMENT(ID_VIEW_RIBBON, UPDUI_MENUPOPUP | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_SCRIPT_RUN, UPDUI_TOOLBAR | UPDUI_RIBBON)
      UPDATE_ELEMENT(ID_SCRIPT_STOP, UPDUI_TOOLBAR | UPDUI_RIBBON)
   END_UPDATE_UI_MAP()

   BEGIN_MSG_MAP(MainFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_CLOSE, OnClose)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
      COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
      COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
      COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
      COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
      COMMAND_ID_HANDLER(ID_FILE_SAVE_AS, OnFileSaveAs)
      COMMAND_RANGE_HANDLER(ID_FILE_MRU_FIRST, ID_FILE_MRU_LAST, OnFileRecent)
      COMMAND_ID_HANDLER(ID_VIEW_RIBBON, OnViewRibbon)
      COMMAND_ID_HANDLER(ID_VIEW_OUTPUT, OnViewOutput)
      COMMAND_ID_HANDLER(ID_SCRIPT_RUN, OnScriptRun)
      COMMAND_ID_HANDLER(ID_SCRIPT_STOP, OnScriptStop)
      CHAIN_MSG_MAP_MEMBER(m_view)
      CHAIN_MSG_MAP_MEMBER(m_ecOutputWindow)
      CHAIN_MSG_MAP(CRibbonFrameWindowImpl<MainFrame>)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
   LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnFileRecent(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewRibbon(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewOutput(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnScriptRun(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnScriptStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   /// sets up command bar
   void SetupCmdBar();

   /// sets up ribbon bar
   void SetupRibbonBar();

   /// sets up toolbar
   void SetupToolbar();

   /// sets up Most Recently Used file list
   void SetupMRUList();

   /// sets up status bar
   void SetupStatusBar();

   /// sets up view
   void SetupView();

   /// sets up output pane
   void SetupOutputPane();

   /// carries out "file | new" operation
   void DoFileNew();

   /// carries out "file | open" operation
   bool DoFileOpen(LPCTSTR lpstrFileName, LPCTSTR lpstrFileTitle);

   /// carries out "file | save" operation
   bool DoFileSave();

   /// carries out "file | save as" operation
   bool DoFileSaveAs();

   /// updates window title
   void UpdateTitle();

   /// called when debug string is output
   void OnOutputDebugString(const CString& cszText);

   /// called when camera script processor changes its state
   void OnExecutionStateChanged(LuaScheduler::T_enExecutionState enExecutionState);

private:
   // UI

   /// command bar
   CCommandBarCtrl m_CmdBar;

   /// most recently used items list
   CRibbonRecentItemsCtrl<ID_RIBBON_RECENT_FILES> m_mru;

   /// splitter window
   CHorSplitterWindow m_splitter;

   /// editor view
   LuaScriptEditorView m_view;

   /// pane container for output window
   CPaneContainer m_pane;

   /// output window
   OutputWindow m_ecOutputWindow;

   /// status bar
   CMultiPaneStatusBarCtrl m_statusBar;


   // model

   /// camera script processor
   CameraScriptProcessor m_processor;

   /// when showing the scripting view, this tracks if the currently open file has been modified
   bool m_bScriptingFileModified;

   /// filename of file to open at start
   CString m_cszFilenameOpenAtStart;
};
