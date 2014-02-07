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

CString Path_Combine(const CString& cszPath1, const CString& cszPath2)
{
   CString cszPath = cszPath1;

   if (cszPath.Right(1) != _T("\\"))
      cszPath += _T("\\");

   cszPath += cszPath2;

   return cszPath;
}

CString Path_GetDirectoryName(const CString& cszPath)
{
   int iPos = cszPath.ReverseFind(_T('\\'));
   if (iPos == 0)
      return CString();

   return cszPath.Left(iPos+1);
}

bool Directory_Exists(const CString& cszPath)
{
   DWORD dwAttr = GetFileAttributes(cszPath);
   return (dwAttr != INVALID_FILE_ATTRIBUTES) && ((dwAttr & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

bool File_Exists(const CString& cszPath)
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

CString App_GetAppDataFolder(bool bMachineWide)
{
   CString cszAppData;

   // CSIDL_APPDATA - user-dependent app data folder
   // CSIDL_COMMON_APPDATA - machine-wide app data folder
   SHGetFolderPath(NULL,
      bMachineWide ? CSIDL_COMMON_APPDATA : CSIDL_APPDATA,
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
