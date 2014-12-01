//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file Filesystem.cpp File system functions
//

// includes
#include "stdafx.h"
#include "Filesystem.hpp"
#include "FileFinder.hpp"
#include <ShlObj.h>

bool Directory_Exists(const CString& cszPath) throw()
{
   DWORD dwAttr = GetFileAttributes(cszPath);
   return (dwAttr != INVALID_FILE_ATTRIBUTES) && ((dwAttr & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

bool File_Exists(const CString& cszPath) throw()
{
   DWORD dwAttr = GetFileAttributes(cszPath);
   return (dwAttr != INVALID_FILE_ATTRIBUTES) && ((dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0);
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

   CString cszAppData;

   SHGetFolderPath(NULL,
      int(enAppDataFolderType),
      NULL, SHGFP_TYPE_CURRENT,
      cszAppData.GetBuffer(MAX_PATH));
   cszAppData.ReleaseBuffer();

   return cszAppData;
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
