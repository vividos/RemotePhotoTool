//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file NMEA0183/Parser.hpp Parser for NMEA 0183 sentences
//
#pragma once

#include "Common.hpp"
#include "GPS/PositionInfo.hpp"
#include "GPS/SatelliteInfo.hpp"
#include <set>

/// \brief NMEA 0183 format parsing
/// \see http://www.nmea.de/nmea0183datensaetze.html
/// \see http://www.gpsinformation.org/dale/nmea.htm
/// \see http://freenmea.net/docs
/// \see http://www.catb.org/gpsd/NMEA.html
namespace NMEA0183
{
   class SentenceInfo;

   /// parser for NMEA 0183 sentences
   class Parser
   {
   public:
      /// ctor
      Parser();

      /// parses a single text line
      void ParseLine(LPCTSTR line);

      /// resets the Update*() flags (below)
      void ResetUpdateFlags();

      /// returns current position info
      const GPS::PositionInfo& GetPositionInfo() const { return m_positionInfo; }

      /// returns current position info
      const std::vector<GPS::SatelliteInfo>& GetSatelliteInfos() const { return m_satelliteInfos; }

      /// indicates if position info has changed since last ParseLine() call
      bool UpdatedPositionInfo() const { return m_updatedPositionInfo; }

      /// indicates if date/time has changed since last ParseLine() call
      bool UpdatedDateTime() const { return m_updatedDateTime; }

      /// indicates if satellite info has changed since last ParseLine() call
      bool UpdatedSatelliteInfos() const { return m_updatedSatelliteInfos; }

   private:
      /// parses sentence info from given line
      SentenceInfo ParseSentenceInfo(LPCTSTR line);

      /// parses GGA sentence
      void OnSentenceTypeGGA(NMEA0183::SentenceInfo &sentence);

      /// parses GSA sentence
      void OnSentenceTypeGSA(const SentenceInfo& sentence);

      /// parses RMC sentence
      void OnSentenceTypeRMC(NMEA0183::SentenceInfo &sentence);

      /// parses VTG sentence
      void OnSentenceTypeVTG(NMEA0183::SentenceInfo &sentence);

      /// parses GSV sentence
      void OnSentenceTypeGSV(const SentenceInfo& sentence);

      /// parses GGL sentence
      void OnSentenceTypeGLL(NMEA0183::SentenceInfo &sentence);

      /// parses GNS sentence
      void OnSentenceTypeGNS(NMEA0183::SentenceInfo &sentence);

      /// updates valid date/time from new time-only value
      void UpdateDateTimeFromTimeSpan(const TimeSpan& newTime);

      /// updates list of satellite infos with "used for fix" flag
      void UpdateSatIdsUsedForFix();

   private:
      /// indicates if position info has changed since last ParseLine() call
      bool m_updatedPositionInfo;

      /// indicates if date/time has changed since last ParseLine() call
      bool m_updatedDateTime;

      /// indicates if satellite info has changed since last ParseLine() call
      bool m_updatedSatelliteInfos;

      /// current position info
      GPS::PositionInfo m_positionInfo;

      /// list of satellite infos
      std::vector<GPS::SatelliteInfo> m_satelliteInfos;

      /// set of sat IDs that are used for fix; from GSA sentence
      std::set<unsigned int> m_setSatIdsForFix;
   };

} // namespace NMEA0183
