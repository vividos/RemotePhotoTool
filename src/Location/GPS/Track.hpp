//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2020 Michael Fink
//
/// \file Coordinate.hpp WGS84 coordinate
//
#pragma once

#include "GPS/Coordinate.hpp"
#include <ulib/DateTime.hpp>
#pragma warning(push)
#pragma warning(disable: 26495)
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/member.hpp>
#pragma warning(pop)

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

      /// checks if given time stamp is contained in track range
      bool InTrackRange(const DateTime& timeStamp) const;

   private:
      /// value type to hold datetime and coordinate
      struct TrackPoint
      {
         /// ctor
         TrackPoint(const GPS::WGS84::Coordinate& coord, const DateTime& timeStamp)
            :m_coord(coord),
            m_timeStamp(timeStamp)
         {
         }

         GPS::WGS84::Coordinate m_coord;  ///< stored coordinate
         DateTime m_timeStamp;            ///< time stamp
      };

      // type of multi index container used for storage
      typedef boost::multi_index::multi_index_container<
         TrackPoint, // value type
         boost::multi_index::indexed_by<
            boost::multi_index::sequenced<>,    // std::list like
            boost::multi_index::ordered_unique<
               boost::multi_index::member<TrackPoint, DateTime, &TrackPoint::m_timeStamp> // order by member m_timeStamp
            >
         >
      > T_TrackPointsContainer;

      /// track points
      T_TrackPointsContainer m_trackPoints;
   };

} // namespace GPS
