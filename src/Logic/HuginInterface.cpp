//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file HuginInterface.cpp Interface to Hugin panorama stitching software
//

// includes
#include "StdAfx.h"
#include "HuginInterface.hpp"
#include <ulib/Path.hpp>
#include <ulib/win32/Process.hpp>
#include <array>
#include <tuple>

HuginInterface::HuginInterface(const CString& cszInstallPath)
:m_cszInstallPath(cszInstallPath)
{
   if (cszInstallPath.IsEmpty())
      Detect();
}

void HuginInterface::RunUI(const std::vector<CString>& vecImageFilenames)
{
   CString cszCommandLine = Path::Combine(m_cszInstallPath, _T("hugin.exe"));

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
   ATLVERIFY(true == process.Create(cszCommandLine));
}

void HuginInterface::RunStitcher(const CString& cszPtoScript, const CString& cszOutputFile)
{
   CString cszCommandLine = Path::Combine(m_cszInstallPath, _T("nona.exe"));

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

   Win32::Process process;
   process.WorkingDirectory(m_cszInstallPath);
   ATLVERIFY(true == process.Create(cszCommandLine));
}

void HuginInterface::Detect()
{
   std::array<std::tuple<HKEY, LPCTSTR, LPCTSTR>, 2> regKeys
   {
      std::tuple<HKEY, LPCTSTR, LPCTSTR> {
         HKEY_CURRENT_USER, _T("Software\\hugin"), _T("startDir")
      },
      std::tuple<HKEY, LPCTSTR, LPCTSTR> {
         HKEY_LOCAL_MACHINE, _T("Software\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Hugin"), _T("DisplayIcon")
      }
      //HKEY_CLASSES_ROOT\Applications\hugin.exe\shell\open\command
   };

   CString cszInstallPath;
   for (int i=0, iMax=regKeys.size(); i<iMax; i++)
   {
      CRegKey regKey;
      if (ERROR_SUCCESS != regKey.Open(std::get<0>(regKeys[i]), std::get<1>(regKeys[i]), KEY_READ))
         continue;

      DWORD dwLength = MAX_PATH;
      LPTSTR pszBuffer = cszInstallPath.GetBuffer(MAX_PATH);
      regKey.QueryStringValue(std::get<2>(regKeys[i]), pszBuffer, &dwLength);
      cszInstallPath.ReleaseBuffer();

      if (!cszInstallPath.IsEmpty())
      {
         CString cszInstallPathLower(cszInstallPath);
         cszInstallPathLower.MakeLower();

         if (cszInstallPathLower.Find(_T(".exe")) != -1)
            cszInstallPath = Path(cszInstallPath).FolderName();

         Path::AddEndingBackslash(cszInstallPath);

         break;
      }
   }

   if (!cszInstallPath.IsEmpty())
      m_cszInstallPath = cszInstallPath;
}
