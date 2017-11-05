//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file RemoteScriptingEditor/App.cpp Application
//

// includes
#include "stdafx.h"
#include "App.hpp"
#include <ulib/ProgramOptions.hpp>
#include "res/Ribbon.h"
#include "resource.h"
#include "MainFrame.hpp"
#include "Filesystem.hpp"
#include <ulib/Path.hpp>
#include <ulib/CrashReporter.hpp>
#include <crtdbg.h>
#include <ShlObj.h>

/// WTL app module
CAppModule _Module;

App::App(HINSTANCE hInstance)
{
#ifdef _DEBUG
   // turn on leak-checking
   int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
   _CrtSetDbgFlag(flag | _CRTDBG_LEAK_CHECK_DF);
#endif

   // note: Apartment Threading needed for Canon ED-SDK
   ATLVERIFY(SUCCEEDED(::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)));

   // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
#pragma warning(suppress: 6387)
   ::DefWindowProc(NULL, 0, 0, 0L);

   AtlInitCommonControls(ICC_WIN95_CLASSES);

   ATLVERIFY(SUCCEEDED(_Module.Init(NULL, hInstance)));
}

App::~App()
{
   _Module.Term();
   ::CoUninitialize();
}

void App::InitCrashReporter()
{
   CString cszFolder = Path::SpecialFolder(CSIDL_LOCAL_APPDATA) + _T("\\RemotePhotoTool\\");

   if (!Path(cszFolder).FolderExists())
      CreateDirectory(cszFolder, NULL);

   cszFolder += _T("crashdumps\\");

   if (!Path(cszFolder).FolderExists())
      CreateDirectory(cszFolder, NULL);

   CrashReporter::Init(cszFolder, _T("RemotePhotoTool"));
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
