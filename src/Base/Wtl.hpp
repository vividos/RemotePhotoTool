//
// ulib - a collection of useful classes
// Copyright (C) 2009,2012,2014,2015 Michael Fink
//
/// \file Wtl.hpp configuration for WTL 8 or higher
//
#pragma once

// needed includes
#include "Atl.hpp"

// ignore /analyze warnings in WTL header files
#pragma warning(push)
#pragma warning(disable: 6001 6011 6031 6387)

// ignore warnings in Win32 API header files that WTL includes
#pragma warning(disable: 4091) // 'typedef ' : ignored on left of 'T1' when no variable is declared

// WTL includes
#include <atlapp.h>
extern CAppModule _Module; ///< app module
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <shellapi.h> // needed for ShellExecute, used in atlctrlx.h
#include <atlctrlx.h>
#include <atlddx.h>
#include <atlsplit.h>
#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
#include <atlribbon.h>
#endif

#pragma warning(pop)

// add manifest for common controls 6
#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
