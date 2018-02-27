//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file GeoTagTool.cpp GeoTagTool main function
//
#include "stdafx.h"
#include "res/Ribbon.h"
#include "resource.h"
#include "GeoTagToolView.hpp"
#include "AboutDlg.hpp"
#include "MainFrame.hpp"

/// app module
CAppModule _Module;

/// runs GeoTagTool main frame window
int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
   CMessageLoop theLoop;
   _Module.AddMessageLoop(&theLoop);

   MainFrame wndMain;

   if (wndMain.CreateEx() == NULL)
   {
      ATLTRACE(_T("Main window creation failed!\n"));
      return 0;
   }

   wndMain.ShowWindow(nCmdShow);

   int ret = theLoop.Run();

   _Module.RemoveMessageLoop();
   return ret;
}

/// main function
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
   HRESULT hRes = ::CoInitialize(NULL);
   ATLASSERT(SUCCEEDED(hRes));

   AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

   hRes = _Module.Init(NULL, hInstance);
   ATLASSERT(SUCCEEDED(hRes));

   int ret = 0;
   if (RunTimeHelper::IsRibbonUIAvailable())
      ret = Run(lpstrCmdLine, nCmdShow);
   else
      AtlMessageBox(NULL, L"Cannot run with this version of Windows", IDR_MAINFRAME);

   _Module.Term();
   ::CoUninitialize();

   return ret;
}
