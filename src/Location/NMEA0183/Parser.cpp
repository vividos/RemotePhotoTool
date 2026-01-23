//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file NMEA0183/Parser.cpp Parser for NMEA 0183 sentences
//
#include "stdafx.h"
#include "Common.hpp"
#include "Data.hpp"
#include "SentenceInfo.hpp"
#include "Parser.hpp"
#include "ParserException.hpp"
#include <cmath>
#include "GPS/PositionInfo.hpp"
#include "GPS/SatelliteInfo.hpp"

using NMEA0183::Parser;
using NMEA0183::T_enSentenceType;
using NMEA0183::SentenceInfo;

Parser::Parser()
   :m_updatedPositionInfo(false),
   m_updatedDateTime(false),
   m_updatedSatelliteInfos(false)
{
}

void Parser::ParseLine(LPCTSTR line)
{
   try
   {
      // parse sentence
      SentenceInfo sentence = ParseSentenceInfo(line);

      // parse specific data
      switch (sentence.Type())
      {
      case typeGGA:
         OnSentenceTypeGGA(sentence);
         break;

      case typeGSA:
         OnSentenceTypeGSA(sentence);
         break;

      case typeRMC:
         OnSentenceTypeRMC(sentence);
         break;

      case typeVTG:
         OnSentenceTypeVTG(sentence);
         break;

      case typeGSV:
         OnSentenceTypeGSV(sentence);
         break;

      case typeGLL:
         OnSentenceTypeGLL(sentence);
         break;

      case typeGNS:
         OnSentenceTypeGNS(sentence);
         break;

      default:
         ATLTRACE(_T("unknown NMEA sentence type: %u\n"), sentence.Type());
         break;
      }
   }
   catch (ParserException& ex)
   {
      ATLTRACE(_T("parser error: %s(%u): message=%s sentence=%s\n"),
         ex.SourceFile().GetString(),
         ex.SourceLine(),
         ex.Message().GetString(),
         ex.SentenceInfo().GetString());
   }
}

void Parser::ResetUpdateFlags()
{
   m_updatedPositionInfo = false;
   m_updatedDateTime = false;
   m_updatedSatelliteInfos = false;
}

/// note that all sentences according to NMEA 0183 must be up to 82 characters
/// long, but this fact isn't being checked.
SentenceInfo Parser::ParseSentenceInfo(LPCTSTR line)
{
   // starts with proper string?
   if (_tcsncmp(_T("$GP"), line, 3) != 0 && // GPS
      _tcsncmp(_T("$GL"), line, 3) != 0 && // GLONASS
      _tcsncmp(_T("$GA"), line, 3) != 0 && // Galileo
      _tcsncmp(_T("$GN"), line, 3) != 0 && // GNSS (multiple systems)
      _tcsncmp(_T("$P"), line, 2) != 0)
      throw ParserException(_T("unknown sentence type"), line, __FILE__, __LINE__);

   CString strLine(line);

   // calculate checksum
   int pos = strLine.Find(_T('*'));
   if (pos != -1)
   {
      BYTE checksumCalc = Helper::CalculateChecksum(line + 1, pos - 1);
      BYTE checksumRecv = static_cast<BYTE>(_tcstoul(strLine.Mid(pos + 1, 2), NULL, 16) & 0xFF);

      if (checksumCalc != checksumRecv)
      {
         ATLTRACE(_T("checksum mismatch, should be %02x, is %02x\n"), checksumCalc, checksumRecv);
         throw ParserException(_T("checksum mismatch"), line, __FILE__, __LINE__);
      }
   }

   // determine sentence type
   T_enSentenceType type = typeUnknown;
   int start;
   if (_tcsncmp(_T("$G"), line, 2) == 0) // $GP, $GL, $GA, $GN
   {
      if (line[6] != _T(','))
         throw ParserException(_T("invalid NMEA sentence format"), line, __FILE__, __LINE__);

      CString strType(strLine.Mid(3, 3));
      start = 6;

      if (strType == _T("GGA"))        type = typeGGA;
      else if (strType == _T("GSA"))   type = typeGSA;
      else if (strType == _T("RMC"))   type = typeRMC;
      else if (strType == _T("VTG"))   type = typeVTG;
      else if (strType == _T("GSV"))   type = typeGSV;
      else if (strType == _T("GLL"))   type = typeGLL;
      else if (strType == _T("GNS"))   type = typeGNS;
      else
         throw ParserException(_T("unknown $G* sentence type"), line, __FILE__, __LINE__);
   }
   else
      if (_tcsncmp(_T("$P"), line, 2) == 0)
      {
         start = 1;
         type = typePrivate;
      }
      else
         throw ParserException(_T("unknown sentence type"), line, __FILE__, __LINE__);

   SentenceInfo sentence(type);

   sentence.ParseFields(strLine.Mid(start + 1, pos - (start + 1)));

   return sentence;
}

void NMEA0183::Parser::OnSentenceTypeGGA(NMEA0183::SentenceInfo &sentence)
{
   DataGGA data;
   data.Parse(sentence);

   if (data.m_qualityIndicator == qualityFixNotAvail)
      return;

   m_positionInfo.Coordinate(data.m_coordinate);
   m_positionInfo.PositionPrecision(data.m_horizontalDOP);
   m_positionInfo.Altitude(data.m_altitudeAboveMeanSeaLevel);
   m_positionInfo.GeoidHeight(data.m_altitudeGeoidalSeparation);
   m_updatedPositionInfo = true;

   UpdateDateTimeFromTimeSpan(data.m_time);

   // not used values:
   // m_numberOfSatellites // number of satellites in view
}

void Parser::OnSentenceTypeGSA(const SentenceInfo& sentence)
{
   DataGSA data;
   data.Parse(sentence);

   m_setSatIdsForFix.clear();

   if (data.m_fixMode == fixUnknown ||
      data.m_fixMode == fixNone ||
      data.m_faaMode == modeNotValid)
      return;

   // storing satellite id's that are currently used, when needed for later
   for (size_t i = 0, iMax = data.m_satelliteIDs.size(); i < iMax; i++)
   {
      unsigned int uiSatId = data.m_satelliteIDs[i];
      m_setSatIdsForFix.insert(uiSatId);
   }

   m_positionInfo.PositionPrecision(data.m_horizontalDOP);

   // Besides storing, check for sat ids used for fix
   UpdateSatIdsUsedForFix();

   // not used values:
   // m_selectionMode // selection mode
   // m_positionalDOP
   // m_verticalDOP
}

void NMEA0183::Parser::OnSentenceTypeRMC(NMEA0183::SentenceInfo &sentence)
{
   DataRMC data;
   data.Parse(sentence);

   if (data.m_gpsStatus == statusVoid ||
      data.m_faaMode == modeNotValid)
      return;

   m_positionInfo.Coordinate(data.m_coordinate);
   m_positionInfo.Speed(data.m_speedOverGround);
   m_positionInfo.Angle(data.m_trackMadeGood);
   m_positionInfo.AngleMagnetic(data.m_magneticVariation);
   m_updatedPositionInfo = true;

   m_positionInfo.TimeStamp(data.m_datetime);
   m_updatedDateTime = true;
}

void NMEA0183::Parser::OnSentenceTypeVTG(NMEA0183::SentenceInfo &sentence)
{
   DataVTG data;
   data.Parse(sentence);

   if (data.m_faaMode == modeNotValid)
      return;

   m_positionInfo.Speed(data.m_speedOverGround);
   m_updatedPositionInfo = true;

   // not used values:
   // m_trackDegreesTrue // track angle in degrees true
   // m_trackDegreesMagnetic // track angle in degrees magnetic
   // m_speedOverGroundKnots // speed over ground in knots
}

void Parser::OnSentenceTypeGSV(const NMEA0183::SentenceInfo& sentence)
{
   DataGSV data;
   data.Parse(sentence);

   // first GSV sentence? clear list
   if (data.m_sentenceNr == 1)
      m_satelliteInfos.clear();

   for (size_t i = 0, iMax = data.m_satelliteInfos.size(); i < iMax; i++)
      m_satelliteInfos.push_back(data.m_satelliteInfos[i]);

   // last sentence? then check with sat ids used for fix
   if (data.m_sentenceNr >= data.m_maxSentenceNr)
   {
      UpdateSatIdsUsedForFix();

      m_updatedSatelliteInfos = true;
   }
}

void NMEA0183::Parser::OnSentenceTypeGLL(NMEA0183::SentenceInfo &sentence)
{
   DataGLL data;
   data.Parse(sentence);

   if (data.m_gpsStatus == statusVoid ||
      data.m_faaMode == modeNotValid)
      return;

   m_positionInfo.Coordinate(data.m_coordinate);
   m_updatedPositionInfo = true;

   UpdateDateTimeFromTimeSpan(data.m_time);
}

void NMEA0183::Parser::OnSentenceTypeGNS(NMEA0183::SentenceInfo &sentence)
{
   DataGNS data;
   data.Parse(sentence);

   m_positionInfo.Coordinate(data.m_coordinate);
   m_positionInfo.PositionPrecision(data.m_horizontalDOP);
   m_positionInfo.Altitude(data.m_altitudeAboveMeanSeaLevel);
   m_positionInfo.GeoidHeight(data.m_altitudeGeoidalSeparation);
   m_updatedPositionInfo = true;

   UpdateDateTimeFromTimeSpan(data.m_time);

   // not used values:
   // m_faaModeList // FAA mode indicators for GPS, GLONASS
   // m_numberOfSatellitesInUse // number of satellites in use
}

void Parser::UpdateDateTimeFromTimeSpan(const TimeSpan& newTime)
{
   if (m_positionInfo.TimeStamp().Status() == DateTime::T_enStatus::valid)
   {
      DateTime oldTimeStamp = m_positionInfo.TimeStamp();

      DateTime newTimeStamp(
         oldTimeStamp.Year(),
         oldTimeStamp.Month(),
         oldTimeStamp.Day(),
         newTime.Hours(),
         newTime.Minutes(),
         newTime.Seconds());

      m_positionInfo.TimeStamp(newTimeStamp);

      m_updatedDateTime = true;
   }
}

void Parser::UpdateSatIdsUsedForFix()
{
   // Check if the sat ids in this sentence can be found
   // in the sat info list we have stored.
   auto iter = m_satelliteInfos.begin(), stop = m_satelliteInfos.end();
   for (; iter != stop; iter++)
   {
      // found in sat ID set?
      bool usedForFix = m_setSatIdsForFix.find(iter->SatID()) != m_setSatIdsForFix.end();

      if (usedForFix != iter->UsedForFix())
      {
         iter->UsedForFix(usedForFix);
      }
   }
}
