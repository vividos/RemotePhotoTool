//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CameraException.hpp Canon control - Camera exception
//
#pragma once

// includes
#include "Exception.hpp"

/// camera exception
class CameraException: public Exception
{
public:
   /// ctor
   CameraException(const CString& cszFunction, const CString& cszMessage,
      unsigned int uiErrorCode, LPCSTR pszFile, UINT uiLine) throw()
      :Exception(cszMessage, pszFile, uiLine),
       m_cszFunction(cszFunction),
       m_uiErrorCode(uiErrorCode)
   {
   }

   /// returns name of function where exception occured
   const CString& Function() const throw() { return m_cszFunction; }

   /// returns error code
   unsigned int ErrorCode() const throw() { return m_uiErrorCode; }

private:
   /// function
   CString m_cszFunction;

   /// error code
   unsigned int m_uiErrorCode;
};
