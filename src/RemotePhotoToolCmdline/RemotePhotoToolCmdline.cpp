//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoToolCmdline\stdafx.cpp Precompiled header support
//

// includes
#include "stdafx.h"
#include "CmdlineApp.hpp"

/// command line app main function
int _tmain(int argc, _TCHAR* argv[])
{
   try
   {
      CmdlineApp::InitCrashReporter();

      CmdlineApp app;
      app.Run(argc, argv);

      return 0;
   }
   catch (...)
   {
      // exception while running
      _tprintf(_T("Exception while running RemotePhotoToolCmdLine\n"));
      return -1;
   }
}
