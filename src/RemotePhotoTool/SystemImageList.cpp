//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file SystemImageList.cpp System image list
//
#include "stdafx.h"
#include "SystemImageList.hpp"

CImageList SystemImageList::Get(bool smallIcons)
{
   DWORD flags = SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX;

   if (smallIcons)
      flags |= SHGFI_SMALLICON;

   // Load the image list - use an arbitrary file extension for the
   // call to SHGetFileInfo (we don't want to touch the disk, so use
   // FILE_ATTRIBUTE_NORMAL && SHGFI_USEFILEATTRIBUTES).
   SHFILEINFO sfi = { 0 };
   HIMAGELIST hImageList = (HIMAGELIST)SHGetFileInfo(_T(".txt"),
      FILE_ATTRIBUTE_NORMAL,
      &sfi, sizeof(SHFILEINFO),
      flags);

   CImageList imageList(hImageList);

   // make the background color transparent
   imageList.SetBkColor(CLR_NONE);

   return imageList;
}

int SystemImageList::IndexFromFilename(LPCTSTR filename)
{
   SHFILEINFO shfi = {};
   SHGetFileInfo(
      filename,
      0,
      &shfi,
      sizeof(shfi),
      SHGFI_SYSICONINDEX | SHGFI_DISPLAYNAME | SHGFI_TYPENAME);

   return shfi.iIcon;
}
