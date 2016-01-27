//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file ImageTypeScanner.hpp Scanner for image types
//
#pragma once

/// includes
#include <vector>
#include "ImageFileInfo.hpp"
#include "ImageTypeFilesList.hpp"
#include "ImageTypeScannerOptions.hpp"

/// \brief Scanner for image types
/// scans a list of images (or their image file infos) and determines if an image is a single
/// image, belongs to a list of single HDR images or can be classified as a set of images produced
/// by taking panorama or HDR panorama images.
class ImageTypeScanner
{
public:
   /// ctor; takes options
   ImageTypeScanner(ImageTypeScannerOptions& options)
      :m_options(options)
   {
   }

   /// scans images and returns list of image types recognized
   void ScanImages(std::vector<ImageFileInfo>& imageFileList,
      std::vector<ImageTypeFilesList>& imageTypeFilesList);

private:
   /// collects all HDR images in one files list
   void CollectHDRImages(std::vector<ImageFileInfo>& imageFileList,
      ImageTypeFilesList& hdrFilesList);

   /// finds panorama images in list of AEB mode image infos
   void FindAEBHDRImages(const std::vector<ImageFileInfo>& imageFileList, size_t startIndex,
      size_t& lastImageIndex);

   /// finds panorama images in list of non-AEB mode image infos
   void FindNonAEBHDRImages(const std::vector<ImageFileInfo>& imageFileList, size_t startIndex,
      size_t& lastImageIndex);

   /// checks if a given image is taken with same parameters to be considered an HDR image
   bool CheckNonBracketHDRImage(const ImageFileInfo& currentInfo,
      const ImageFileInfo& previousInfo);

   /// collects all Panorama images in separate files list
   void CollectPanoramaImages(std::vector<ImageFileInfo>& imageFileList,
      std::vector<ImageTypeFilesList>& imageTypeFilesList, bool onlyHDRImages);

   /// finds panorama images in list of image infos
   void FindPanoramaImages(const std::vector<ImageFileInfo>& imageFileList, size_t startIndex,
      size_t& lastImageIndex, bool onlyHDRImages);

   /// moves images from source list to destination list
   void MoveImages(std::vector<ImageFileInfo>& sourceImageFileList, size_t startIndex,
      size_t length, std::vector<ImageFileInfo>& destImageFileList);

private:
   /// options for image type scanner
   ImageTypeScannerOptions m_options;
};
