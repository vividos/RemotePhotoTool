//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file PhotomatixInterface.cpp Interface to Photomatix HDR image creation software
//

// includes
#include "StdAfx.h"
#include "PhotomatixInterface.hpp"
#include <ulib/Path.hpp>

PhotomatixInterface::PhotomatixInterface(const CString& cszInstallPath)
:m_cszInstallPath(cszInstallPath)
{
   if (cszInstallPath.IsEmpty())
      Detect();
}

void PhotomatixInterface::RunUI(const std::vector<CString>& imageFilenamesList)
{
   RunApplication(
      Path::Combine(m_cszInstallPath, _T("PhotomatixPro.exe")),
      m_cszInstallPath,
      imageFilenamesList);
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
