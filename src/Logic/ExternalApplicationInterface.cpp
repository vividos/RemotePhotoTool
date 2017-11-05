//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file ExternalApplicationInterface.cpp Interface to external applications
//
#include "stdafx.h"
#include "ExternalApplicationInterface.hpp"
#include <ulib/win32/Process.hpp>

void ExternalApplicationInterface::RunApplication(const CString& applicationName,
   const CString& workingDirectory,
   const std::vector<CString>& imageFilenamesList)
{
   CString commandLine = applicationName;

   CString filenames;
   for (size_t i = 0, iMax = imageFilenamesList.size(); i<iMax; i++)
   {
      filenames = imageFilenamesList[i];
      filenames.TrimLeft(_T('\"'));
      filenames.TrimRight(_T('\"'));

      commandLine += _T(" \"");
      commandLine += filenames;
      commandLine += _T("\"");
   }

   Win32::Process process;
   process.WorkingDirectory(workingDirectory);
   ATLVERIFY(true == process.Create(commandLine));
}
