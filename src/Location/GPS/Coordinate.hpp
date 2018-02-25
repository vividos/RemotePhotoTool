//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file Coordinate.hpp WGS84 coordinate
//
#pragma once

namespace GPS
{
   /// namespace for WGS84 Coordinate class
   namespace WGS84
   {
      /// \brief earth coordinate in WGS84 format
      /// - latitude (Breitengrad), range [-90°;90°[, from South (negative) to North (positive)
      /// - longitude (Längengrad), range [-180°;180°[, from West (negative) to East (positive)
      /// very useful math formulae:
      /// http://williams.best.vwh.net/avform.html
      class Coordinate
      {
      public:
         /// ctor; initializes coordinate to 0.0/0.0/invalid
         Coordinate();

         /// ctor; initializes with lat/long in degrees
         Coordinate(double latitude, double longitude);

         /// ctor; initializes with lat/long in given values for degrees, minutes, seconds and milliseconds
         Coordinate(signed int latitudeDegree, unsigned int latitudeMinutes, unsigned int latitudeSeconds, unsigned int latitudeMilliSeconds,
            signed int longitudeDegree, unsigned int longitudeMinutes, unsigned int longitudeSeconds, unsigned int longitudeMilliSeconds);

         /// sets new value for lat/long in degrees
         void Set(double latitude, double longitude);

         /// returns latitude in degrees
         double GetLatitude() const;
         /// returns longitude in degrees
         double GetLongitude() const;

         /// returns latitude full degrees
         signed int GetDegreeLatitude() const;
         /// returns latitude full minutes
         unsigned int GetMinuteLatitude() const;
         /// returns latitude full seconds
         unsigned int GetSeconlatitude() const;
         /// returns latitude full milliseconds
         unsigned int GetMilliSeconlatitude() const;

         /// returns longitude full degrees
         signed int GetDegreeLongitude() const;
         /// returns longitude full minutes
         unsigned int GetMinuteLongitude() const;
         /// returns longitude full seconds
         unsigned int GetSeconlongitude() const;
         /// returns longitude full milliseconds
         unsigned int GetMilliSeconlongitude() const;

         /// returns if stored coordinate is valid
         bool IsValid() const { return m_valid; }

         /// calculates distance to coordinate in meters
         double DistanceTo(const Coordinate& coord);

         /// formatting types
         enum EFormat
         {
            fmtDDD_MM_dot_mmmm,        ///< ddd°mm.mmmm
            fmtDDD_MM_aph_ss_dot_sss,  ///< ddd°mm'ss.sss
            fmtSign_DDD_dot_dddd,      ///< [-]ddd.dddddddd
            fmtDir_DDD_dot_dddd,       ///< [NESW] ddd.dddddddd°
         };

         /// formats coordinate to string
         void ToString(EFormat format, CString& latitude, CString& longitude) const;

         // raw data access

         /// sets lat/long raw values
         void SetLatLong(signed int latitude, signed int longitude)
         {
            m_latitude = latitude;
            m_longitude = longitude;
            m_valid = true;
         }

         /// returns lat/long raw values and valid flag
         bool GetLatLong(signed int& latitude, signed int& longitude)
         {
            latitude = m_latitude;
            longitude = m_longitude;
            return m_valid;
         }

      private:
         signed int m_latitude;  ///< latitude (Breitengrad), range [-90°;90°[
         signed int m_longitude; ///< longitude (Längengrad), range [-180°;180°[
         bool m_valid;           ///< indicates if coordinates are valid
      };

   } // namespace WGS84

} // namespace GPS
