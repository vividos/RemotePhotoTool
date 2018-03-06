//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file Import/GPXTrackImport.cpp GPX track import
//
#include "stdafx.h"
#include "GPXTrackImport.hpp"

using Import::GPXTrackImport;

GPXTrackImport::GPXTrackImport(LPCTSTR filename)
   :m_filename(filename)
{
}

void GPXTrackImport::ImportTrack(GPS::Track& track)
{
   UNUSED(track);

   // TODO implement
}
