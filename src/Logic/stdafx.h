//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file Logic\stdafx.h Precompiled header support
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
