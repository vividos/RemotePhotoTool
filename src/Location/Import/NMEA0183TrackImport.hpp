//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file Import/NMEA0183TrackImport.hpp NMEA 0183 track import
//
#pragma once

#include "NMEA0183/Parser.hpp"

namespace GPS
{
   class Track;
}

namespace Import
{
   /// track import for NMEA 0183 format
   class NMEA0183TrackImport
   {
   public:
      /// ctor
      NMEA0183TrackImport(LPCTSTR filename);

      /// imports track
      void ImportTrack(GPS::Track& track);

   private:
      /// filename of track to import
      CString m_filename;

      /// NMEA 0183 parser
      NMEA0183::Parser m_parser;
   };

} // namespace Import
