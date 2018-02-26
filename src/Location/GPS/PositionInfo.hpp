//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file PositionInfo.hpp Position info class
//
#pragma once

#include "Coordinate.hpp"
#include <ulib/DateTime.hpp>

namespace GPS
{
   /// \brief position info
   /// infos about current position, speed and direction
   /// infos may be extracted from the following NMEA 0183 sentences:
   /// - coordinate: GGA, GLL, RMC, GNS
   /// - position precision: GGA, GSA, GNS
   /// - altitude: GGA, GNS
   /// - speed: RMC, VTG
   /// - angle: RMC, VTG
   /// - magnetic angle: VTG
   /// - magnetic variation: RMC
   /// - time stamp: GGA, GLL, RMC, GNS
   class PositionInfo
   {
   public:
      PositionInfo()
         :m_positionPrecision(0.0),
         m_altitude(0.0),
         m_geoidHeight(0.0),
         m_speed(0.0),
         m_angle(0.0),
         m_angleMagnetic(0.0),
         m_magneticVariation(0.0)
      {
      }

      // get methods

      /// lat/long coordinate of position
      WGS84::Coordinate Coordinate() const { return m_coordinate; }

      /// position precision
      double PositionPrecision() const { return m_positionPrecision; }

      /// altitude, above mean sea level
      double Altitude() const { return m_altitude; }

      /// height of geoid (mean sea level) above WGS84 ellipsoid
      double GeoidHeight() const { return m_geoidHeight; }

      /// speed in km/h
      double Speed() const { return m_speed; }

      /// angle of movement, true north
      double Angle() const { return m_angle; }

      /// angle of movement, magnetic
      double AngleMagnetic() const { return m_angleMagnetic; }

      /// magnetic variation
      double MagneticVariation() const { return m_magneticVariation; }

      /// time stamp of position info
      DateTime TimeStamp() const { return m_timeStamp; }

      // set methods

      /// sets coordinates
      void Coordinate(WGS84::Coordinate& coord) { m_coordinate = coord; }

      /// precision value for coordinate
      void PositionPrecision(double PositionPrecision) { m_positionPrecision = PositionPrecision; }

      /// altitude, above mean sea level
      void Altitude(double altitude) { m_altitude = altitude; }

      /// height of geoid (mean sea level) above WGS84 ellipsoid
      void GeoidHeight(double geoidHeight) { m_geoidHeight = geoidHeight; }

      /// speed in km/h
      void Speed(double speed) { m_speed = speed; }

      /// angle of movement, true north
      void Angle(double angle) { m_angle = angle; }

      /// angle of movement, magnetic
      void AngleMagnetic(double angleMagnetic) { m_angleMagnetic = angleMagnetic; }

      /// magnetic variation at this position
      void MagneticVariation(double magneticVariation) { m_magneticVariation = magneticVariation; }

      /// time stamp of position info
      void TimeStamp(const DateTime& timeStamp) { m_timeStamp = timeStamp; }

   private:
      WGS84::Coordinate m_coordinate;  ///< lat/long coordinate in WGS84 format
      double m_positionPrecision;      ///< position precision
      double m_altitude;               ///< altitude in m
      double m_geoidHeight;            ///< geoid height in m
      double m_speed;                  ///< speed in km/h
      double m_angle;                  ///< angle in relation to true north
      double m_angleMagnetic;          ///< angle in relation to magnetic north
      double m_magneticVariation;      ///< magnetic variation
      DateTime m_timeStamp;            ///< time stamp of infos
   };

} // namespace GPS
