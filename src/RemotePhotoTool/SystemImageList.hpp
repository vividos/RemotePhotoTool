//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file SystemImageList.hpp System image list
//
#pragma once

/// system image list
class SystemImageList
{
public:
   /// \brief returns the small or large image list
   /// \details be sure to use LVS_SHAREIMAGELISTS flag when using the image list in a
   /// CListViewCtrl
   static CImageList Get(bool smallIcons);

   /// \brief returns an index into the system image list from the given filename
   /// \details note tat the filename doesn't have to exist
   static int IndexFromFilename(LPCTSTR filename);
};
