//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file PhotomatixInterface.cpp Interface to Photomatix HDR image creation software
//

// includes
#include "StdAfx.h"
#include "PhotomatixInterface.hpp"
#include <ulib/Path.hpp>
#include <ulib/win32/Process.hpp>

PhotomatixInterface::PhotomatixInterface(const CString& cszInstallPath)
:m_cszInstallPath(cszInstallPath)
{
   if (cszInstallPath.IsEmpty())
      Detect();
}

void PhotomatixInterface::RunUI(const std::vector<CString>& vecImageFilenames)
{
   CString cszCommandLine = Path::Combine(m_cszInstallPath, _T("PhotomatixPro.exe"));

   CString cszFilename;
   for (size_t i=0,iMax=vecImageFilenames.size(); i<iMax; i++)
   {
      cszFilename = vecImageFilenames[i];
      cszFilename.TrimLeft(_T('\"'));
      cszFilename.TrimRight(_T('\"'));

      cszCommandLine += _T(" \"");
      cszCommandLine += cszFilename;
      cszCommandLine += _T("\"");
   }

   Win32::Process process;
   process.WorkingDirectory(m_cszInstallPath);
   bool bRet = process.Create(cszCommandLine);

   ATLASSERT(true == bRet); bRet;
}

void PhotomatixInterface::Detect()
{
   LPCTSTR pszRegKeys[] =
   {
      _T("Software\\HDRsoft\\PhotomatixPro\\4.0"),
      _T("Software\\MultimediaPhoto\\Photomatix"),
   };

   CString cszInstallPath;
   for (size_t i=0, iMax=sizeof(pszRegKeys)/sizeof(*pszRegKeys); i<iMax; i++)
   {
      CRegKey regKey;
      if (ERROR_SUCCESS != regKey.Open(HKEY_CURRENT_USER, pszRegKeys[i], KEY_READ))
         continue;

      ULONG nChars = MAX_PATH;
      LPTSTR pszBuffer = cszInstallPath.GetBuffer(nChars+1);
      regKey.QueryStringValue(_T("Path"), pszBuffer, &nChars);

      cszInstallPath.ReleaseBuffer();

      if (cszInstallPath.Right(1) != _T("\\"))
         cszInstallPath += _T("\\");

      break;
   }

   if (!cszInstallPath.IsEmpty())
      m_cszInstallPath = cszInstallPath;
}
