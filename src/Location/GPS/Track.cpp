//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file Track.cpp WGS84 coordinate
//
#include "stdafx.h"
#include "Track.hpp"

using GPS::Track;

Track::Track()
{
}

size_t Track::NumPoints() const
{
   return 0;
}

/// \todo implement
DateTime Track::StartTime() const
{
   return DateTime(DateTime::invalid);
}

/// \todo implement
DateTime Track::EndTime() const
{
   return DateTime(DateTime::invalid);
}

/// \todo implement
void Track::AddPoint(const GPS::WGS84::Coordinate& coordinate, const DateTime& timeStamp)
{
   UNUSED(timeStamp);
   UNUSED(coordinate);
}

/// \todo implement
std::pair<GPS::WGS84::Coordinate, DateTime> Track::FindNearest(const DateTime& timeStamp) const
{
   UNUSED(timeStamp);

   return std::make_pair(GPS::WGS84::Coordinate(), DateTime(DateTime::invalid));
}
