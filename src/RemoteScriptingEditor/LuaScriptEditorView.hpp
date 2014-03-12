//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file LuaScriptEditorView.hpp View for editing Lua scripts
//
#pragma once

/// Lua script editor view
class LuaScriptEditorView : public CWindowImpl<LuaScriptEditorView, CScintillaWindow>
{
   typedef CScintillaWindow BaseClass;

public:
   /// ctor
   LuaScriptEditorView() throw() {}

   /// pre-translates messages
   BOOL PreTranslateMessage(MSG* pMsg);

   /// window class infos
   DECLARE_WND_SUPERCLASS(NULL, BaseClass::GetWndClassName())

   /// sets up source editor
   void SetupSourceEditor();

   /// returns file path of edited file
   CString GetFilePath() const { return m_cszFilePath; }

   /// inits editor with empty file
   void Init(LPCTSTR lpstrFilePath, LPCTSTR lpstrFileTitle);

   /// executes "File | Save as" command
   bool DoFileSaveAs();

   /// queries user to save the current file, when modified
   bool QueryClose();

   /// returns if there's a text selection in the editor
   bool IsTextSelected() const;

private:
   BEGIN_MSG_MAP(LuaScriptEditorView)
      COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnEditUndo)
      COMMAND_ID_HANDLER(ID_EDIT_REDO, OnEditRedo)
      COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCut)
      COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
      COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditPaste)
      COMMAND_ID_HANDLER(ID_EDIT_CLEAR, OnEditClear)
      COMMAND_ID_HANDLER(ID_EDIT_SELECT_ALL, OnEditSelectAll)
      //COMMAND_ID_HANDLER(ID_EDIT_FIND, OnEditFind)
      //COMMAND_ID_HANDLER(ID_EDIT_REPEAT, OnEditRepeat)
      //COMMAND_ID_HANDLER(ID_EDIT_REPLACE, OnEditReplace)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   /// called for command "Edit | Undo"
   LRESULT OnEditUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      Undo();
      return 0;
   }

   /// called for command "Edit | Redo"
   LRESULT OnEditRedo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      Redo();
      return 0;
   }

   /// called for command "Edit | Cut"
   LRESULT OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      Cut();
      return 0;
   }

   /// called for command "Edit | Copy"
   LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      Copy();
      return 0;
   }

   /// called for command "Edit | Paste"
   LRESULT OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      Paste();
      return 0;
   }

   /// called for command "Edit | Clear"
   LRESULT OnEditClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      Clear();
      return 0;
   }

   /// called for command "Edit | Select All"
   LRESULT OnEditSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) 
   {
      SelectAll();
      return 0;
   }

private:
   // model

   /// full file path and file name of the currently open file
   CString m_cszFilePath;

   /// file title (e.g. file name only)
   CString m_cszFileTitle;
};
