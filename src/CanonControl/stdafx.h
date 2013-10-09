//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file CanonControl\stdafx.h Precompiled header support
//
#pragma once

// compile for Windows XP target
#include <WinSDKVer.h>
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <SDKDDKVer.h>

// includes
#include "Atl.hpp"

// Standard C++ Library includes
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <functional>

#include "Logging.hpp"
