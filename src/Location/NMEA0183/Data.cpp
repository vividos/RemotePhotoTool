//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file NMEA0183/Data.cpp Data structs for NMEA 0183 sentences
//
#include "stdafx.h"
#include "Common.hpp"
#include "Data.hpp"
#include "SentenceInfo.hpp"
#include "ParserException.hpp"

using NMEA0183::SentenceInfo;
using NMEA0183::ParserException;
using NMEA0183::DataGGA;
using NMEA0183::DataGSA;
using NMEA0183::DataRMC;
using NMEA0183::DataVTG;
using NMEA0183::DataGSV;
using NMEA0183::DataGLL;
using NMEA0183::DataGNS;
using NMEA0183::DataP;

//// \brief parse GGA sentence - Global Positioning System Fix Data, Time, Position
/// and fix related data for a GPS receiver.
/// \verbatim
///                                                      11
///        1         2       3 4        5 6 7  8   9  10 |  12 13  14
///        |         |       | |        | | |  |   |   | |   | |   |
/// $--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh<CR><LF>
///
/// Field Number:
///  1) Universal Time Coordinated (UTC)
///  2) Latitude
///  3) N or S (North or South)
///  4) Longitude
///  5) E or W (East or West)
///  6) GPS Quality Indicator
///     0 - fix not available
///     1 - GPS fix
///     2 - DGPS fix
///     (values above 2 are NMEA 2.3 features)
///     3 - PPS fix (unusual)
///     4 - Real Time Kinematic (unusual)
///     5 - Float RTK (unusual)
///     6 - estimated (dead reckoning)
///     7 - Manual input mode (unusual)
///     8 - Simulation mode (unusual)
///  7) Number of satellites in view, 00 - 12
///  8) Horizontal Dilution of precision (HDOP)
///  9) Antenna Altitude above/below mean-sea-level (geoid)
/// 10) Units of antenna altitude, meters (always 'M')
/// 11) Geoidal separation, the difference between the WGS-84 earth
///     ellipsoid and mean-sea-level (geoid), "-" (negative) means mean-sea-level
///     below ellipsoid
/// 12) Units of geoidal separation, meters (always 'M')
/// 13) Age of differential GPS data, time in seconds since last SC104
///     type 1 or 9 update, null field when DGPS is not used
/// 14) Differential reference station ID, 0000-1023 (may be null field)
/// \endverbatim
void DataGGA::Parse(const SentenceInfo& sentence)
{
   ATLASSERT(sentence.Type() == NMEA0183::typeGGA);

   if (sentence.FieldCount() != 14)
      throw ParserException(_T("wrong field count"), sentence.Sentence(), __FILE__, __LINE__);

   // 1: UTC time
   m_time = Helper::ParseUTCTime(sentence.Field(0));

   // 2/3: latitude, 4/5: longitude
   m_coordinate = Helper::ParseLatLong(
      sentence.Field(1), sentence.Field(2).IsEmpty() ? 0 : sentence.Field(2)[0],
      sentence.Field(3), sentence.Field(4).IsEmpty() ? 0 : sentence.Field(4)[0]);

   // 6: quality indicator
   CString field = sentence.Field(5);
   unsigned long ulField = _tcstoul(field, NULL, 10);
   if (ulField > 8)
      throw ParserException(_T("invalid quality indicator (field 6)"), sentence.Sentence(), __FILE__, __LINE__);

   // map 1 and 2 to their meanings, and all other values to "no fix"
   m_qualityIndicator =
      ulField == 1 ? NMEA0183::qualityFixGPS :
      ulField == 2 ? NMEA0183::qualityFixDGPS :
      ulField >= 3 && ulField <= 8 ? NMEA0183::qualityFixOther : qualityFixNotAvail;

   // 7: number of satellites
   m_numberOfSatellites = _tcstoul(sentence.Field(6), NULL, 10);

   // 8: horizontal DOP
   m_horizontalDOP = _tcstod(sentence.Field(7), NULL);

   // 9: altitude above geoid
   m_altitudeAboveMeanSeaLevel = _tcstod(sentence.Field(8), NULL);

   // 10: altitude reference (always M for "mean sea level"); ignored
   // other sources specify this as unit for 8.
   field = sentence.Field(9);
   if (field != _T("M"))
      throw ParserException(_T("invalid altitude ref (field 10)"), sentence.Sentence(), __FILE__, __LINE__);

   // 11: geoidal separation
   m_altitudeGeoidalSeparation = _tcstod(sentence.Field(10), NULL);

   // 12: geoidal separation reference (always M for "mean sea level"); ignored
   // other sources specify this as unit for 8.
   field = sentence.Field(11);
   if (field != _T("M"))
      throw ParserException(_T("invalid altitude ref (field 12)"), sentence.Sentence(), __FILE__, __LINE__);

   // 13: age of DGPS data
   field = sentence.Field(12);
   m_DGPSDataAge = static_cast<unsigned int>(_tcstoul(field, NULL, 10));

   // 14: DGPS reference station number (0000...1023)
   field = sentence.Field(13);
   m_DGPSReferenceStation = static_cast<unsigned int>(_tcstoul(field, NULL, 10));
}

/// \brief parse GSA sentence - GPS DOP and active satellites (SA=satellite active)
/// \verbatim
///        1 2 3 4 ...              14 15  16  17  18
///        | | | |                  |  |   |   |   |
/// $--GSA,a,a,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x.x,x.x,x.x*hh<CR><LF>
///
/// Field Number:
///  1) Selection mode of 2D or 3D fix (A=automatic, M=manual, forced to operate in 2D or 3D)
///  2) Mode of fix: 1 = no fix, 2 = 2D fix, 3 = 3D fix
///  3) ID of 1st satellite used for fix
///  4) ID of 2nd satellite used for fix
///  ...
///  14) ID of 12th satellite used for fix
///  15) PDOP positional (lat/long/height) dilution of precision (unitless)
///  16) HDOP horizontal dilution of precision
///  17) VDOP vertical dilution of precision
/// \endverbatim
void DataGSA::Parse(const SentenceInfo& sentence)
{
   ATLASSERT(sentence.Type() == NMEA0183::typeGSA);

   if (sentence.FieldCount() != 17 && sentence.FieldCount() != 18)
      throw ParserException(_T("wrong field count"), sentence.Sentence(), __FILE__, __LINE__);

   // 1: selection mode, can be 'A' (valid data), 'M' (manual)
   CString field = sentence.Field(0);
   if (field != _T("A") && field != _T("M"))
      throw ParserException(_T("invalid selection mode (field 1)"), sentence.Sentence(), __FILE__, __LINE__);

   m_selectionMode = field == _T("A") ? selModeAutomatic : selModeManual;

   // 2: mode of fix
   field = sentence.Field(1);
   // enum T_enFixMode is designed so that field can be casted
   m_fixMode = static_cast<NMEA0183::T_enFixMode>(_tcstoul(field, NULL, 10));

   // 3..14: satellites used for fix
   for (unsigned int i = 3; i <= 14; i++)
   {
      field = sentence.Field(i - 1);
      if (!field.IsEmpty())
         m_satelliteIDs.push_back(static_cast<unsigned int>(_tcstoul(field, NULL, 10)));
   }

   // 15: positional DOP
   m_positionalDOP = _tcstod(sentence.Field(14), NULL);

   // 16: horizontal DOP
   m_horizontalDOP = _tcstod(sentence.Field(15), NULL);

   // 17: vertical DOP
   m_verticalDOP = _tcstod(sentence.Field(16), NULL);

   // 18: FAA mode indicator
   if (sentence.FieldCount() == 18)
      m_faaMode = Helper::ParseFAAMode(sentence.Field(14));
   else
      m_faaMode = modeUnknown;
}

/// \brief parse RMC sentence - Recommended Minimum Navigation Information C
/// \verbatim
///        1         2 3       4 5        6 7   8   9    10  1112
///        |         | |       | |        | |   |   |    |   | |
/// $--RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxx,x.x,a,x*hh<CR><LF>
///
///Field Number:
///  1) UTC Time
///  2) Status, A = active, V = void (Navigation receiver warning)
///  3) Latitude
///  4) N or S
///  5) Longitude
///  6) E or W
///  7) Speed over ground, knots
///  8) Track made good, degrees true
///  9) Date, ddmmyy
/// 10) Magnetic Variation, degrees
/// 11) E or W
/// 12) FAA Mode (A=autonomous, D=differential, E=estimated, N=not valid, S=simulator)
///     (NMEA 0183 version 2.3 only)
/// \endverbatim
void DataRMC::Parse(const SentenceInfo& sentence)
{
   ATLASSERT(sentence.Type() == NMEA0183::typeRMC);

   if (sentence.FieldCount() != 11 && sentence.FieldCount() != 12)
      throw ParserException(_T("wrong field count"), sentence.Sentence(), __FILE__, __LINE__);

   // 1/9: UTC Date/Time (1 has time, 9 has date)
   m_datetime = Helper::ParseUTCDateTime(sentence.Field(8), sentence.Field(0));

   // 2: GPS status
   CString field = sentence.Field(1);
   if (field != _T("A") && field != _T("V"))
      throw ParserException(_T("invalid GPS status (field 2)"), sentence.Sentence(), __FILE__, __LINE__);

   m_gpsStatus = field == _T("A") ? statusActive : statusVoid;

   // 3/4: latitude, 5/6: longitude
   m_coordinate = Helper::ParseLatLong(
      sentence.Field(2), sentence.Field(3).IsEmpty() ? 0 : sentence.Field(3)[0],
      sentence.Field(4), sentence.Field(5).IsEmpty() ? 0 : sentence.Field(5)[0]);

   // 7: speed over ground, knots
   field = sentence.Field(6);
   double speedKnots = _tcstod(field, NULL);

   // convert from knots to km/h
   // 1 knot is the same as 1 nautical mile per hour, or 1.852 km/h
   m_speedOverGround = speedKnots * 1.852;

   // 8: track angle made good
   field = sentence.Field(7);
   m_trackMadeGood = _tcstod(field, NULL);

   // 10/11: magnetic variation and angle direction
   field = sentence.Field(9);
   if (!field.IsEmpty())
   {
      m_magneticVariation = _tcstod(field, NULL);

      field = sentence.Field(10);
      if (field != _T("E") && field != _T("W"))
         throw ParserException(_T("invalid magnetic variation reference (field 11)"), sentence.Sentence(), __FILE__, __LINE__);

      if (field == _T("W"))
         m_magneticVariation *= -1.0;
   }

   // 13: FAA mode indicator
   if (sentence.FieldCount() == 12)
      m_faaMode = Helper::ParseFAAMode(sentence.Field(11));
   else
      m_faaMode = modeUnknown;
}

/// \brief parse VTG sentence - Track made good and Ground speed
/// \verbatim
///        1   2 3   4 5   6 7   8 9
///        |   | |   | |   | |   | |
/// $--VTG,x.x,T,x.x,M,x.x,N,x.x,K,x*hh<CR><LF>
///
/// Field Number:
///  1) Track Degrees true north
///  2) T = True (always)
///  3) Track Degrees magnetic
///  4) M = Magnetic (always)
///  5) Speed over ground in knots
///  6) N = Knots
///  7) Speed over ground Kilometers Per Hour
///  8) K = Kilometers Per Hour
///  9) FAA mode indicator (only version 2.3)
///     A=autonomous, D=differential, E=Estimated, N=not valid, S=Simulator, etc.
///     (NMEA 0183 version 2.3 only)
///
/// Note: in some older versions of NMEA 0183, the sentence looks like this:
///
///        1   2  3   4
///        |   |  |   |
/// $--VTG,x.x,x,x.x,x.x,*hh<CR><LF>
///
/// Field Number:
///  1) True course over ground (degrees) 000 to 359
///  2) Magnetic course over ground 000 to 359
///  3) Speed over ground (knots) 00.0 to 99.9
///  4) Speed over ground (kilometers) 00.0 to 99.9
///
///The two forms can be distinguished by field 2, which will be
///the fixed text 'T' in the newer form.  The new form appears
///to have been introduced with NMEA 3.01 in 2002.
/// \endverbatim
void DataVTG::Parse(const SentenceInfo& sentence)
{
   ATLASSERT(sentence.Type() == NMEA0183::typeVTG);

   // determine struct version
   if (sentence.FieldCount() == 4 && sentence.Field(1) != _T("T"))
   {
      // old version
      // 1: track degrees made true
      CString field = sentence.Field(0);
      m_trackDegreesTrue = _tcstod(field, NULL);

      // 2: track degrees magnetic
      field = sentence.Field(1);
      m_trackDegreesMagnetic = _tcstod(field, NULL);

      // 3: speed in knots
      field = sentence.Field(2);
      m_speedOverGroundKnots = _tcstod(field, NULL);

      // 4: speed in km/h
      field = sentence.Field(3);
      m_speedOverGround = _tcstod(field, NULL);
   }
   else
      if ((sentence.FieldCount() == 8 || sentence.FieldCount() == 9) && sentence.Field(1) == _T("T"))
      {
         // new version

         // check fixed fields, 2, 4, 6, 8
         if (sentence.Field(1) != _T("T") ||
            sentence.Field(3) != _T("M") ||
            sentence.Field(5) != _T("N") ||
            sentence.Field(7) != _T("K"))
            throw ParserException(_T("invalid VTG sentence units (fields 2, 4, 6, 8)"), sentence.Sentence(), __FILE__, __LINE__);

         // 1: track degrees made true
         CString field = sentence.Field(0);
         m_trackDegreesTrue = _tcstod(field, NULL);

         // 3: track degrees magnetic
         field = sentence.Field(2);
         m_trackDegreesMagnetic = _tcstod(field, NULL);

         // 5: speed in knots
         field = sentence.Field(4);
         m_speedOverGroundKnots = _tcstod(field, NULL);

         // 7: speed in km/h
         field = sentence.Field(6);
         m_speedOverGround = _tcstod(field, NULL);

         // 9: FAA mode indicator
         if (sentence.FieldCount() == 9)
            m_faaMode = Helper::ParseFAAMode(sentence.Field(8));
         else
            m_faaMode = modeUnknown;
      }
      else
         throw ParserException(_T("wrong field count"), sentence.Sentence(), __FILE__, __LINE__);
}

/// \brief parse GSV sentence - Satellites in view
/// \verbatim
///        1 2 3 4 5 6 7     n
///        | | | | | | |     |
/// $--GSV,x,x,x,x,x,x,x,...*hh<CR><LF>
///
/// Field Number:
///  1) total number of messages
///  2) message number
///  3) satellites in view
///  4) satellite number (PRN number)
///  5) elevation in degrees
///  6) azimuth in degrees to true
///  7) SNR in dB (range 0..99); 0 often means "visible but not being tracked"
///  more satellite infos like 4)-7)
///  n) checksum
/// \endverbatim
void DataGSV::Parse(const SentenceInfo& sentence)
{
   ATLASSERT(sentence.Type() == NMEA0183::typeGSV);

   // field count must be 3 + 4 * n; may be: 7, 11, 15, 19, 23, etc.
   if ((sentence.FieldCount() - 3) % 4 != 0)
      throw ParserException(_T("wrong field count"), sentence.Sentence(), __FILE__, __LINE__);

   // 1: total number of messages
   CString field = sentence.Field(0);
   unsigned int maxMessageNumber = static_cast<unsigned int>(_tcstoul(field, NULL, 10));

   // max message count must be non-zero
   if (maxMessageNumber < 1)
      throw ParserException(_T("invalid GSV message count (field 1)"), sentence.Sentence(), __FILE__, __LINE__);

   m_maxSentenceNr = maxMessageNumber;

   // 2: message number
   field = sentence.Field(1);
   unsigned int messageNumber = static_cast<unsigned int>(_tcstoul(field, NULL, 10));

   // message number is zero or message number higher than max count of messages?
   if (messageNumber == 0 || messageNumber > maxMessageNumber)
      throw ParserException(_T("invalid GSV message index (field 2)"), sentence.Sentence(), __FILE__, __LINE__);

   m_sentenceNr = messageNumber;

   // when message number is 1, a new message starts
   if (messageNumber == 1)
      m_satelliteInfos.clear();

   // 3: satellites in view
   field = sentence.Field(2);
   m_satellitesInView = static_cast<unsigned int>(_tcstoul(field, NULL, 10));

   // 4..7, 8..11, etc.: satellite infos
   unsigned int maxIndex = (sentence.FieldCount() - 3) / 4;

   for (unsigned int index = 0; index < maxIndex; index++)
   {
      // 4: satellite ID or PRN
      field = sentence.Field(3 + index * 4);

      GPS::SatelliteInfo satInfo(static_cast<unsigned int>(_tcstoul(field, NULL, 10)));

      // 5: elevation
      field = sentence.Field(4 + index * 4);
      satInfo.Elevation(static_cast<unsigned int>(_tcstoul(field, NULL, 10)));

      // 6: azimuth
      field = sentence.Field(5 + index * 4);
      satInfo.Azimuth(static_cast<unsigned int>(_tcstoul(field, NULL, 10)));

      // 7: signal to noise ratio
      field = sentence.Field(6 + index * 4);
      satInfo.SignalNoiseRatio(static_cast<unsigned int>(_tcstoul(field, NULL, 10)));

      m_satelliteInfos.push_back(satInfo);
   }
}

///\brief parse GLL sentence - Geographic Position - Latitude/Longitude
/// \verbatim
///        1       2 3        4 5         6 7
///        |       | |        | |         | |
/// $--GLL,llll.ll,a,yyyyy.yy,a,hhmmss.ss,a,m*hh<CR><LF>
///
/// Field Number:
///  1) Latitude
///  2) N or S (North or South)
///  3) Longitude
///  4) E or W (East or West)
///  5) Universal Time Coordinated (UTC)
///  6) Status A - Data Valid, V - Data Invalid
///  7) FAA mode indicator (NMEA 2.3 and later)
///
///Introduced in NMEA 3.0.
/// \endverbatim
void DataGLL::Parse(const SentenceInfo& sentence)
{
   ATLASSERT(sentence.Type() == NMEA0183::typeGLL);

   if (sentence.FieldCount() != 6 && sentence.FieldCount() != 7)
      throw ParserException(_T("wrong field count"), sentence.Sentence(), __FILE__, __LINE__);

   // 1/2: latitude, 3/4: longitude
   m_coordinate = Helper::ParseLatLong(
      sentence.Field(0), sentence.Field(1).IsEmpty() ? 0 : sentence.Field(1)[0],
      sentence.Field(2), sentence.Field(3).IsEmpty() ? 0 : sentence.Field(3)[0]);

   // 5: UTC time
   m_time = Helper::ParseUTCTime(sentence.Field(4));

   // 6: GPS status
   CString field = sentence.Field(5);
   if (field != _T("A") && field != _T("V"))
      throw ParserException(_T("invalid GPS status (field 6)"), sentence.Sentence(), __FILE__, __LINE__);

   m_gpsStatus = field == _T("A") ? statusActive : statusVoid;

   // 7: FAA mode indicator
   if (sentence.FieldCount() == 7)
      m_faaMode = Helper::ParseFAAMode(sentence.Field(6));
   else
      m_faaMode = modeUnknown;
}

///\brief parse GNS sentence - Fix data
/// \verbatim
///        1         2       3 4        5 6    7  8   9   10  11  12  13
///        |         |       | |        | |    |  |   |   |   |   |   |
/// $--GNS,hhmmss.ss,llll.ll,a,yyyyy.yy,a,c--c,xx,x.x,x.x,x.x,x.x,x.x*hh
///
/// Field Number:
///  1) Universal Time Coordinated (UTC)
///  2) Latitude
///  3) N or S (North or South)
///  4) Longitude
///  5) E or W (East or West)
///  6) FAA Mode indicator; first char is for GPS, second is for GLONASS
///     N = Constellation not in use, or no valid fix
///     A = Autonomous (non-differential)
///     D = Differential mode
///     P = Precise (no degradation, like Selective Availability)
///     R = Real Time Kinematic
///     F = Float RTK
///     E = Estimated (dead reckoning) Mode
///     M = Manual Input Mode
///     S = Simulator Mode
///  7) Total number of satelites in use, 00-99
///  8) Horizontal Dilution of precision (HDOP)
///  9) Antenna Altitude above/below mean-sea-level (geoid); in meters
/// 10) Geoidal separation, the difference between the WGS-84 earth
///     ellipsoid and mean-sea-level (geoid), "-" (negative) means mean-sea-level
///     below ellipsoid; in meters
/// 11) Age of differential GPS data, time in seconds since last SC104
///     type 1 or 9 update, null field when DGPS is not used
/// 12) Differential reference station ID, 0000-1023 (may be null field)
///
/// \endverbatim
void DataGNS::Parse(const SentenceInfo& sentence)
{
   ATLASSERT(sentence.Type() == NMEA0183::typeGNS);

   if (sentence.FieldCount() != 12)
      throw ParserException(_T("wrong field count"), sentence.Sentence(), __FILE__, __LINE__);

   // 1: UTC time
   m_time = Helper::ParseUTCTime(sentence.Field(0));

   // 2/3: latitude, 4/5: longitude
   m_coordinate = Helper::ParseLatLong(
      sentence.Field(1), sentence.Field(2).IsEmpty() ? 0 : sentence.Field(2)[0],
      sentence.Field(3), sentence.Field(4).IsEmpty() ? 0 : sentence.Field(4)[0]);

   // 6: FAA mode indicator
   CString field = sentence.Field(5);
   for (int index = 0; index < field.GetLength(); index++)
   {
      m_faaModeList.push_back(Helper::ParseFAAMode(field.Mid(index, 1)));
   }

   // 7: number of satellites in use
   m_numberOfSatellitesInUse = _tcstoul(sentence.Field(6), NULL, 10);

   // 8: horizontal DOP
   m_horizontalDOP = _tcstod(sentence.Field(7), NULL);

   // 9: altitude above geoid
   m_altitudeAboveMeanSeaLevel = _tcstod(sentence.Field(8), NULL);

   // 10: geoidal separation
   m_altitudeGeoidalSeparation = _tcstod(sentence.Field(9), NULL);

   // 11: age of DGPS data
   m_DGPSDataAge = static_cast<unsigned int>(_tcstoul(sentence.Field(10), NULL, 10));

   // 12: DGPS reference station number (0000...1023)
   m_DGPSReferenceStation = static_cast<unsigned int>(_tcstoul(sentence.Field(11), NULL, 10));
}

/// \brief parse P sentence - Private data
/// \verbatim
///   1  2
///   |  |
/// $PXXXxxx*hh<CR><LF>
///
/// Field Number:
///  1) Manufacturer mnemonic code (e.g. SRF for Sirf)
///  2) free form field
/// \endverbatim
void DataP::Parse(const SentenceInfo& sentence)
{
   ATLASSERT(sentence.Type() == NMEA0183::typePrivate);

   if (sentence.FieldCount() == 0)
      throw ParserException(_T("wrong field count"), sentence.Sentence(), __FILE__, __LINE__);

   CString field = sentence.Field(0);
   m_manufacturerCode = field.Left(3);
   field.Delete(0, 3);
   m_fields.push_back(field);

   // add remaining fields
   for (unsigned int index = 1, maxIndex = sentence.FieldCount(); index < maxIndex; index++)
      m_fields.push_back(sentence.Field(index));
}
