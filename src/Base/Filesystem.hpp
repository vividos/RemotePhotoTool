//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file Filesystem.hpp File system functions
//
#pragma once

// includes
#include <vector>
#include <shlobj.h>

/// combines two path parts
CString Path_Combine(const CString& cszPath1, const CString& cszPath2);

/// returns directory part of path (without filename)
CString Path_GetDirectoryName(const CString& cszPath);

/// checks if a directory exists
bool Directory_Exists(const CString& cszPath) throw();

/// checks if a file exists
bool File_Exists(const CString& cszPath) throw();

/// returns current app filename
CString App_GetFilename();

/// app data folder type
enum T_enAppDataFolderType
{
   appDataAllUsers = CSIDL_COMMON_APPDATA,      ///< `All Users\Application Data`
   appDataUser = CSIDL_APPDATA,                 ///< `<user name>\Application Data`
   appDataUserNonRoaming = CSIDL_LOCAL_APPDATA  ///< `<user name>\Local Settings\Applicaiton Data` (non roaming)
};

/// returns app data folder
CString App_GetAppDataFolder(T_enAppDataFolderType enAppDataFolderType);

/// gets all files and folders in a given path
std::vector<CString> FindAllInPath(const CString& cszPath, const CString& cszFileSpec, bool bFindFolders, bool bRecursive);
