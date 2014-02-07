//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file RemotePhotoTool.cpp RemotePhotoTool main function
//

// includes
#include "stdafx.h"
#include "App.hpp"

/// main function
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
   App app(hInstance);
   return app.Run(lpstrCmdLine, nCmdShow);
}
