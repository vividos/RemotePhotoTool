//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2Include.hpp gPhoto2 - central include file
//
#pragma once

extern "C"
{
#undef interface
#define interface _interface ///< interface is used in gPhoto2 headers
#include <gphoto2/gphoto2.h>
#include <gphoto2/gphoto2-version.h>
#include <gphoto2/gphoto2-abilities-list.h>
#include <gphoto2/gphoto2-camera.h>
#undef interface
#define interface __STRUCT__ ///< re-define interface with previous value
}
