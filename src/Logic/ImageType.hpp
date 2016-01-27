//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file ImageType.hpp Image type enum
//
#pragma once

/// image type
enum T_enImageType
{
   imageTypeNormal = 0, ///< normal image
   imageTypeHDR = 1,    ///< image of a HDR bracketing series
   imageTypePano = 2,   ///< image of a panorama take
   imageTypeHDRPano = 3,   ///< image of a HDR panorama bracketing series
   imageTypeTimelapse = 4,    ///< image of a timelapse series
   imageTypePhotostack = 5,   ///< image of a photostack series
};
