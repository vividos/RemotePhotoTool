//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file LuaScriptViewerView.hpp Viewer view for Lua scripts
//
#pragma once

/// Viewer class for Lua scripts, based on Scintilla editing control.
/// Editing is switched off.
class LuaScriptViewerView :
   public CWindowImpl<LuaScriptViewerView, CScintillaWindow>
{
   /// base class type
   typedef CWindowImpl<LuaScriptViewerView, CScintillaWindow> BaseClass;

public:
   /// ctor
   LuaScriptViewerView() {}

   /// window class infos
   DECLARE_WND_SUPERCLASS(NULL, CScintillaWindow::GetWndClassName())

   /// sets up source editor
   void SetupSourceEditor();

   /// returns file path of edited file
   CString GetFilePath() const { return m_cszFilePath; }

   /// inits editor with empty file
   void Init(LPCTSTR lpstrFilePath);

private:
   BEGIN_MSG_MAP(LuaScriptViewerView)
   END_MSG_MAP()

private:
   // model

   /// full file path and file name of the currently open file
   CString m_cszFilePath;
};
