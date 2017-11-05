//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file IImagePropertyControl.hpp Interface for image property controls
//
#pragma once

/// interface for UI controls that represent an image property
class IImagePropertyControl
{
public:
   /// dtor
   virtual ~IImagePropertyControl()
   {
   }

   /// returns property id of image property managed by control
   virtual unsigned int GetPropertyId() = 0;

   /// called to update all possible values of image property
   virtual void UpdateValuesList() = 0;

   /// called to update current value of control
   virtual void UpdateValue() = 0;
};

