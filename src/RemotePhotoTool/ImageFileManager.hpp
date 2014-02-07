//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ImageFileManager.hpp Image file manager
//
#pragma once

// includes
#include "AppSettings.hpp"

/// image file manager image type
enum T_enImageType
{
   imageTypeNormal = 0, ///< normal image
   imageTypeHDR = 1,    ///< image of a HDR bracketing series
   imageTypePano = 2,   ///< image of a panorama take
   imageTypeHDRPano = 3,   ///< image of a HDR panorama bracketing series
   imageTypeTimelapse = 4,    ///< image of a timelapse series
   imageTypePhotostack = 5,   ///< image of a photostack series
};

/// image file manager
class ImageFileManager
{
public:
   /// ctor
   ImageFileManager(AppSettings& settings);

   /// returns next filename for given image type
   CString NextFilename(T_enImageType enImageType, bool bStartNewSeries = false);

private:
   /// adds current date to path
   void AddCurrentDate(CString& cszPath);

   /// adds image type path
   void AddImageTypePath(CString& cszPath, T_enImageType enImageType, bool bStartNewSeries);

   /// formats numbered image path
   void FormatNumberedImagePath(const CString& cszPath, CString& cszSubfolder, bool bStartNewSeries);

   /// finds last used filename image index
   void FindLastUsedFilename();

private:
   /// app settings (copy)
   AppSettings m_settings;

   /// next image index
   unsigned int m_uiNextImageIndex;
};
