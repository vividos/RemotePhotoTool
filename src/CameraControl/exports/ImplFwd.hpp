//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
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
   variantWia,
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
class PropertyAccess;
class RemoteReleaseControlImpl;
}

namespace GPhoto2
{
   class SourceDeviceImpl;
   class PropertyAccess;
   class RemoteReleaseControlImpl;
}

namespace WIA
{
   class SourceDeviceImpl;
   class RemoteReleaseControlImpl;
}

// forward references when compiling with C++/CLI
namespace RemotePhotoTool
{
   namespace CameraControl
   {
#ifdef _MANAGED
      ref class DeviceProperty;
#else
      class DeviceProperty;
#endif
   }
}
