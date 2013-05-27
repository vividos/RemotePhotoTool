//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file ImplFwd.hpp Canon control - Implementation forward declarations
//
#pragma once

// includes

/// SDK variant
enum T_enSDKVariant
{
   variantCdsdk,
   variantEdsdk,
   variantPsrec
};

// forward declarations for implementation classes
namespace EDSDK
{
class SourceDeviceImpl;
class RemoteReleaseControlImpl;
class PropertyAccess;
}

namespace CDSDK
{
class SourceDeviceImpl;
class RemoteReleaseControlImpl;
class DevicePropertyAccess;
}

namespace PSREC
{
class SourceDeviceImpl;
class RemoteReleaseControlImpl;
}
