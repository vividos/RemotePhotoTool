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

bool File_Exists(const CString& cszPath) throw()
{
   return Path(cszPath).FileExists();
}

CString App_GetFilename()
{
   CString cszFilename;

   GetModuleFileName(NULL, cszFilename.GetBuffer(MAX_PATH), MAX_PATH);
   cszFilename.ReleaseBuffer();

   return cszFilename;
}

CString App_GetAppDataFolder(T_enAppDataFolderType enAppDataFolderType)
{
   ATLASSERT(enAppDataFolderType == CSIDL_COMMON_APPDATA ||
      enAppDataFolderType == CSIDL_LOCAL_APPDATA ||
      enAppDataFolderType == CSIDL_APPDATA);

   return Path::SpecialFolder(int(enAppDataFolderType));
}

std::vector<CString> FindAllInPath(const CString& cszPath, const CString& cszFileSpec, bool bFindFolders, bool bRecursive)
{
   std::vector<CString> vecFilenames;

   FileFinder finder(cszPath, cszFileSpec);
   if (finder.IsValid())
   {
      do
      {
         if (finder.IsDot())
            continue;

         if (bRecursive && finder.IsFolder())
         {
            std::vector<CString> vecSubfolderFilenames =
               FindAllInPath(finder.Filename(), cszFileSpec, bFindFolders, true);

            vecFilenames.insert(vecFilenames.end(), vecSubfolderFilenames.begin(), vecSubfolderFilenames.end());
         }

         if (bFindFolders && finder.IsFile())
            continue;

         if (!bFindFolders && finder.IsFolder())
            continue;

         vecFilenames.push_back(finder.Filename());

      } while (finder.Next());
   }

   return vecFilenames;
}
