//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file RemotePhotoTool\stdafx.h Precompiled header support
//
#pragma once

/// compile for Windows 7 target to have Ribbon code available
#define _WIN32_WINNT _WIN32_WINNT_WIN7

// WTL includes
#include "Wtl.hpp"

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

// Boost includes
#include <boost/noncopyable.hpp>

// CanonControl includes
#include "Instance.hpp"
#include "Variant.hpp"
#include "CameraException.hpp"
#include "SourceInfo.hpp"
#include "SourceDevice.hpp"
#include "RemoteReleaseControl.hpp"
#include "Viewfinder.hpp"
#include "ImageProperty.hpp"
#include "DeviceProperty.hpp"

// app wide window message

/// sent when viewfinder image is available
#define WM_VIEWFINDER_AVAIL_IMAGE WM_APP+1

/// sent when property has changed; wParam has image property id
#define WM_RELEASECONTROL_PROPERTY_CHANGED WM_APP+2

/// sent when all properties have changed
#define WM_RELEASECONTROL_ALL_PROPERTIES_CHANGED WM_APP+3

/// sent to lock or unlock action mode
#define WM_LOCK_ACTIONMODE WM_APP+4

/// sent to PreviousImagesView to update current image
#define WM_PREV_IMAGES_UPDATE WM_APP + 5
