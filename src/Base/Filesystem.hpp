//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file Filesystem.hpp File system functions
//
#pragma once

// includes
#include <vector>

/// combines two path parts
CString Path_Combine(const CString& cszPath1, const CString& cszPath2) throw();

/// returns directory part of path (without filename)
CString Path_GetDirectoryName(const CString& cszPath) throw();

/// checks if a directory exists
bool Directory_Exists(const CString& cszPath) throw();

/// checks if a file exists
bool File_Exists(const CString& cszPath) throw();

/// returns current app filename
CString App_GetFilename() throw();

/// returns app data folder
CString App_GetAppDataFolder(bool bMachineWide) throw();

/// gets all files and folders in a given path
std::vector<CString> FindAllInPath(const CString& cszPath, const CString& cszFileSpec, bool bFindFolders, bool bRecursive) throw();
