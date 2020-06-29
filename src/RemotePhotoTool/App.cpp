//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file RemotePhotoTool/App.cpp Application
//

// includes
#include "stdafx.h"
#include "App.hpp"
#include <ulib/ProgramOptions.hpp>
#include "resource.h"
#include "MainFrame.hpp"
#include <ulib/Path.hpp>
#include <ulib/CrashReporter.hpp>
#include <crtdbg.h>
#include <wia.h>

#pragma comment(lib, "wiaguid.lib")

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
   HRESULT hRes = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
   ATLASSERT(SUCCEEDED(hRes));

   // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
#pragma warning(suppress: 6387)
   ::DefWindowProc(NULL, 0, 0, 0L);

   AtlInitCommonControls(ICC_WIN95_CLASSES);

   hRes = _Module.Init(NULL, hInstance);
   ATLASSERT(SUCCEEDED(hRes));
}

App::~App()
{
   _Module.Term();
   ::CoUninitialize();
}

void App::InitCrashReporter()
{
   CString cszFolder = Path::SpecialFolder(CSIDL_LOCAL_APPDATA) + _T("\\RemotePhotoTool\\");

   if (!Path::FolderExists(cszFolder))
      CreateDirectory(cszFolder, NULL);

   cszFolder += _T("crashdumps\\");

   if (!Path::FolderExists(cszFolder))
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

   bool bStartApp = true;

   options.RegisterOption(_T("r"), _T("register-wia"), _T("registers WIA handler for RemotePhotoTool"),
      0, [&](const std::vector<CString>&)
   {
      RegisterWIAHandler(true); bStartApp = false; return true;
   });

   options.RegisterOption(_T("u"), _T("unregister-wia"), _T("unregisters WIA handler for RemotePhotoTool"),
      0, [&](const std::vector<CString>&)
   {
      RegisterWIAHandler(false); bStartApp = false; return true;
   });

   options.RegisterOption(_T("o"), _T("open-wia"), _T("opens camera using WIA handler; <arg1>: Port, <arg2>: Guid"),
      2, [&](const std::vector<CString>& vecArgs)
   {
      vecArgs;
      ATLASSERT(vecArgs.size() == 2);
      // just let the app start here
      return true;
   });

   options.Parse(GetCommandLine());

   if (!bStartApp || options.IsSelectedHelpOption())
      return false;

   return true;
}

/// \see http://stackoverflow.com/questions/324974/in-wia-how-can-i-add-my-program-in-the-program-list-of-acquiring-images-from-a
void App::RegisterWIAHandler(bool bRegister)
{
   CComPtr<IWiaDevMgr> spWiaDevMgr;
   HRESULT hr = spWiaDevMgr.CoCreateInstance(CLSID_WiaDevMgr,
      NULL,
      CLSCTX_LOCAL_SERVER);

   if (FAILED(hr))
      return;

   CComBSTR bstrCommandline(_T("\"") + Path::ModuleFilename() + _T("\" --open-wia \"%1\" \"%2\""));
   CComBSTR bstrName(CString(MAKEINTRESOURCE(IDR_MAINFRAME)));
   CComBSTR bstrDescription(CString(MAKEINTRESOURCE(IDR_WIA_DESC)));
   CComBSTR bstrIcon(Path::ModuleFilename() + _T(",0"));

   hr = spWiaDevMgr->RegisterEventCallbackProgram(
      bRegister ? WIA_REGISTER_EVENT_CALLBACK : WIA_UNREGISTER_EVENT_CALLBACK,
      NULL,
      &WIA_EVENT_DEVICE_CONNECTED,
      bstrCommandline,
      bstrName,
      bstrDescription,
      bstrIcon);

   // when this fails, the user isn't an elevated user
   if (FAILED(hr))
      AtlMessageBox(NULL, _T("Error registering/unregisterin WIA event! Are you administrator?"), IDR_MAINFRAME, MB_OK);
}
