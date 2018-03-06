//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file Import/TrackImport.cpp track import
//
#include "stdafx.h"
#include "TrackImport.hpp"
#include "GPXTrackImport.hpp"
#include "NMEA0183TrackImport.hpp"
#include <ulib/Path.hpp>
#include <ulib/Exception.hpp>

using Import::TrackImport;

void TrackImport::ImportTrack(LPCTSTR filename, GPS::Track& track)
{
   CString extension = Path(filename).ExtensionOnly();
   extension.MakeLower();

   if (extension == _T(".gpx"))
   {
      GPXTrackImport gpxImport(filename);
      gpxImport.ImportTrack(track);
   }
   else if (extension == _T(".txt") || extension == _T(".nmea"))
   {
      NMEA0183TrackImport nmea0183Import(filename);
      nmea0183Import.ImportTrack(track);
   }
   else
      throw Exception(_T("unknown file format"), __FILE__, __LINE__);
}
