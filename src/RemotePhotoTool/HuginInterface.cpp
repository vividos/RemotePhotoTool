//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file HuginInterface.cpp Interface to Hugin panorama stitching software
//

// includes
#include "StdAfx.h"
#include "HuginInterface.hpp"
#include "Filesystem.hpp"
#include "Process.hpp"

HuginInterface::HuginInterface(const CString& cszInstallPath)
:m_cszInstallPath(cszInstallPath)
{
   if (cszInstallPath.IsEmpty())
      Detect();
}

void HuginInterface::RunUI(const std::vector<CString>& vecImageFilenames)
{
   CString cszCommandLine = Path_Combine(m_cszInstallPath, _T("hugin.exe"));

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

   Process process;
   process.WorkingDirectory(m_cszInstallPath);
   bool bRet = process.Create(cszCommandLine);

   ATLASSERT(true == bRet); bRet;
}

void HuginInterface::RunStitcher(const CString& cszPtoScript, const CString& cszOutputFile)
{
   CString cszCommandLine = Path_Combine(m_cszInstallPath, _T("nona.exe"));

   CString cszFilename(cszOutputFile);
   cszFilename.TrimLeft(_T('\"'));
   cszFilename.TrimRight(_T('\"'));

   cszCommandLine += _T("-o \"");
   cszCommandLine += cszFilename;
   cszCommandLine += _T("\" ");

   cszFilename = cszPtoScript;
   cszFilename.TrimLeft(_T('\"'));
   cszFilename.TrimRight(_T('\"'));

   cszCommandLine += _T("\"");
   cszCommandLine += cszFilename;
   cszCommandLine += _T("\"");

   Process process;
   process.WorkingDirectory(m_cszInstallPath);
   bool bRet = process.Create(cszCommandLine);

   ATLASSERT(true == bRet); bRet;
}

void HuginInterface::Detect()
{
   LPCTSTR apszRegKeys[] =
   {
      _T("Software\\hugin"), _T("startDir"),
   };

   CString cszInstallPath;
   for (int i=0, iMax=sizeof(apszRegKeys)/sizeof(*apszRegKeys); i<iMax; i+=2)
   {
      CRegKey regKey;
      if (ERROR_SUCCESS != regKey.Open(HKEY_CURRENT_USER, apszRegKeys[i+0], KEY_READ))
         continue;

      DWORD dwLength = MAX_PATH;
      LPTSTR pszBuffer = cszInstallPath.GetBuffer(MAX_PATH);
      regKey.QueryStringValue(apszRegKeys[i+1], pszBuffer, &dwLength);
      cszInstallPath.ReleaseBuffer();

      if (!cszInstallPath.IsEmpty())
      {
         if (cszInstallPath.Right(1) != _T("\\"))
            cszInstallPath += _T("\\");

         break;
      }
   }

   if (!cszInstallPath.IsEmpty())
      m_cszInstallPath = cszInstallPath;
}
