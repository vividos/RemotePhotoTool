//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file Logging.cpp Simple logging for SDK functions
//

// includes
#include "stdafx.h"
#include "Logging.hpp"
#include <ulib/Path.hpp>
#include <ulib/thread/LightweightMutex.hpp>
#include <atomic>
#include <ctime>

/// indicates if logging is active
static std::atomic<bool> s_bLoggingActive = false;

/// log filename
CString s_cszLogFilename;

/// formats current date for logging
static CString LogFormatCurrentDate()
{
   time_t nowtime = time(&nowtime);

   struct tm nowtm = { 0 };
   ATLVERIFY(0 == localtime_s(&nowtm, &nowtime));

   CString cszDate;
   _tcsftime(cszDate.GetBuffer(256), 256, _T("%Y-%m-%d"), &nowtm);
   cszDate.ReleaseBuffer();

   return cszDate;
}

/// formats current date/time for logging
static CString LogGetCurrentDateTime()
{
   time_t now = time(&now);

   struct tm tmNow = { 0 };
   localtime_s(&tmNow, &now);

   CString cszDateTime;
   _tcsftime(cszDateTime.GetBuffer(64), 64, _T("%Y-%m-%dT%H:%M:%S"), &tmNow);
   cszDateTime.ReleaseBuffer();

   return cszDateTime;
}

void LogConfigure(bool bEnable, const CString& cszLogfilePath)
{
   s_bLoggingActive = bEnable;

   if (bEnable)
   try
   {
      CString cszDate = LogFormatCurrentDate();
      s_cszLogFilename = Path::Combine(cszLogfilePath, _T("Log-") + cszDate + _T(".txt"));
   }
   catch (...)
   {
      OutputDebugString(_T("Unknown exception occured during LogConfigure() call\n"));
   }
}

/// write to file
void LogWriteText(const CString& cszText, bool bPreviousIncompleteLine)
{
   FILE* fd = NULL;
   errno_t err = _tfopen_s(&fd, s_cszLogFilename.GetString(), _T("a+t"));
   if (err != 0 || fd == nullptr)
      return;

   if (bPreviousIncompleteLine)
   {
      // append to line
      _ftprintf(fd, _T("%s"), cszText.GetString());
   }
   else
   {
      // output new line

      // get current time
      CString cszDateTime = LogGetCurrentDateTime();

      _ftprintf(fd, _T("%s [%08x] %s"),
         cszDateTime.GetString(),
         GetCurrentThreadId(),
         cszText.GetString());
   }

   fflush(fd);

   fclose(fd);
}

void LogTrace(LPCTSTR pszFormat, ...)
{
#ifndef _DEBUG
   // in release, early-quit here
   if (!s_bLoggingActive)
      return;
#endif

   try
   {
      // format text
      CString cszText;
      {
         va_list args;
         va_start(args, pszFormat);

         cszText.FormatV(pszFormat, args);

         va_end(args);

         cszText.Replace(_T("%"), _T("%%"));
      }

      ATLTRACE(_T("%s"), cszText.GetString());

#ifdef _DEBUG
      // in debug, quit after tracing message
      if (!s_bLoggingActive)
         return;
#endif

      static LightweightMutex s_mtxLogging;
      LightweightMutex::LockType lock(s_mtxLogging);

      static bool s_bPreviousIncompleteLine = false;

      LogWriteText(cszText, s_bPreviousIncompleteLine);

      {
         // check if line is complete
         if (cszText.Right(1) == _T("\n"))
            s_bPreviousIncompleteLine = false;
      }
   }
   catch (...)
   {
      OutputDebugString(_T("Unknown exception occured during LogTrace() call\n"));
   }
}
