//
// ulib - a collection of useful classes
// Copyright (C) 2006,2007,2008,2009,2012,2014 Michael Fink
//
/// \file Atl.hpp configuration for ATL 7.x or higher
//
#pragma once

// needed includes

// exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

// no min-max macors, we use std::min / std::max instead
#define NOMINMAX

// ignore prefast warnings in ATL header files
#pragma prefast(push)
#pragma prefast(disable: 6001 6011 6101 6386 6387 6506 6509 6518 28251 28252 28253 28302)

// ATL includes
#include <atlbase.h>
#include <atlstr.h>
#define _WTL_NO_CSTRING // don't use WTL CString
#include <atltypes.h>
#include <atlwin.h>

#pragma prefast(pop)

// undef the macros so that std::min and std::max work as they should be
#undef min
#undef max

// link to static ATL libs
#ifdef _DEBUG
   #pragma comment(lib, "atlsd.lib")
#else
   #pragma comment(lib, "atls.lib")
#endif
#pragma comment(lib, "atlthunk.lib")
