//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file ImplFwd.hpp Canon control - Implementation forward declarations
//
#pragma once

// includes

/// SDK variant
enum T_enSDKVariant
{
   variantCdsdk,
   variantEdsdk,
   variantPsrec,
   variantGphoto2,
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

namespace GPhoto2
{
   class SourceDeviceImpl;
   class PropertyAccess;
   class RemoteReleaseControlImpl;
}
