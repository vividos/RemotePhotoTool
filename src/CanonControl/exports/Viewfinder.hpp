//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file Viewfinder.hpp Canon control - Viewfinder
//
#pragma once

// includes
#include <vector>

/// viewfinder class
class Viewfinder
{
public:
   /// dtor
   virtual ~Viewfinder() throw() {}

   /// viewfinder output type
   /// \note this corresponds to the values found in PS-ReC and CDSDK
   enum T_enOutputType
   {
      outputTypeUndefined = 0,   ///< undefined; don't use
      outputTypeLCD = 1,         ///< live viewfinder is also shown on LCD
      outputTypeVideoOut = 2,    ///< live viewfinder is also shown on Video Out
      outputTypeOff = 3,         ///< live viewfinder is only transferred to PC
   };

   /// sets viewfinder output type
   virtual void SetOutputType(T_enOutputType enOutputType) = 0;

   /// callback function type to call when viewfinder image is available
   typedef std::function<void (const std::vector<BYTE>& vecImage)> T_fnOnAvailViewfinderImage;

   /// sets (or resets) viewfinder callback
   virtual void SetAvailImageHandler(T_fnOnAvailViewfinderImage fnOnAvailViewfinderImage = T_fnOnAvailViewfinderImage()) = 0;

   /// closes viewfinder
   virtual void Close() = 0;
};
