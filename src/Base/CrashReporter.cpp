//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CrashReporter.cpp Application crash reporting
//

// includes
#include "StdAfx.h"
#include "CrashReporter.hpp"
#include "Logging.hpp"
#include "Filesystem.hpp"
#include "Path.hpp"
#include <strsafe.h>
#include <ctime>
#include <memory>

#pragma warning(disable: 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include <dbghelp.h>
#pragma warning(default: 4091)

#pragma comment(lib, "dbghelp.lib")

/// base path for writing minidump file
static TCHAR g_szMinidumpBasePath[MAX_PATH] = {0};

/// writes minidump file
bool WriteMinidump(HANDLE hFile, _EXCEPTION_POINTERS* pExceptionInfo)
{
   __try
   {
      MINIDUMP_EXCEPTION_INFORMATION exceptionInfo = {0};
      exceptionInfo.ThreadId = GetCurrentThreadId();
      exceptionInfo.ExceptionPointers = pExceptionInfo;
      exceptionInfo.ClientPointers = false;

      BOOL bRet = MiniDumpWriteDump(
         GetCurrentProcess(),
         GetCurrentProcessId(),
         hFile,
         MiniDumpNormal,
         &exceptionInfo,
         NULL, NULL);

      return bRet != FALSE;
   }
   __except(EXCEPTION_EXECUTE_HANDLER)
   {
      return false;
   }
}

/// creates minidump filename in given buffer
void GetMinidumpFilename(LPTSTR pszMinidumpFilename, UINT uiMaxChars)
{
   HRESULT hr = StringCchCopy(
      pszMinidumpFilename,
      uiMaxChars,
      g_szMinidumpBasePath);
   ATLASSERT(S_OK == hr); hr;

   size_t uiLenBasePath = _tcslen(pszMinidumpFilename);
   TCHAR* pszStart = pszMinidumpFilename + uiLenBasePath;
   unsigned int uiRemaining = uiMaxChars - uiLenBasePath;

   // add date
   time_t nowt = ::time(&nowt);

   struct tm now = {0};
   localtime_s(&now, &nowt);

   _sntprintf_s(pszStart, uiRemaining, uiRemaining,
      _T("%04u-%02u-%02u %02u.%02u.%02u.mdmp"),
      now.tm_year + 1900,
      now.tm_mon + 1,
      now.tm_mday,
      now.tm_hour,
      now.tm_min,
      now.tm_sec);
}

/// exception filter function to write minidump file
LONG WINAPI ExceptionFilterWriteMinidump(_EXCEPTION_POINTERS* pExceptionInfo)
{
   OutputDebugString(_T("!!! ExceptionFilterWriteMinidump() called!\n"));

   // construct filename
   TCHAR szMinidumpFilename[MAX_PATH];
   GetMinidumpFilename(szMinidumpFilename, sizeof(szMinidumpFilename)/sizeof(*szMinidumpFilename));

   // write minidump file
   HANDLE hFile = CreateFile(szMinidumpFilename, GENERIC_WRITE, 0, NULL,
      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

   if (INVALID_HANDLE_VALUE == hFile)
      return EXCEPTION_CONTINUE_SEARCH;

   std::shared_ptr<void> spFile(hFile, &::CloseHandle);

   OutputDebugString(_T("!!! Writing minidump to file: "));
   OutputDebugString(szMinidumpFilename);
   OutputDebugString(_T("\n"));

   WriteMinidump(hFile, pExceptionInfo);

   CloseHandle(hFile);

   // as last resort, try to log error
   LOG_TRACE(CString(_T("wrote minidump file: ")) + szMinidumpFilename);

   return EXCEPTION_CONTINUE_SEARCH;
}

/// handler function for std::terminate
void OnStdTerminate()
{
   OutputDebugString(_T("!!! OnStdTerminate() called!\n"));

   // construct filename
   TCHAR szMinidumpFilename[MAX_PATH];
   GetMinidumpFilename(szMinidumpFilename, sizeof(szMinidumpFilename)/sizeof(*szMinidumpFilename));

   // cause an exception, so that we can write a minidump
   EXCEPTION_POINTERS* pExceptionInfo = NULL;
   __try
   {
      throw 42;
   }
   __except( (pExceptionInfo = GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER)
   {
      if (pExceptionInfo != NULL)
         ExceptionFilterWriteMinidump(pExceptionInfo);
   }
}

void CrashReporter::Init(const CString& cszBasePath)
{
   // set minidump base path
   CString cszPath = cszBasePath;
   Path::AddEndingBackslash(cszPath);

   ::CreateDirectory(cszPath, NULL);

   cszPath += Path(App_GetFilename()).FilenameOnly() + _T("-");

   HRESULT hr = StringCchCopy(
      g_szMinidumpBasePath,
      sizeof(g_szMinidumpBasePath)/sizeof(*g_szMinidumpBasePath),
      cszPath);
   ATLASSERT(S_OK == hr); hr;

   // set exception filter
   SetUnhandledExceptionFilter(&ExceptionFilterWriteMinidump);

   // catch all std::terminate() calls
   std::set_terminate(&OnStdTerminate);
}
