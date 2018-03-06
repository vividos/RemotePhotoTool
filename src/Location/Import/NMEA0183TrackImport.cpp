//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file Import/NMEA0183TrackImport.cpp NMEA 0183 track import
//
#include "stdafx.h"
#include "NMEA0183TrackImport.hpp"
#include <ulib/stream/TextFileStream.hpp>
#include "GPS/Track.hpp"

using Import::NMEA0183TrackImport;

NMEA0183TrackImport::NMEA0183TrackImport(LPCTSTR filename)
   :m_filename(filename)
{
}

void NMEA0183TrackImport::ImportTrack(GPS::Track& track)
{
   Stream::TextFileStream stream(
      m_filename,
      Stream::FileStream::EFileMode::modeOpen,
      Stream::FileStream::EFileAccess::accessRead,
      Stream::FileStream::EFileShare::shareReadWrite,
      Stream::ITextStream::textEncodingAnsi,
      Stream::ITextStream::lineEndingReadAny);

   ATLASSERT(true == stream.CanRead());

   if (!stream.IsOpen())
      return;

   m_parser.ResetUpdateFlags();

   DateTime lastTimeStamp;

   CString line;
   do
   {
      stream.ReadLine(line);
      m_parser.ParseLine(line);
      if (m_parser.UpdatedPositionInfo() &&
         m_parser.UpdatedDateTime() &&
         lastTimeStamp != m_parser.GetPositionInfo().TimeStamp())
      {
         const GPS::PositionInfo& positionInfo = m_parser.GetPositionInfo();

         track.AddPoint(positionInfo.Coordinate(), positionInfo.TimeStamp());

         m_parser.ResetUpdateFlags();
      }

   } while (!stream.AtEndOfStream());
}
