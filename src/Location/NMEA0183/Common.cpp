//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file NMEA0183/Common.cpp Helper methods for NMEA 0183 parsing
//
#include "stdafx.h"
#include "Common.hpp"
#include "ParserException.hpp"

using NMEA0183::Helper;
using NMEA0183::ParserException;

TimeSpan Helper::ParseUTCTime(const CString& field)
{
   // check length and point
   if (field.GetLength() != 6)
   {
      if (field.GetLength() < 6 + 1 + 1 || field[6] != _T('.'))
         throw ParserException(_T("invalid UTC time format"), field, __FILE__, __LINE__);
   }

   // convert
   LPTSTR end = NULL;
   unsigned long intVal = _tcstoul(field, &end, 10);

   unsigned int fractVal = 0;
   if (field.GetLength() != 6)
   {
      if (end == NULL || *end != _T('.'))
         throw ParserException(_T("invalid UTC time format"), field, __FILE__, __LINE__);

      double d = _tcstod(field, NULL);
      fractVal = static_cast<unsigned int>((d - floor(d))*1000.0 + 0.5);
   }

   // check digit ranges
   if ((intVal / 10000) >= 24 ||
      ((intVal / 100) % 100) > 59 ||
      (intVal % 100) > 59 ||
      fractVal > 999)
      throw ParserException(_T("invalid UTC time format"), field, __FILE__, __LINE__);

   // convert to time
   TimeSpan t(
      intVal / 10000,
      (intVal / 100) % 100,
      intVal % 100,
      fractVal);

   // check fields
   if (t.Status() == TimeSpan::invalid ||
      t.Hours() >= 24 ||
      t.Minutes() > 59 ||
      t.Seconds() > 59 ||
      t.Milliseconds() > 1000)
      throw ParserException(_T("invalid UTC time format"), field, __FILE__, __LINE__);

   return t;
}

DateTime Helper::ParseUTCDateTime(const CString& fieldDate, const CString& fieldTime)
{
   // check length
   if (fieldDate.GetLength() != 6)
      throw ParserException(_T("invalid UTC date format"), fieldDate, __FILE__, __LINE__);

   // convert
   LPTSTR end = NULL;
   unsigned long intVal = _tcstoul(fieldDate, &end, 10);

   if (end != NULL && *end != 0)
      throw ParserException(_T("invalid UTC date format"), fieldDate, __FILE__, __LINE__);

   unsigned int day = intVal / 10000;
   unsigned int month = (intVal / 100) % 100;
   unsigned int year = intVal % 100;

   // adjust to full year
   // note: since this code is assumed to parse every valid date and we only have two digits
   // for the year, assume that dates older than 1980 don't appear. So treat "80" and above as
   // 20th century (19xx), all below as 21th century (20xx). This makes this code safe for use
   // up to 2080. Another workaround would be to check for the current date and assume
   // something sensible.
   if (year < 80)
      year += 2000;
   else
      year += 1900;

   TimeSpan timeInDay = ParseUTCTime(fieldTime);

   return DateTime(year, month, day, timeInDay.Hours(), timeInDay.Minutes(), timeInDay.Seconds());
}

GPS::WGS84::Coordinate Helper::ParseLatLong(const CString& strLatitude, TCHAR latitudeDir,
   const CString& strLongitude, TCHAR longitudeDir)
{
   // empty fields?
   if (strLatitude.GetLength() == 0 && latitudeDir == 0 &&
      strLongitude.GetLength() == 0 && longitudeDir == 0)
   {
      // invalid coordinate
      return GPS::WGS84::Coordinate();
   }

   // lat and long dir characters valid?
   if (latitudeDir != _T('N') && latitudeDir != _T('S'))
      throw ParserException(_T("invalid latitude format"), CString(latitudeDir) + strLatitude, __FILE__, __LINE__);

   if (longitudeDir != _T('E') && longitudeDir != _T('W'))
      throw ParserException(_T("invalid longitude format"), CString(longitudeDir) + strLongitude, __FILE__, __LINE__);

   // parse degrees, minutes and seconds
   double dLatitude = _tcstod(strLatitude, NULL);
   double dLongitude = _tcstod(strLongitude, NULL);

   // the format is always as following:
   // dddmm.mm
   signed int latitudeDegree = static_cast<signed int>(dLatitude) / 100;
   double dLatMinutes = fmod(dLatitude, 100.0);
   unsigned int latitudeMinutes = static_cast<unsigned int>(dLatMinutes);
   unsigned int latitudeSeconds = static_cast<unsigned int>(floor(dLatMinutes * 60.0 + 0.5)) % 60;
   unsigned int latitudeMilliSeconds = static_cast<unsigned int>(floor(dLatMinutes * 60.0 * 1000.0 + 0.5)) % 1000;

   if (latitudeDir == _T('S'))
      latitudeDegree = -latitudeDegree;

   signed int longitudeDegree = static_cast<signed int>(dLongitude) / 100;
   double dLongMinutes = fmod(dLongitude, 100.0);
   unsigned int longitudeMinutes = static_cast<unsigned int>(dLongMinutes);
   unsigned int longitudeSeconds = static_cast<unsigned int>(floor(dLongMinutes * 60.0 + 0.5)) % 60;
   unsigned int longitudeMilliSeconds = static_cast<unsigned int>(floor(dLongMinutes * 60.0 * 1000.0 + 0.5)) % 1000;

   if (longitudeDir == _T('W'))
      longitudeDegree = -longitudeDegree;

   GPS::WGS84::Coordinate coord = GPS::WGS84::Coordinate(latitudeDegree, latitudeMinutes, latitudeSeconds, latitudeMilliSeconds,
      longitudeDegree, longitudeMinutes, longitudeSeconds, longitudeMilliSeconds);

   if (!coord.IsValid())
      throw ParserException(_T("invalid lat/long format"),
         latitudeDir + strLatitude + _T(" / ") + longitudeDir + strLongitude, __FILE__, __LINE__);

   return coord;
}

NMEA0183::T_enFAAMode Helper::ParseFAAMode(const CString& field)
{
   NMEA0183::T_enFAAMode mode = modeUnknown;

   if (field.GetLength() != 1)
      throw ParserException(_T("invalid FAA mode format"), field, __FILE__, __LINE__);

   switch (field.GetAt(0))
   {
   case _T('N'): mode = modeNotValid; break;
   case _T('A'): mode = modeAutonomous; break;
   case _T('D'): mode = modeDifferential; break;
   case _T('P'): mode = modePrecise; break;
   case _T('R'): mode = modeRealTimeKinematic; break;
   case _T('F'): mode = modeFloatRTK; break;
   case _T('E'): mode = modeEstimated; break;
   case _T('M'): mode = modeManual; break;
   case _T('S'): mode = modeSimulator; break;
   default:
      throw ParserException(_T("unknown FAA mode field"), field, __FILE__, __LINE__);
   }

   return mode;
}

BYTE Helper::CalculateChecksum(LPCTSTR data, UINT length)
{
   BYTE result = 0;
   for (UINT index = 0; index < length; index++)
      result ^= data[index];

   return result;
}
