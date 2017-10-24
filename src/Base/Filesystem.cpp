//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file Filesystem.cpp File system functions
//

// includes
#include "stdafx.h"
#include "Filesystem.hpp"
#include <ulib/FileFinder.hpp>
#include <ulib/Path.hpp>

bool Directory_Exists(const CString& cszPath) throw()
{
   return Path(cszPath).FolderExists();
}

CString App_GetFilename()
{
   CString cszFilename;

   GetModuleFileName(NULL, cszFilename.GetBuffer(MAX_PATH), MAX_PATH);
   cszFilename.ReleaseBuffer();

   return cszFilename;
}
