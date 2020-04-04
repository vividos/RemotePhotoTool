//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ShutterSpeedValue.hpp Canon control - Shutter speed value class
//
#pragma once

// includes
#include "ImageProperty.hpp"

// forward references
class RemoteReleaseControl;

/// represents a shutter speed value
class ShutterSpeedValue
{
public:
   /// ctor
   ShutterSpeedValue(ImageProperty& value, std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
      :m_value(value),
       m_spRemoteReleaseControl(spRemoteReleaseControl)
   {
   }

   /// subtracts 2 EV, resulting in a faster shutter speed, making the image darker
   void Subtract2EV();

   /// adds 2 EV, resulting in a slower shutter speed, making the image lighter
   void Add2EV();

   /// returns current image property for given shutter speed value
   ImageProperty Value() const { return m_value; }

   /// inequality operator
   bool operator!=(const ShutterSpeedValue& rhs) const
   {
      return RawValue() != rhs.RawValue();
   }

   /// equality operator
   bool operator==(const ShutterSpeedValue& rhs) const
   {
      return RawValue() == rhs.RawValue();
   }

private:
   /// returns raw value
   unsigned int RawValue() const;

   /// sets new raw value and checks if value is available in camera
   void SetAndCheckNewRaw(unsigned int uiValue);

private:
   /// current shutter speed image property
   ImageProperty m_value;

   /// remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;
};
