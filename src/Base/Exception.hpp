//
// ulib - a collection of useful classes
// Copyright (C) 2006,2007,2008,2009,2012 Michael Fink
//
/// \file Exception.hpp exception base class
//
#pragma once

/// exception base class
class Exception
{
public:
   /// ctor
   Exception(LPCSTR pszSourceFile, UINT uiSourceLine) throw()
      :m_cszSourceFile(pszSourceFile),
       m_uiSourceLine(uiSourceLine)
   {
   }

   /// ctor
   Exception(const CString& cszMessage, LPCSTR pszSourceFile, UINT uiSourceLine) throw()
      :m_cszMessage(cszMessage),
       m_cszSourceFile(pszSourceFile),
       m_uiSourceLine(uiSourceLine)
   {
   }

   /// returns exception message
   CString Message() const throw() { return m_cszMessage; }

   /// returns source file where the exception occured
   CString SourceFile() const throw() { return m_cszSourceFile; }
   /// returns source file line where the exception occured
   UINT SourceLine() const throw() { return m_uiSourceLine; }

private:
   /// message
   CString m_cszMessage;

   /// source file name
   CString m_cszSourceFile;

   /// source file line
   UINT m_uiSourceLine;
};
