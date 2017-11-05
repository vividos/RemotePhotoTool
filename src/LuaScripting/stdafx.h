//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file LuaScripting\stdafx.h Precompiled header support
//
#pragma once

// includes
#include <SDKDDKVer.h>
#include <ulib/config/BoostAsio.hpp>
#include <ulib/config/Common.hpp>
#include <ulib/config/Atl.hpp>

// redefine ATLVERIFY when analyzing using Coverity Scan
#if !defined(_DEBUG) && defined(__COVERITY__)
#undef ATLVERIFY
#define ATLVERIFY(expr) (void)(expr)
#endif

// Standard C++ Library includes
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <functional>
