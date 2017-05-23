//
// ulib - a collection of useful classes
// Copyright (C) 2006,2007,2008,2009,2012,2014,2015 Michael Fink
//
/// \file Atl.hpp configuration for ATL library
//
#pragma once

// needed includes

/// exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
/// exclude more stuff from Visual C++ headers
#define VC_EXTRALEAN
/// prevent ATL using stuff removed by WIN32_LEAN_AND_MEAN
#define ATL_NO_LEAN_AND_MEAN

/// no min-max macros, we use std::min / std::max instead
#define NOMINMAX

// min and max macros are used in atlcom.h, etc., so temporarily define them here
#ifndef min
#  define min(x,y) (x) < (y) ? (x) : (y) ///< temporary define of min()
#endif

#ifndef max
#  define max(x,y) (x) > (y) ? (x) : (y) ///< temporary define of min()
#endif

// ATL includes
#include <atlbase.h>
#include <atlstr.h>
#define _WTL_NO_CSTRING ///< don't use WTL CString
#define _WTL_NO_WTYPES ///< don't use WTL types, such as CSize, CRect, etc.
#include <atltypes.h>
#include <atlwin.h>

// for _stdcallthunk
#include <atlstdthunk.h>

// undef the macros so that std::min and std::max work as they should be
#undef min
#undef max
