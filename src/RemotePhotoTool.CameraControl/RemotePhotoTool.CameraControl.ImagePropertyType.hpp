//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.ImagePropertyType.hpp ImagePropertyType enum class
//
#pragma once

namespace RemotePhotoTool
{
   namespace CameraControl
   {
      /// generic image property type
      /// may be translated to actual ImageProperty by SDK, or property might not even exist for some models
      public enum class ImagePropertyType
      {
         ShootingMode = 0, ///< current shooting mode (also called AE mode); set only when capChangeShootingMode is true
         DriveMode = 1, ///< drive mode (e.g. single shot, high speed, etc.)
         ISOSpeed = 2, ///< ISO speed
         MeteringMode = 3, ///< metering mode (e.g. center-weight, ...)
         AFMode = 4, ///< autofocus mode (?)
         Av = 5, ///< Av value; note that available Av values can depend on shooting mode, Tv value and zoom factor
         Tv = 6, ///< Tv value; note that available Tv values can depend shooting mode, Av value and zoom factor
         ExposureCompensation = 7, ///< -2..2
         FlashExposureComp = 8,
         FocalLength = 9,
         FlashMode = 10, ///< flash mode on/off/? etc.
         WhiteBalance = 11, ///< white balance setting
         AFDistance = 12,
         CurrentZoomPos = 13,     ///< current zoom pos; get/set only when capZoomControl is true
         MaxZoomPos = 14,         ///< max. zoom position; get only; min. zoom position is 0
         AvailableShots = 16, ///< number of available shots
         SaveTo = 17,
         BatteryLevel = 18,
         ImageFormat = 19,

         Unknown = 100
      };
   }
}
