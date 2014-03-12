//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file RemoteScriptingEditor\stdafx.h Precompiled header support
//
#pragma once

// compile for Windows 7 target to have Ribbon code available
#define _WIN32_WINNT _WIN32_WINNT_WIN7

// WTL includes
#include "Wtl.hpp"
#include <atlsplit.h>

// Scintilla include
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#include <atlscintilla.h>
#include <SciLexer.h>
#pragma warning(pop)

// Standard C++ Library includes
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <functional>
