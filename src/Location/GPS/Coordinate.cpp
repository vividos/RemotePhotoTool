//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file Coordinate.cpp WGS84 coordinate
//

// includes
#include "stdafx.h"
#include "Coordinate.hpp"
#include <cmath>

using GPS::WGS84::Coordinate;

Coordinate::Coordinate()
   :m_latitude(0),
   m_longitude(0),
   m_valid(false)
{
}

/// \param latitude latitude in full degrees [0°;360°[
/// \param longitude longitude in full degrees [-90°;90°[
Coordinate::Coordinate(double latitude, double longitude)
   :m_latitude(0),
   m_longitude(0),
   m_valid(false)
{
   Set(latitude, longitude);
}

Coordinate::Coordinate(
   signed int latitudeDegree, unsigned int latitudeMinutes, unsigned int latitudeSeconds, unsigned int latitudeMilliSeconds,
   signed int longitudeDegree, unsigned int longitudeMinutes, unsigned int longitudeSeconds, unsigned int longitudeMilliSeconds)
   :m_valid(false),
   m_latitude((((abs(latitudeDegree) * 60) + int(latitudeMinutes)) * 60 + int(latitudeSeconds)) * 1000 + int(latitudeMilliSeconds)),
   m_longitude((((abs(longitudeDegree) * 60) + int(longitudeMinutes)) * 60 + int(longitudeSeconds)) * 1000 + int(longitudeMilliSeconds))
{
   if (latitudeDegree < 0)
      m_latitude = -m_latitude;

   if (longitudeDegree < 0)
      m_longitude = -m_longitude;

   m_valid = latitudeDegree >= -90 && latitudeDegree < 90 &&
      latitudeMinutes < 60 && latitudeSeconds < 60 && latitudeMilliSeconds < 1000 &&
      longitudeDegree >= -180 && longitudeDegree < 180 &&
      longitudeMinutes < 60 && longitudeSeconds < 60 && longitudeMilliSeconds < 1000;
}

/// \param latitude latitude in full degrees [-90°;90°[
/// \param longitude longitude in full degrees [-180°;180°[
void Coordinate::Set(double latitude, double longitude)
{
   ATLASSERT(latitude >= -90.0 && latitude <= 90.0);
   ATLASSERT(longitude >= -180.0 && longitude <= 180.0);

   m_valid = latitude >= -90.0 && latitude <= 90.0 && longitude >= -180.0 && longitude <= 180.0;

   if (m_valid)
   {
      // lat/long is stored in a 0.001 arc second resolution
      m_latitude = static_cast<signed int>(floor(latitude  * 60.0 * 60.0 * 1000.0 + 0.5));
      m_longitude = static_cast<signed int>(floor(longitude * 60.0 * 60.0 * 1000.0 + 0.5));
   }
}

double Coordinate::GetLatitude() const
{
   return m_latitude / (60.0 * 60.0 * 1000.0);
}

double Coordinate::GetLongitude() const
{
   return m_longitude / (60.0 * 60.0 * 1000.0);
}

signed int Coordinate::GetDegreeLatitude() const
{
   return m_latitude / (60 * 60 * 1000);
}

unsigned int Coordinate::GetMinuteLatitude() const
{
   unsigned int latitudeMin = static_cast<unsigned int>(abs(m_latitude)) / (60 * 1000);
   return latitudeMin % 60;
}

unsigned int Coordinate::GetSecondLatitude() const
{
   unsigned int latitudeSec = static_cast<unsigned int>(abs(m_latitude)) / 1000;
   return latitudeSec % 60;
}

unsigned int Coordinate::GetMilliSecondLatitude() const
{
   return static_cast<unsigned int>(abs(m_latitude)) % 1000;
}

signed int Coordinate::GetDegreeLongitude() const
{
   return m_longitude / (60 * 60 * 1000);
}

unsigned int Coordinate::GetMinuteLongitude() const
{
   unsigned int longitudeMin = static_cast<unsigned int>(abs(m_longitude)) / (60 * 1000);
   return longitudeMin % 60;
}

unsigned int Coordinate::GetSecondLongitude() const
{
   unsigned int longitudeSec = static_cast<unsigned int>(abs(m_longitude)) / 1000;
   return longitudeSec % 60;
}

unsigned int Coordinate::GetMilliSecondLongitude() const
{
   return static_cast<unsigned int>(abs(m_longitude)) % 1000;
}

/// pi
double c_pi = 3.14159265358979323846264338327950288419716939937510;

/// nautical mile in meter
double c_dNauticalMile = 1852.0;

/// converts radians to degrees
inline double Rad2Deg(double dRad) { return (180.0 / c_pi) * dRad; }

/// converts degrees to radians
inline double Deg2Rad(double dDeg) { return (c_pi / 180.0) * dDeg; }

/// converts nautical miles to radians
inline double DistanceNauticalMile2Rad(double dNM) { return (c_pi / (180.0*60.0)) * dNM; }

/// converts radians to nautical miles
inline double DistanceRad2NauticalMile(double dRad) { return ((180.0 * 60.0) / c_pi) * dRad; }

double Coordinate::DistanceTo(const Coordinate& coord)
{
   // d=2*asin(sqrt((sin((lat1-lat2)/2))^2 + cos(lat1)*cos(lat2)*(sin((lon1-lon2)/2))^2))

   double lat1 = Deg2Rad(GetLatitude());
   double lon1 = Deg2Rad(GetLongitude());
   double lat2 = Deg2Rad(coord.GetLatitude());
   double lon2 = Deg2Rad(coord.GetLongitude());

   double n1 = sin((lat1 - lat2) / 2.0);
   double n2 = sin((lon1 - lon2) / 2.0);

   double d = n1*n1 + cos(lat1)*cos(lat2)*n2*n2;
   d = sqrt(d);
   d = 2.0 * asin(d);

   return c_dNauticalMile * DistanceRad2NauticalMile(d);
}

void Coordinate::ToString(EFormat eFormat, CString& strLatitude, CString& strLongitude) const
{
   TCHAR cLatHemisphere = GetLatitude() >= 0.0 ? _T('N') : _T('S');
   TCHAR cLongHemisphere = GetLongitude() >= 0.0 ? _T('E') : _T('W');

   switch (eFormat)
   {
   case fmtDDD_MM_dot_mmmm: // ddd°mm.mmmm
   {
      unsigned int latitudeSec = GetSecondLatitude() * 1000 + GetMilliSecondLatitude();
      strLatitude.Format(_T("%c %u°%02u.%03u"),
         cLatHemisphere,
         abs(GetDegreeLatitude()),
         GetMinuteLatitude(),
         latitudeSec / 60);

      unsigned int longitudeSec = GetSecondLongitude() * 1000 + GetMilliSecondLongitude();
      strLongitude.Format(_T("%c %u°%02u.%03u"),
         cLongHemisphere,
         abs(GetDegreeLongitude()),
         GetMinuteLongitude(),
         longitudeSec / 60);
   }
   break;

   case fmtDDD_MM_aph_ss_dot_sss: // ddd°mm'ss.sss
      strLatitude.Format(_T("%c %u°%02u'%02u.%03u"),
         cLatHemisphere,
         abs(GetDegreeLatitude()),
         GetMinuteLatitude(),
         GetSecondLatitude(),
         GetMilliSecondLatitude());
      strLongitude.Format(_T("%c %u°%02u'%02u.%03u"),
         cLongHemisphere,
         abs(GetDegreeLongitude()),
         GetMinuteLongitude(),
         GetSecondLongitude(),
         GetMilliSecondLongitude());
      break;

   case fmtSign_DDD_dot_dddd: // [-]ddd.dddddddd
   {
      double fractLat = GetLatitude() - floor(GetLatitude());
      strLatitude.Format(_T("%i.%08u"),
         GetDegreeLatitude(),
         static_cast<unsigned int>(floor(fractLat*100000000.0 + 0.5)));

      double fractLong = GetLongitude() - floor(GetLongitude());
      strLongitude.Format(_T("%i.%08u"),
         GetDegreeLongitude(),
         static_cast<unsigned int>(floor(fractLong*100000000.0 + 0.5)));
   }
   break;

   case fmtDir_DDD_dot_dddd: // [NESW] ddd.dddddddd°
   {
      double fractLat = GetLatitude() - floor(GetLatitude());
      strLatitude.Format(_T("%c %u.%08u"),
         cLatHemisphere,
         abs(GetDegreeLatitude()),
         static_cast<unsigned int>(floor(fractLat*100000000.0 + 0.5)));

      double fractLong = GetLongitude() - floor(GetLongitude());
      strLongitude.Format(_T("%c %u.%08u"),
         cLongHemisphere,
         abs(GetDegreeLongitude()),
         static_cast<unsigned int>(floor(fractLong*100000000.0 + 0.5)));
   }
   break;

   default:
      ATLASSERT(false); // invalid format
      break;
   }
}
