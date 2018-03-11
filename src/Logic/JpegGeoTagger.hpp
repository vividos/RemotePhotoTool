//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file JpegGeoTagger.cpp JPEG Geo-tagger
//
#pragma once

#include "JFIFRewriter.hpp"
#include <ulib/DateTime.hpp>
#include "GPS/Coordinate.hpp"

/// JPEG Geo-tagger helper class
class JpegGeoTagger : private JFIFRewriter
{
public:
   /// function that finds GPS coordinates for a given image creation date; may return invalid coordinates
   typedef std::function<GPS::WGS84::Coordinate(const DateTime& imageCreationDate)> T_fnFindCoordinateByDate;

   /// geo-tags file with given latitude and longitude
   static void GeoTagFile(LPCTSTR filename, T_fnFindCoordinateByDate fnFindCoordinateByDate);

private:
   /// ctor
   JpegGeoTagger(Stream::IStream& streamIn, Stream::IStream& streamOut, T_fnFindCoordinateByDate fnFindCoordinateByDate);
   /// dtor
   virtual ~JpegGeoTagger() {}

   /// called when a new JFIF block is handled
   virtual void OnBlock(BYTE marker, WORD length);

   /// reads date/time of image creation from EXIF tag
   DateTime ReadDateTimeExif(const std::vector<BYTE>& data);

   /// adds EXIF GPS info tag
   void AddExifGPSInfo(std::vector<BYTE>& data, const GPS::WGS84::Coordinate& coord);

private:
   /// function to find coordinate by image creation date
   T_fnFindCoordinateByDate m_fnFindCoordinateByDate;
};
