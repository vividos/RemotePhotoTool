//
// ulib - a collection of useful classes
// Copyright (C) 2007,2008,2009 Michael Fink
//
/// \file SystemException.hpp win32 system exception class
//
#pragma once

// includes
#include "Exception.hpp"
#include "ErrorMessage.hpp"

/// win32 system exception
class SystemException: public Exception
{
public:
   /// ctor
   SystemException(const CString& cszMessage, DWORD dwWin32Error, LPCSTR pszSourceFile, UINT uiSourceLine) throw()
      :Exception(cszMessage + _T(": ") + Win32::ErrorMessage(dwWin32Error).Get(), pszSourceFile, uiSourceLine),
       m_dwWin32Error(dwWin32Error)
   {
   }

   /// returns win32 error code
   DWORD Win32Error() const throw() { return m_dwWin32Error; }

private:
   /// win32 error code
   DWORD m_dwWin32Error;
};
