//
// ulib - a collection of useful classes
// Copyright (C) 2008,2012,2014 Michael Fink
//
/// \file ErrorMessage.hpp Win32 error message
//
#pragma once

namespace Win32
{

/// \brief Win32 API error message
/// \details Formats a readable error message from an error value obtained by
/// GetLastError() or other means.
class ErrorMessage
{
public:
   /// ctor; takes a win32 error code
   ErrorMessage(DWORD dwError = GetLastError()) throw()
      :m_dwError(dwError)
   {
   }

   /// Returns formatted error message
   CString Get() throw()
   {
      LPVOID lpMsgBuf = NULL;
      ::FormatMessage(
         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL,
         m_dwError,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
         reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, NULL);

      CString cszErrorMessage;
      if (lpMsgBuf)
      {
         cszErrorMessage = reinterpret_cast<LPTSTR>(lpMsgBuf);
         LocalFree(lpMsgBuf);
      }

      try
      {
         cszErrorMessage.TrimRight(_T("\r\n"));
      }
      catch (...)
      {
      }

      return cszErrorMessage;
   }

private:
   /// error code
   DWORD m_dwError;
};

} // namespace Tools
