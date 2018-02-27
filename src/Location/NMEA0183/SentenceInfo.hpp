//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file NMEA0183/SentenceInfo.hpp NMEA 0183 sentence info
//
#pragma once

#include <vector>
#include "Common.hpp"

namespace NMEA0183
{
   /// a field contains a single field value of a NMEA 0183 protocol sentence
   struct FieldInfo
   {
      /// ctor
      FieldInfo(unsigned int fieldStart, unsigned int fieldLength)
         :m_fieldStart(fieldStart),
         m_fieldLength(fieldLength)
      {
      }

      unsigned int m_fieldStart;    ///< start index of field in sentence
      unsigned int m_fieldLength;   ///< length of field
   };

   /// one sentence of NMEA 0183 protocol
   class SentenceInfo
   {
   public:
      /// ctor
      SentenceInfo(T_enSentenceType type)
         :m_type(type)
      {
      }

      // actions

      /// parses all fields contained in given text
      void ParseFields(LPCTSTR fields);

      // getter

      /// returns sentence type
      T_enSentenceType Type() const { return m_type; }

      /// returns whole sentence
      const CString& Sentence() const { return m_sentenceInfo; }

      /// returns field count in the sentence
      unsigned int FieldCount() const { return static_cast<unsigned int>(m_fieldInfos.size()); }

      /// returns field
      CString Field(unsigned int fieldIndex) const
      {
         ATLASSERT(fieldIndex < m_fieldInfos.size());
         const FieldInfo& fieldInfo = m_fieldInfos[fieldIndex];
         return m_sentenceInfo.Mid(fieldInfo.m_fieldStart, fieldInfo.m_fieldLength);
      }

      /// returns if field with given index is an empty field
      bool IsNullField(unsigned int fieldIndex) const
      {
         ATLASSERT(fieldIndex < m_fieldInfos.size());
         const FieldInfo& fieldInfo = m_fieldInfos[fieldIndex];
         return fieldInfo.m_fieldLength == 0;
      }

   private:
      /// sentence type stored
      T_enSentenceType m_type;

      /// indices into the sentence text, as pairs of start and end offsets
      std::vector<FieldInfo> m_fieldInfos;

      /// the sentence as text
      CString m_sentenceInfo;
   };

} // namespace NMEA0183
