//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file BulbReleaseControl.hpp Canon control - Release control for Bulb mode
//
#pragma once

// includes

/// \brief bulb shutter release control
/// this object is created by RemoteReleaseControl::StartBulb; just destroy it to stop bulb mode
class BulbReleaseControl
{
public:
   /// dtor
   virtual ~BulbReleaseControl() throw() {}

   /// returns elapsed time since bulb start
   virtual double ElapsedTime() const throw() = 0;
};
