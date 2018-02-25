//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file Coordinate.hpp WGS84 coordinate
//
#pragma once

#include <ulib/DateTime.hpp>
#include "GPS/Coordinate.hpp"

namespace GPS
{
   /// Track of multiple coordinate points, forming a line string
   class Track
   {
   public:
      /// ctor
      Track();

      /// returns number of points in track
      size_t NumPoints() const;

      /// returns the start time of the track; returns invalid date when track contains no points
      DateTime StartTime() const;

      /// returns the start time of the track
      DateTime EndTime() const;

      /// adds a new coordinate point
      void AddPoint(const WGS84::Coordinate& coordinate, const DateTime& timeStamp);

      /// finds nearest WGS84 coordiates and time stamp, relative to given time stamp
      std::pair<WGS84::Coordinate, DateTime> FindNearest(const DateTime& timeStamp) const;

   private:
      //boost::multi_index_container<> m_trackPoints
   };

} // namespace GPS
