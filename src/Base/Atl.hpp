//
// ulib - a collection of useful classes
// Copyright (C) 2006,2007,2008,2009,2012,2014,2015 Michael Fink
//
/// \file Atl.hpp configuration for ATL library
//
#pragma once

// needed includes

// exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define ATL_NO_LEAN_AND_MEAN

// no min-max macros, we use std::min / std::max instead
#define NOMINMAX

// ATL includes
#include <atlbase.h>
#include <atlstr.h>
#define _WTL_NO_CSTRING // don't use WTL CString
#define _WTL_NO_WTYPES
#include <atltypes.h>
#include <atlwin.h>

#if (_ATL_VER < 0x0800)
#error ATL 8.0 or higher is needed!
#endif

// for _stdcallthunk
#include <atlstdthunk.h>
