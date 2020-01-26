//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file FfmpegInterface.cpp Interface to ffmpeg movie creation software
//
#include "stdafx.h"
#include "FfmpegInterface.hpp"
#include <ulib/Path.hpp>
#include <ulib/win32/Process.hpp>

FfmpegInterface::FfmpegInterface(const CString& binPath)
   :m_binPath(binPath)
{
}

bool FfmpegInterface::IsInstalled() const
{
   CString exeFilename = Path::Combine(m_binPath, _T("ffmpeg.exe"));

   return Path::FileExists(exeFilename);
}

void FfmpegInterface::Run(const std::vector<CString>& imageFilenamesList, const CString& extraCommandLine, const CString& outputFilename)
{
   CString commandLine = CString(_T("\"")) + CString(Path::Combine(m_binPath, _T("ffmpeg.exe"))) + _T("\" ");

   commandLine += extraCommandLine + _T(" ");

   CString filenames;
   for (size_t i = 0, iMax = imageFilenamesList.size(); i<iMax; i++)
   {
      filenames = imageFilenamesList[i];
      filenames.TrimLeft(_T('\"'));
      filenames.TrimRight(_T('\"'));

      commandLine += _T("-i \"");
      commandLine += filenames;
      commandLine += _T("\" ");
   }

   commandLine += _T("\"");
   commandLine += outputFilename;
   commandLine += _T("\" ");

   Win32::Process process;
   process.WorkingDirectory(m_binPath);
   ATLVERIFY(true == process.Create(commandLine));
}
