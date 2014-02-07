//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ImageProperty.hpp Canon control - Image property
//
#pragma once

// includes
#include "Variant.hpp"
#include "ImplFwd.hpp"

/// generic image property type
/// may be translated to actual ImageProperty by SDK, or property might not even exist for some models
enum T_enImagePropertyType
{
   propShootingMode = 0, ///< current shooting mode (also called AE mode); set only when capChangeShootingMode is true
   propDriveMode = 1, ///< drive mode (e.g. single shot, high speed, etc.)
   propISOSpeed = 2, ///< ISO speed
   propMeteringMode = 3, ///< metering mode (e.g. center-weight, ...)
   propAFMode = 4, ///< autofocus mode (?)
   propAv = 5, ///< Av value; note that available Av values can depend on shooting mode, Tv value and zoom factor
   propTv = 6, ///< Tv value; note that available Tv values can depend shooting mode, Av value and zoom factor
   propExposureCompensation = 7, ///< -2..2
   propFlashExposureComp = 8,
   propFocalLength = 9,
   propFlashMode = 10, ///< flash mode on/off/? etc.
   propWhiteBalance = 11, ///< white balance setting
   propAFDistance = 12,
   propCurrentZoomPos = 13,     ///< current zoom pos; get/set only when capZoomControl is true
   propMaxZoomPos = 14,         ///< max. zoom position; get only; min. zoom position is 0
   propDigitalZoomMagnification = 15,
   propAvailableShots = 16, ///< number of available shots
   propSaveTo = 17,
   propBatteryLevel = 18,

   propUnknown = 100
};


/// image property; used during remote camera control
class ImageProperty
{
public:
   /// image property id
   unsigned int Id() const throw() { return m_uiImageProperty; }

   /// property name
   CString Name() const throw();

   /// property value as string
   CString AsString() const throw();

   /// value as variant
   Variant Value() const { return m_value; }

   /// returns if image property is read only
   bool IsReadOnly() const { return m_bReadOnly; }

   /// converts a given value (e.g. from ValidValues()) to string
   CString ValueAsString(Variant value) const throw();

   /// inequality operator
   bool operator!=(const ImageProperty& rhs) const
   {
      return !this->operator==(rhs);
   }

   /// equality operator
   bool operator==(const ImageProperty& rhs) const
   {
      return Id() == rhs.Id() &&
         AsString() == rhs.AsString();
   }

private:
   friend EDSDK::RemoteReleaseControlImpl;
   friend CDSDK::RemoteReleaseControlImpl;
   friend PSREC::RemoteReleaseControlImpl;
   friend class RemoteReleaseControl;
   friend class ShutterSpeedValue;
   friend class ImageFormat;

   /// ctor
   ImageProperty(T_enSDKVariant enSDKVariant, unsigned int uiImageProperty, Variant value, bool bReadOnly)
      :m_enSDKVariant(enSDKVariant),
       m_uiImageProperty(uiImageProperty),
       m_value(value),
       m_bReadOnly(bReadOnly)
   {
   }

   /// sets new value
   void SetValue(const Variant& value)
   {
      m_value = value;
   }

private:
   /// SDK variant
   T_enSDKVariant m_enSDKVariant;

   /// image property
   unsigned int m_uiImageProperty;

   /// value
   Variant m_value;

   /// indicates if value is read only
   bool m_bReadOnly;
};
