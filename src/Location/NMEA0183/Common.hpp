//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file NMEA0183/Common.hpp Common types for NMEA 0183 parsing
//
#pragma once

#include <ulib/DateTime.hpp>
#include "GPS/Coordinate.hpp"

namespace NMEA0183
{
   /// quality indicator; found in GGA sentence
   enum T_enQualityIndicator
   {
      qualityFixNotAvail = 0, ///< fix not available
      qualityFixGPS = 1,      ///< GPS fix
      qualityFixDGPS = 2,     ///< DGPS fix
      qualityFixOther = 3,    ///< other fix (original values 3..8)
   };

   /// \brief indicates selection mode of GPS device; used in GSA sentence
   /// some devices can force a GPS fix; they then operate in manual mode
   enum T_enSelectionMode
   {
      selModeAutomatic = 0,   ///< GPS device automatically decides mode
      selModeManual,          ///< manual mode
      selModeInvalid,         ///< invalid mode
      selModeUnknown          ///< unknown selection mode
   };

   /// indicates fix mode of the GPS device; used in GSA sentence
   enum T_enFixMode
   {
      fixUnknown = 0,   ///< unknown fix mode
      fixNone = 1,      ///< no fix
      fix2D = 2,        ///< 2D fix (lat/long is valid)
      fix3D = 3,        ///< 3D fix (lat/long/height is valid)
   };

   /// FAA mode indicator used in NMEA 0183 version 2.3 in RMC, GSA, GSV, GLL and VTG
   enum T_enFAAMode
   {
      modeUnknown = 0,     ///< the mode field wasn't present in the sentence
      modeNotValid,        ///< No valid fix
      modeAutonomous,      ///< Autonomous (non-differential)
      modeDifferential,    ///< Differential mode
      modePrecise,         ///< Precise (no degradation, like SA)
      modeRealTimeKinematic, ///< Real time kinematic
      modeFloatRTK,        ///< Float RTK
      modeEstimated,       ///< Estimated mode (dead reckoning)
      modeManual,          ///< Manual input mode
      modeSimulator        ///< Simulator mode
   };

   /// status in RMC/GLL sentences
   enum T_enGPSStatus
   {
      statusUnknown = 0,   ///< status is unknown (field was missing from sentence)
      statusActive,        ///< GPS is active
      statusVoid           ///< GPS data is void
   };

   /// NMEA 0183 protocol sentence type
   enum T_enSentenceType
   {
      typeUnknown = 0, ///< unknown type
      typeGGA, ///< Global Positioning System Fix Data, Time, Position and fix related data for a GPS receiver.
      typeGSA, ///< GPS DOP and active satellites
      typeRMC, ///< Recommended Minimum Navigation Information
      typeVTG, ///< Track made good and Ground speed
      typeGSV, ///< Satellites in view
      typeGLL, ///< Geographic Position - Latitude/Longitude
      typeGNS, ///< Fix data
      typePrivate, ///< private data; e.g. $PSRFTXT
   };

   /// parser helper functions
   class Helper
   {
   public:
      /// parses field as UTC time
      static TimeSpan ParseUTCTime(const CString& field);

      /// parse fields as UTC date/time
      static DateTime ParseUTCDateTime(const CString& fieldDate, const CString& fieldTime);

      /// parses latitude/longitude to WGS84 coordinates
      static GPS::WGS84::Coordinate ParseLatLong(const CString& latitude, TCHAR latitudeDir,
         const CString& longitude, TCHAR longitudeDir);

      /// parses FAA mode indicator
      static T_enFAAMode ParseFAAMode(const CString& field);

      /// calculates checksum from given data
      static BYTE CalculateChecksum(LPCTSTR data, UINT length);
   };

} // namespace NMEA0183
