//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file NMEA0183/SentenceInfo.cpp NMEA 0183 sentence info
//
#include "stdafx.h"
#include "SentenceInfo.hpp"

using NMEA0183::SentenceInfo;

void SentenceInfo::ParseFields(LPCTSTR fields)
{
   unsigned int offset = 0;
   LPCTSTR pos;
   while (NULL != (pos = _tcschr(fields + offset, _T(','))))
   {
      ATLASSERT(pos >= fields + offset);
      unsigned int dist = static_cast<unsigned int>(pos - (fields + offset));

      m_fieldInfos.push_back(FieldInfo(offset, dist));

      offset += static_cast<unsigned int>(dist + 1);
   }

   // add last position
   m_fieldInfos.push_back(FieldInfo(offset, static_cast<unsigned int>(_tcslen(fields + offset))));

   m_sentenceInfo = fields;
}
