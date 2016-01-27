//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file ImageTypeFilesList.hpp Collection of files of one image type
//
#pragma once

/// includes
#include <vector>
#include "ImageType.hpp"
#include "ImageFileInfo.hpp"

/// \brief Collection of files of one image type
/// \details Rpresents a list of files that belongs to a single image type, e.g. a
/// collection of HDR or panorama images.
class ImageTypeFilesList
{
public:
   /// ctor; constructs empty list with given type
   ImageTypeFilesList(T_enImageType imageType = imageTypeNormal)
      :m_imageType(imageType)
   {
   }

   /// returns image type of the list of files
   T_enImageType ImageType() const throw() { return m_imageType; }

   /// returns list of files belonging to this image type; const version
   const std::vector<ImageFileInfo>& ImageFileInfoList() const throw() { return m_imageFileInfoList; }

   /// returns list of files belonging to this image type; non-const version
   std::vector<ImageFileInfo>& ImageFileInfoList() throw() { return m_imageFileInfoList; }

private:
   /// image type of the list of files
   T_enImageType m_imageType;

   /// list of files belonging to this image type
   std::vector<ImageFileInfo> m_imageFileInfoList;
};
