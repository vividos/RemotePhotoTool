//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ScriptingPhotoModeView.hpp View for scripting camera controls
//
#pragma once

// includes
#include "IPhotoModeView.hpp"
#include <atlsplit.h>

#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#include <atlscintilla.h>
#pragma warning(pop)

// forward references
class IPhotoModeViewHost;

// extern defines
extern LPCTSTR g_pszLuaScriptingFilter;

/// view for Lua Scripting photo mode
class ScriptingPhotoModeView :
   public CHorSplitterWindow,
   public IPhotoModeView
{
   /// base class
   typedef CHorSplitterWindow BaseClass;

public:
   /// ctor
   ScriptingPhotoModeView(IPhotoModeViewHost& host) throw();
   /// dtor
   virtual ~ScriptingPhotoModeView() throw() {}

   /// returns file path of edited file
   CString GetFilePath() const { return m_cszFilePath; }

   /// loads new file contents
   bool LoadFile(LPCTSTR lpstrFilePath);

   /// saves editor contents to file
   bool SaveFile(LPCTSTR lpstrFilePath);

   /// inits editor with empty file
   void Init(LPCTSTR lpstrFilePath, LPCTSTR lpstrFileTitle);

   /// sets new editor content
   void SetContent(const CString& cszText);

   /// returns if editor content is modified
   bool IsModified() const;

   /// executes "File | Save as" command
   bool DoFileSaveAs();

   /// queries user to save the current file, when modified
   bool QueryClose();

   /// returns if "Edit | Paste" command is currently available (something is on the clipboard)
   bool CanPaste() const;

   /// returns if "Edit | Undo" command is currently available
   bool CanUndo() const;

   /// returns if "Edit | Redo" command is currently available
   bool CanRedo() const;

   /// returns if there's a text selection in the editor
   bool IsTextSelected() const;

private:
   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hWndParent) override;

   virtual void SetSourceDevice(std::shared_ptr<SourceDevice> spSourceDevice) override
   {
      m_spSourceDevice = spSourceDevice;
   }

   virtual BOOL PreTranslateMessage(MSG* pMsg) override;

   virtual void DestroyView() override
   {
      ATLVERIFY(TRUE == DestroyWindow());
   }

private:
   BEGIN_DDX_MAP(ScriptingPhotoModeView)
   END_DDX_MAP()

   BEGIN_MSG_MAP(ScriptingPhotoModeView)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
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
      REFLECT_NOTIFICATIONS() // to make sure superclassed controls get notification messages
      CHAIN_MSG_MAP(BaseClass)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   /// called for command "Edit | Undo"
   LRESULT OnEditUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      m_sourceEditor.Undo();
      return 0;
   }

   /// called for command "Edit | Redo"
   LRESULT OnEditRedo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      m_sourceEditor.Redo();
      return 0;
   }

   /// called for command "Edit | Cut"
   LRESULT OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      m_sourceEditor.Cut();
      return 0;
   }

   /// called for command "Edit | Copy"
   LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      m_sourceEditor.Copy();
      return 0;
   }

   /// called for command "Edit | Paste"
   LRESULT OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      m_sourceEditor.Paste();
      return 0;
   }

   /// called for command "Edit | Clear"
   LRESULT OnEditClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      m_sourceEditor.Clear();
      return 0;
   }

   /// called for command "Edit | Select All"
   LRESULT OnEditSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) 
   {
      m_sourceEditor.SelectAll();
      return 0;
   }

   /// sets up output pane
   void SetupOutputPane();

   /// sets up source editor
   void SetupSourceEditor();

private:
   /// host access
   IPhotoModeViewHost& m_host;

   // UI

   /// source editor window
   CScintillaWindow m_sourceEditor;

   /// pane container for output window
   CPaneContainer m_pane;

   /// output window
   CEdit m_ecOutputWindow;


   // model

   /// full file path and file name of the currently open file
   CString m_cszFilePath;

   /// file title (e.g. file name only)
   CString m_cszFileTitle;

   /// source device
   std::shared_ptr<SourceDevice> m_spSourceDevice;
};
