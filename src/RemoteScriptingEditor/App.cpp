//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file RemoteScriptingEditor/App.cpp Application
//

// includes
#include "stdafx.h"
#include "App.hpp"
#include "ProgramOptions.hpp"
#include "res\Ribbon.h"
#include "resource.h"
#include "MainFrame.hpp"
#include "Filesystem.hpp"
#include "CrashReporter.hpp"
#include <crtdbg.h>

/// WTL app module
CAppModule _Module;

App::App(HINSTANCE hInstance) throw()
{
#ifdef _DEBUG
   // turn on leak-checking
   int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
   _CrtSetDbgFlag(flag | _CRTDBG_LEAK_CHECK_DF);
#endif

   // note: Apartment Threading needed for Canon ED-SDK
   HRESULT hRes = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
   ATLASSERT(SUCCEEDED(hRes));

   // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
#pragma warning(suppress: 6387)
   ::DefWindowProc(NULL, 0, 0, 0L);

   AtlInitCommonControls(ICC_WIN95_CLASSES);

   hRes = _Module.Init(NULL, hInstance);
   ATLASSERT(SUCCEEDED(hRes));
}

App::~App() throw()
{
   _Module.Term();
   ::CoUninitialize();
}

void App::InitCrashReporter()
{
   CString cszFolder = App_GetAppDataFolder(appDataUserNonRoaming) + _T("\\RemotePhotoTool\\");

   if (!Directory_Exists(cszFolder))
      CreateDirectory(cszFolder, NULL);

   cszFolder += _T("crashdumps\\");

   if (!Directory_Exists(cszFolder))
      CreateDirectory(cszFolder, NULL);

   CrashReporter::Init(cszFolder);
}

int App::Run(LPCTSTR /*lpstrCmdLine*/, int nCmdShow)
{
   if (!ParseCommandLine())
      return 0;

   CMessageLoop theLoop;
   _Module.AddMessageLoop(&theLoop);

   MainFrame wndMain;

   if (!m_cszFilename.IsEmpty())
      wndMain.OpenFileAtStart(m_cszFilename);

   if (wndMain.CreateEx() == nullptr)
   {
      ATLTRACE(_T("Main window creation failed!\n"));
      return 0;
   }

   wndMain.ShowWindow(nCmdShow);

   int nRet = theLoop.Run();

   _Module.RemoveMessageLoop();
   return nRet;
}

/// \retval false App should not be started
/// \retval true App can be started
bool App::ParseCommandLine()
{
   ProgramOptions options;

   options.RegisterOutputHandler(&ProgramOptions::OutputConsole);
   options.RegisterHelpOption();

   options.RegisterParameterHandler([&](const CString& cszFilename)-> bool
   {
      // only take first filename
      if (m_cszFilename.IsEmpty())
         m_cszFilename = cszFilename;
      return true;
   });

   options.Parse(GetCommandLine());

   return !options.IsSelectedHelpOption();
}
