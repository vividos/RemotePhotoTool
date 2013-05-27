//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file RemotePhotoTool\stdafx.h Precompiled header support
//
#pragma once

// compile for Windows 7 target to have Ribbon code available
#define _WIN32_WINNT _WIN32_WINNT_WIN7

#include "Wtl.hpp"

// additional WTL includes
#define _WTL_NO_WTYPES
#define max(x,y) ((x) > (y) ? (y) : (x))
#include <atlribbon.h>
#undef max

// Standard C++ Library includes
#include <vector>
#include <set>
#include <map>
#include <memory>

// Boost includes
#define BOOST_DATE_TIME_NO_LIB
#include <boost/noncopyable.hpp>

// app wide window message

/// sent when viewfinder image is available
#define WM_VIEWFINDER_AVAIL_IMAGE WM_APP+1

/// sent when property has changed; wParam has image property id
#define WM_RELEASECONTROL_PROPERTY_CHANGED WM_APP+2

/// sent when all properties have changed
#define WM_RELEASECONTROL_ALL_PROPERTIES_CHANGED WM_APP+3
