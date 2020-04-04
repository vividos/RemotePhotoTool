//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file ImageFormat.hpp Canon control - Image format
//
#pragma once

// includes
#include "ImageProperty.hpp"

/// image format
class ImageFormat
{
public:
   /// ctor
   ImageFormat(ImageProperty& value)
      :m_value(value)
   {
   }
   /// dtor
   ~ImageFormat() {}

   /// converts image format to string
   CString ToString() const;

private:
   /// image property
   ImageProperty m_value;
};
