//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file NMEA0183/ParserException.hpp Parser exception for NMEA 0183 parsing
//
#pragma once

#include <ulib/Exception.hpp>

namespace NMEA0183
{
   /// parser exception
   class ParserException : public Exception
   {
   public:
      /// ctor
      ParserException(const CString& message, const CString& sentenceInfo, LPCSTR sourceFile, UINT sourceLine)
         :Exception(message + _T(" (") + sentenceInfo + _T(")"), sourceFile, sourceLine),
         m_sentenceInfo(sentenceInfo)
      {
      }

      /// returns faulty sentence
      CString SentenceInfo() const { return m_sentenceInfo; }

   private:
      /// sentence that caused the exception
      CString m_sentenceInfo;
   };

} // namespace NMEA0183
