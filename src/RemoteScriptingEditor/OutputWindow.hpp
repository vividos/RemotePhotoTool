//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file OutputWindow.hpp Output window for debugging Lua scripts
//
#pragma once

// includes
#include "LuaScriptEditorView.hpp"

/// \brief output window
/// \details a read-only output window for outputting debug text during Lua script running.
class OutputWindow :
   public CWindowImpl<OutputWindow, ScintillaWindowAdapter, CControlWinTraits>,
   public ScintillaEditCommands<OutputWindow>
{
   /// base class type
   typedef CWindowImpl<OutputWindow, ScintillaWindowAdapter, CControlWinTraits> BaseClass;

   // edit commands mixin type
   typedef ScintillaEditCommands<OutputWindow> EditCommandsClass;

public:
   /// ctor
   OutputWindow() throw() {}

   /// window class infos
   DECLARE_WND_SUPERCLASS(NULL, ScintillaWindowAdapter::GetWndClassName())

   /// creates new window, with parent
   HWND Create(HWND hWndParent)
   {
      HWND hWnd = BaseClass::Create(hWndParent, rcDefault);

      StyleSetFont(STYLE_DEFAULT, "Courier New");
      StyleSetSize(STYLE_DEFAULT, 11);
      SetLexer(SCLEX_ERRORLIST);

      SetReadOnly(true);

      return hWnd;
   }

   /// outputs text by appending and scrolling to the output line
   void OutputText(LPCTSTR pszText)
   {
      CStringA cszaText(pszText);

      SetReadOnly(false);
      AppendText(cszaText.GetString(), cszaText.GetLength());
      SetReadOnly(true);

      ScrollToLine(GetLineCount());
   }

private:
   // message map
   BEGIN_MSG_MAP(OutputWindow)
      CHAIN_MSG_MAP_ALT(EditCommandsClass, 1)
   END_MSG_MAP()
};
