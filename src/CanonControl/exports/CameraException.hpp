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
   CameraException(const CString& cszFunction, bool bIsSpecific, unsigned int uiComponentId, unsigned int uiErrorId,
                   LPCSTR pszFile, UINT uiLine) throw()
      :Exception(FormatExceptionText(cszFunction, bIsSpecific, uiComponentId, uiErrorId), pszFile, uiLine),
       m_cszFunction(cszFunction),
       m_bIsSpecific(bIsSpecific),
       m_uiComponentId(uiComponentId),
       m_uiErrorId(uiErrorId)
   {
   }

   /// returns name of function where exception occured
   const CString& Function() const throw() { return m_cszFunction; }

   /// indicates if error is specific
   bool IsSpecific() const throw() { return m_bIsSpecific; }

   /// returns component id
   unsigned int ComponentId() const throw() { return m_uiComponentId; }

   /// returns error id
   unsigned int ErrorId() const throw() { return m_uiErrorId; }

private:
   /// helper function to format exception text
   static CString FormatExceptionText(const CString& cszFunction, bool bIsSpecific, unsigned int uiComponentId, unsigned int uiErrorId);

private:
   /// function
   CString m_cszFunction;

   /// specific error?
   bool m_bIsSpecific;

   /// component id
   unsigned int m_uiComponentId;

   /// error id
   unsigned int m_uiErrorId;
};
