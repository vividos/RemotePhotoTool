//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2026 Michael Fink
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
   return m_trackPoints.size();
}

DateTime Track::StartTime() const
{
   return m_trackPoints.empty() ? DateTime(DateTime::T_enStatus::invalid) : m_trackPoints.front().m_timeStamp;
}

DateTime Track::EndTime() const
{
   return m_trackPoints.empty() ? DateTime(DateTime::T_enStatus::invalid) : m_trackPoints.back().m_timeStamp;
}

void Track::AddPoint(const GPS::WGS84::Coordinate& coordinate, const DateTime& timeStamp)
{
   m_trackPoints.push_back(TrackPoint(coordinate, timeStamp));
}

std::pair<GPS::WGS84::Coordinate, DateTime> Track::FindNearest(const DateTime& timeStamp) const
{
   // use ordered_unique index to find lower bound of given time stamp
   auto iter = m_trackPoints.get<1>().lower_bound(timeStamp);

   if (iter == m_trackPoints.get<1>().end())
   {
      return std::make_pair(GPS::WGS84::Coordinate(), DateTime(DateTime::T_enStatus::invalid));
   }

   return std::make_pair(iter->m_coord, iter->m_timeStamp);
}

bool Track::InTrackRange(const DateTime& timeStamp) const
{
   return timeStamp.Status() == DateTime::T_enStatus::valid &&
      !m_trackPoints.empty() &&
      timeStamp >= StartTime() &&
      timeStamp <= EndTime();
}
