//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file RemoteScriptingEditor/App.hpp Application
//
#pragma once

/// application
class App
{
public:
   /// ctor
   App(HINSTANCE hInstance);
   /// dtor
   ~App();

   /// inits crash reporter
   static void InitCrashReporter();

   /// runs application
   int Run(LPCTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT);

private:
   /// parses command line; returns true when app should be started
   bool ParseCommandLine();

private:
   /// filename of file to open at start
   CString m_cszFilename;
};
