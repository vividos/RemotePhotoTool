/** \file
* \brief libtool dynamic loading library implementation (part)
*
* \author Copyright 2016 Michael Fink <vividos@users.sf.net>
*
* \note
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* \note
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* \note
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA  02110-1301  USA
*/

#include "ltdl.h"
#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>

#pragma comment(lib, "shlwapi.lib")

char userDefinedSearchDir[1024];

int lt_dlinit()
{
   userDefinedSearchDir[0] = 0;
   return 0;
}

int lt_dlexit()
{
   return 0;
}

static void canonicalize_path(const char pathName[MAX_PATH], char absolutePathName[MAX_PATH])
{
   if (TRUE == PathIsRelativeA(pathName))
   {
      GetModuleFileName(NULL, absolutePathName, MAX_PATH);

      PathRemoveFileSpecA(absolutePathName);

      PathAppendA(absolutePathName, pathName);
   }
   else
      strncpy(absolutePathName, pathName, sizeof(absolutePathName));
}

int lt_dladdsearchdir(const char* searchDir)
{
   char absolutePathName[MAX_PATH];

   canonicalize_path(searchDir, absolutePathName);

   if (strlen(userDefinedSearchDir) > 0)
      strncat(userDefinedSearchDir, ":", sizeof(userDefinedSearchDir));

   strncat(userDefinedSearchDir, absolutePathName, sizeof(userDefinedSearchDir));

   return 0;
}

int lt_dlforeachfile(
   const char* searchPath,
   int(*func) (const char* filename, lt_ptr data),
   lt_ptr data)
{
   char fullSearchPath[MAX_PATH];
   canonicalize_path(searchPath, fullSearchPath);

   char searchSpec[MAX_PATH];
   _snprintf(searchSpec, MAX_PATH, "%s\\*.dll", fullSearchPath);

   WIN32_FIND_DATA findData = { 0 };

   HANDLE hFind = FindFirstFile(searchSpec, &findData);
   if (hFind == INVALID_HANDLE_VALUE)
      return -1;

   do
   {
      if (strcmp(findData.cFileName, ".") == 0 ||
         strcmp(findData.cFileName, "..") == 0)
         continue;

      char completeFilename[MAX_PATH];
      _snprintf(completeFilename, MAX_PATH, "%s\\%s", fullSearchPath, findData.cFileName);

      int iRet = func(completeFilename, data);
      if (iRet == -1)
         break;

   } while (FALSE != FindNextFile(hFind, &findData));

   BOOL bRet = FindClose(hFind);

   return bRet != FALSE ? 0 : -1;
}

lt_dlhandle lt_dlopenext(const char* filename)
{
   UINT prevErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);

   SetDllDirectory(userDefinedSearchDir);

   lt_dlhandle handle = (lt_dlhandle)LoadLibrary(filename);

   SetDllDirectory(NULL);

   SetErrorMode(prevErrorMode);

   return handle;
}

lt_ptr lt_dlsym(lt_dlhandle handle, const char* name)
{
   return GetProcAddress((HMODULE)handle, name);
}

const char* lt_dlerror()
{
   static char buffer[1024];

   DWORD dwError = GetLastError();

   if (dwError == 0)
      buffer[0] = 0;
   else
      _snprintf(buffer, sizeof(buffer)/sizeof(*buffer), "error: %u", dwError);

   return buffer;
}

int lt_dlclose(lt_dlhandle handle)
{
   BOOL bRet = FreeLibrary((HMODULE)handle);
   return bRet != FALSE ? 0 : -1;
}
