//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file RemoteScriptingEditor.cpp RemoteScriptingEditor main function
//

// includes
#include "stdafx.h"
#include "App.hpp"

/// main function
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
   App::InitCrashReporter();

   App app(hInstance);
   return app.Run(lpstrCmdLine, nCmdShow);
}
