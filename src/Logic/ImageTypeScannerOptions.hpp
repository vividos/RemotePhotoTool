//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file ImageTypeScannerOptions.hpp Options for ImageTypeScanner
//
#pragma once

/// Options for ImageTypeScanner class
class ImageTypeScannerOptions
{
public:
   /// default ctor
   ImageTypeScannerOptions()
      :m_minimumNumberOfPanoramaImages(3),
      m_secondsBetweenPanoramaImages(10),
      m_secondsBetweenHDRImages(10)
   {
   }

   /// returns minimum number of images to be considered as panorama
   unsigned int MinimumNumberOfPanoramaImages() const { return m_minimumNumberOfPanoramaImages; }

   /// returns number of seconds that may lie between two panorama images
   unsigned int SecondsBetweenPanoramaImages() const { return m_secondsBetweenPanoramaImages; }

   /// returns number of seconds that may lie between two HDR images
   unsigned int SecondsBetweenHDRImages() const { return m_secondsBetweenHDRImages; }

private:
   /// minimum number of images to be considered as panorama
   unsigned int m_minimumNumberOfPanoramaImages;

   /// number of seconds that may lie between two panorama images
   unsigned int m_secondsBetweenPanoramaImages;

   /// number of seconds that may lie between two HDR images
   unsigned int m_secondsBetweenHDRImages;
};
