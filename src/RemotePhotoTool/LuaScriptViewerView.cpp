#include "stdafx.h"
#include "LuaScriptViewerView.hpp"

/// file open filter for Lua scripting
//LPCTSTR g_pszLuaScriptingFilter =
//_T("Lua Files (*.lua)\0*.lua\0")
//_T("All Files (*.*)\0*.*\0")
//_T("");

void LuaScriptViewerView::SetupSourceEditor()
{
   StyleSetFont(STYLE_DEFAULT, "Courier New");
   StyleSetSize(STYLE_DEFAULT, 11);
   SetEdgeColumn(100);
   SetEdgeMode(EDGE_LINE);
   SetMarginWidthN(0, 50); // margin 0 displays line numbers, but has default width 0

   StyleSetFore(0, RGB(0x80, 0x80, 0x80));
   StyleSetFore(1, RGB(0x00, 0x7f, 0x00));
   StyleSetFore(2, RGB(0x00, 0x7f, 0x00));
   StyleSetFore(3, RGB(0x7f, 0x7f, 0x7f));
   StyleSetFore(4, RGB(0x00, 0x7f, 0x7f));
   StyleSetFore(5, RGB(0x00, 0x00, 0x7f));
   StyleSetFore(6, RGB(0x7f, 0x00, 0x7f));
   StyleSetFore(7, RGB(0x7f, 0x00, 0x7f));
   StyleSetFore(8, RGB(0x00, 0x7f, 0x7f));
   StyleSetFore(9, RGB(0x7f, 0x7f, 0x7f));
   StyleSetFore(10, RGB(0x00, 0x00, 0x00));
   StyleSetFore(11, RGB(0x00, 0x00, 0x00));
   StyleSetBold(5, true);
   StyleSetBold(10, true);

   SetLexer(SCLEX_LUA);

   // set Lua keywords
   SetKeyWords(0,
      "and break do else elseif end for function if in " // official keywords
      "local nil not or repeat return then until while "
      ""); // TODO add RemotePhotoTool specific classes

   SetTabWidth(3);
}

void LuaScriptViewerView::Init(LPCTSTR lpstrFilePath)
{
   m_cszFilePath = lpstrFilePath;
}
