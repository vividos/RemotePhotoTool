//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file ScriptingPhotoModeView.hpp  Scripting photo mode view
//
#pragma once

// includes
#include "IPhotoModeView.hpp"
#include "CameraScriptProcessor.hpp"
#include "LuaScriptViewerView.hpp"
#include "LuaScheduler.hpp"

// forward references
class IPhotoModeViewHost;

/// message sent when execution state has changed
#define WM_EXECUTION_STATE_CHANGED (WM_USER+1)

/// view for scripting photo mode view
class ScriptingPhotoModeView :
   public IPhotoModeView,
   public CSplitterWindowImpl<ScriptingPhotoModeView>
{
public:
   /// ctor
   ScriptingPhotoModeView(IPhotoModeViewHost& host);
   /// dtor
   virtual ~ScriptingPhotoModeView() {}

private:
   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hWndParent) override;

   virtual BOOL PreTranslateMessage(MSG* /*pMsg*/) override { return FALSE; }

   virtual void DestroyView() override
   {
      ATLVERIFY(TRUE == DestroyWindow());
   }

   /// sets up output pane
   void SetupOutputPane();

   /// called when debug string is output
   void OnOutputDebugString(const CString& cszText);

   /// opens an existing script and loads it
   void OpenScript();

   /// calls to external editor to edit the file
   void EditScript(const CString& cszFilename);

private:
   // message map
   BEGIN_MSG_MAP(ScriptingPhotoModeView)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      COMMAND_ID_HANDLER(ID_SCRIPTING_OPEN, OnScriptingOpenScript)
      COMMAND_ID_HANDLER(ID_SCRIPTING_RELOAD, OnScriptingReload)
      COMMAND_ID_HANDLER(ID_SCRIPTING_RUN, OnScriptingRun)
      COMMAND_ID_HANDLER(ID_SCRIPTING_STOP, OnScriptingStop)
      COMMAND_ID_HANDLER(ID_SCRIPTING_EDIT, OnScriptingEditScript)
      MESSAGE_HANDLER(WM_EXECUTION_STATE_CHANGED, OnMessageExecutionStateChanged)
      CHAIN_MSG_MAP(CSplitterWindowImpl<ScriptingPhotoModeView>)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
// LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)

   /// called when window is destroyed
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called when menu "Scripting | Open script" is selected
   LRESULT OnScriptingOpenScript(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when menu "Scripting | Reload script" is selected
   LRESULT OnScriptingReload(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when menu "Scripting | Run" is selected
   LRESULT OnScriptingRun(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when menu "Scripting | Stop" is selected
   LRESULT OnScriptingStop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when menu "Scripting | Edit script" is selected
   LRESULT OnScriptingEditScript(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when message is sent when execution state has changed
   LRESULT OnMessageExecutionStateChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called by Lua scheduler when execution state has changed
   void OnExecutionStateChanged(LuaScheduler::T_enExecutionState enExecutionState);

private:
   /// host access
   IPhotoModeViewHost& m_host;

   // model

   /// remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;

   /// camera script processor
   CameraScriptProcessor m_processor;


   // UI

   /// script view
   LuaScriptViewerView m_view;

   /// pane container for output window
   CPaneContainer m_pane;

   /// output window
   CScintillaWindow m_ecOutputWindow;
};
