//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file ImageFileManager.hpp Image file manager
//
#pragma once

// includes
#include "AppSettings.hpp"
#include "ImageType.hpp"

/// image file manager
class ImageFileManager
{
public:
   /// ctor
   ImageFileManager(AppSettings& settings);

   /// returns next filename for given image type
   CString NextFilename(T_enImageType enImageType, bool bStartNewSeries = false);

   /// returns next filename for image type and given date
   CString NextFilename(T_enImageType enImageType, time_t time, bool bStartNewSeries = false);

private:
   /// adds date to path
   void AddDate(CString& cszPath, time_t time);

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
