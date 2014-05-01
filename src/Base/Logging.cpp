//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file Logging.cpp Simple logging for SDK functions
//

// includes
#include "stdafx.h"
#include "Logging.hpp"
#include "Filesystem.hpp"
#include "LightweightMutex.hpp"
#include <atomic>
#include <ctime>
//#include <boost/log/trivial.hpp>

/// indicates if logging is active
static std::atomic<bool> s_bLoggingActive = false;

/// log filename
CString s_cszLogFilename;

void LogConfigure(bool bEnable, const CString& cszLogfilePath) throw()
{
   s_bLoggingActive = bEnable;

   if (bEnable)
   try
   {
      s_cszLogFilename = Path_Combine(cszLogfilePath, _T("Log.txt"));
   }
   catch (...)
   {
      OutputDebugString(_T("Unknown exception occured during LogConfigure() call\n"));
   }
}

void LogTrace(LPCTSTR pszFormat, ...) throw()
{
   if (!s_bLoggingActive)
      return;

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

      static bool s_bPreviousIncompleteLine = false;

      ATLTRACE(cszText);

      static LightweightMutex s_mtxLogging;
      LightweightMutex::LockType lock(s_mtxLogging);

      // write to file
      {
         FILE* fd = NULL;
         errno_t err = _tfopen_s(&fd, s_cszLogFilename.GetString(), _T("a+t"));
         if (err != 0 || fd == nullptr)
            return;

         if (s_bPreviousIncompleteLine)
         {
            // append to line
            _ftprintf(fd, _T("%s"), cszText.GetString());
         }
         else
         {
            // output new line

            // get current time
            CString cszDateTime;
            {
               time_t now = time(&now);

               struct tm tmNow = {0};
               localtime_s(&tmNow, &now);

               _tcsftime(cszDateTime.GetBuffer(64), 64, _T("%Y-%m-%dT%H:%M:%S"), &tmNow);
               cszDateTime.ReleaseBuffer();
            }

            _ftprintf(fd, _T("%s [%08x] %s"),
               cszDateTime.GetString(),
               GetCurrentThreadId(),
               cszText.GetString());
         }

         fflush(fd);

         fclose(fd);
      }

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
