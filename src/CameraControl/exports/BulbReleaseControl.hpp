//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file BulbReleaseControl.hpp Canon control - Release control for Bulb mode
//
#pragma once

// includes

/// \brief bulb shutter release control
/// this object is created by RemoteReleaseControl::StartBulb; just destroy it to stop bulb mode
class BulbReleaseControl
{
public:
   /// dtor
   virtual ~BulbReleaseControl() {}

   /// returns elapsed time, in seconds, since bulb start
   virtual double ElapsedTime() const = 0;

   /// stops bulb method; can be used when the shared_ptr of BulbReleaseControl
   /// cannot be destroyed, e.g. since it is held somewhere (e.g. Lua)
   virtual void Stop() = 0;
};
