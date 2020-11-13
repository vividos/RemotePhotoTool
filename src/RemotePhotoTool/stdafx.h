//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file RemotePhotoTool\stdafx.h Precompiled header support
//
#pragma once

// includes
#include <SDKDDKVer.h>
#include <ulib/config/BoostAsio.hpp>

// WTL includes
#include <ulib/config/Common.hpp>
#include <ulib/config/Wtl.hpp>

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
#include <algorithm>
#include <thread>
#include <atomic>

// CameraControl includes
#include "Instance.hpp"
#include "Variant.hpp"
#include "CameraException.hpp"
#include "SourceInfo.hpp"
#include "SourceDevice.hpp"
#include "RemoteReleaseControl.hpp"
#include "Viewfinder.hpp"
#include "ImageProperty.hpp"
#include "DeviceProperty.hpp"

#include "WindowMessages.hpp"
