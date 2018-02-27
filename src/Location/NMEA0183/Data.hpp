//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file NMEA0183/Data.hpp Data structs for NMEA 0183 sentences
//
#pragma once

#include <vector>
#include <ulib/DateTime.hpp>
#include "GPS/Coordinate.hpp"
#include "GPS/SatelliteInfo.hpp"
#include "Common.hpp"

namespace NMEA0183
{
   class SentenceInfo;

   /// $--GGA sentence - Global Positioning System Fix Data, Time, Position
   struct DataGGA
   {
   public:
      /// ctor
      DataGGA()
         :m_qualityIndicator(qualityFixNotAvail),
         m_numberOfSatellites(0),
         m_horizontalDOP(0.0),
         m_altitudeAboveMeanSeaLevel(0.0),
         m_altitudeGeoidalSeparation(0.0),
         m_DGPSDataAge(0),
         m_DGPSReferenceStation(0)
      {
      }

      /// parses sentence info into data
      void Parse(const SentenceInfo& sentence);

      /// time part of date/time
      TimeSpan m_time;

      /// coordinate
      GPS::WGS84::Coordinate m_coordinate;

      /// fix quality indicator
      T_enQualityIndicator m_qualityIndicator;

      /// number of satellites in view
      unsigned int m_numberOfSatellites;

      /// horizontal dilution of position
      double m_horizontalDOP;

      /// altitude above mean sea level (geoid) in meter
      double m_altitudeAboveMeanSeaLevel;

      /// geoidal separation (distance between geoid and mean-sea-level) in meter
      double m_altitudeGeoidalSeparation;

      /// DGPS data age in seconds
      unsigned int m_DGPSDataAge;

      /// reference station used for DGPS
      unsigned int m_DGPSReferenceStation;
   };

   /// $--GSA sentence - GPS DOP and active satellites (SA=satellite active)
   struct DataGSA
   {
   public:
      /// ctor
      DataGSA()
         :m_selectionMode(selModeUnknown),
         m_fixMode(fixUnknown),
         m_positionalDOP(-1.0),
         m_horizontalDOP(-1.0),
         m_verticalDOP(-1.0),
         m_faaMode(modeUnknown)
      {
      }

      /// parses sentence info into data
      void Parse(const SentenceInfo& sentence);

      /// selection mode
      T_enSelectionMode m_selectionMode;

      /// GPS fix mode
      T_enFixMode m_fixMode;

      /// satellite IDs used for fix
      std::vector<unsigned int> m_satelliteIDs;

      /// positional Dilution of Precision (for altitude)
      double m_positionalDOP;

      /// horizontal Dilution of Precision
      double m_horizontalDOP;

      /// vertical Dilution of Precision
      double m_verticalDOP;

      /// FAA mode indicator
      T_enFAAMode m_faaMode;
   };

   /// $--RMC sentence - Recommended Minimum Navigation Information C
   struct DataRMC
   {
   public:
      /// ctor
      DataRMC()
         :m_gpsStatus(statusUnknown),
         m_speedOverGround(0.0),
         m_trackMadeGood(0.0),
         m_magneticVariation(0.0),
         m_faaMode(modeUnknown)
      {
      }

      /// parses sentence info into data
      void Parse(const SentenceInfo& sentence);

      /// date/time
      DateTime m_datetime;

      /// GPS status
      T_enGPSStatus m_gpsStatus;

      /// GPS Coordinate
      GPS::WGS84::Coordinate m_coordinate;

      /// speed over ground in km/h
      double m_speedOverGround;

      /// track angle made good; in degrees true
      double m_trackMadeGood;

      /// magnetic variation in degrees
      double m_magneticVariation;

      /// FAA mode indicator
      T_enFAAMode m_faaMode;
   };

   /// $--VTG sentence - Track made good and Ground speed
   struct DataVTG
   {
   public:
      /// ctor
      DataVTG()
         :m_trackDegreesTrue(0.0),
         m_trackDegreesMagnetic(0.0),
         m_speedOverGroundKnots(0.0),
         m_speedOverGround(0.0),
         m_faaMode(modeUnknown)
      {
      }

      /// parses sentence info into data
      void Parse(const SentenceInfo& sentence);

      /// track angle in degrees true
      double m_trackDegreesTrue;

      /// track angle in degrees magnetic
      double m_trackDegreesMagnetic;

      /// speed over ground in knots
      double m_speedOverGroundKnots;

      /// speed over ground in km/h
      double m_speedOverGround;

      /// FAA mode indicator
      T_enFAAMode m_faaMode;
   };

   /// $--GSV sentence - Satellites in view
   struct DataGSV
   {
   public:
      /// ctor
      DataGSV()
         :m_sentenceNr(0),
         m_maxSentenceNr(0),
         m_satellitesInView(0)
      {
      }

      /// parses sentence info into data
      void Parse(const SentenceInfo& sentence);

      /// number of current GSV sentence
      unsigned int m_sentenceNr;

      /// maximum number of GSV sentences
      unsigned int m_maxSentenceNr;

      /// number of satellites in view
      unsigned int m_satellitesInView;

      /// list of satellite infos
      std::vector<GPS::SatelliteInfo> m_satelliteInfos;
   };

   /// $--GLL sentence - Geographic Position - Latitude/Longitude
   struct DataGLL
   {
   public:
      /// ctor
      DataGLL()
         :m_gpsStatus(statusUnknown),
         m_faaMode(modeUnknown)
      {
      }

      /// parses sentence info into data
      void Parse(const SentenceInfo& sentence);

      /// coordinate
      GPS::WGS84::Coordinate m_coordinate;

      /// time part of date/time
      TimeSpan m_time;

      /// GPS status
      T_enGPSStatus m_gpsStatus;

      /// FAA mode indicator
      T_enFAAMode m_faaMode;
   };

   /// $--GNS sentence - Fix data
   struct DataGNS
   {
   public:
      /// ctor
      DataGNS()
         :m_numberOfSatellitesInUse(0),
         m_horizontalDOP(0.0),
         m_altitudeAboveMeanSeaLevel(0.0),
         m_altitudeGeoidalSeparation(0.0),
         m_DGPSDataAge(0),
         m_DGPSReferenceStation(0)
      {
      }

      /// parses sentence info into data
      void Parse(const SentenceInfo& sentence);

      /// time part of date/time
      TimeSpan m_time;

      /// GPS Coordinate
      GPS::WGS84::Coordinate m_coordinate;

      /// FAA mode indicators for GPS, GLONASS
      std::vector<T_enFAAMode> m_faaModeList;

      /// number of satellites in use
      unsigned int m_numberOfSatellitesInUse;

      /// horizontal dilution of position
      double m_horizontalDOP;

      /// altitude above mean sea level (geoid) in meter
      double m_altitudeAboveMeanSeaLevel;

      /// geoidal separation (distance between geoid and mean-sea-level) in meter
      double m_altitudeGeoidalSeparation;

      /// DGPS data age in seconds
      unsigned int m_DGPSDataAge;

      /// reference station used for DGPS
      unsigned int m_DGPSReferenceStation;
   };

   /// private data
   struct DataP
   {
   public:
      /// ctor
      DataP()
      {
      }

      /// parses sentence info into data
      void Parse(const SentenceInfo& sentence);

      /// 3-character manufacturer code
      /// \see http://www.nmea.org/Assets/011309-0183_manufacturer_codes.pdf
      CString m_manufacturerCode;

      /// all remaining fields
      std::vector<CString> m_fields;
   };

} // namespace NMEA0183
