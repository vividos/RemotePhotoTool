//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file JpegGeoTagger.cpp JPEG Geo-tagger
//
#include "stdafx.h"
#include "JpegGeoTagger.hpp"
#include <ulib/Exception.hpp>
#include <ulib/stream/FileStream.hpp>
#include <ulib/stream/EndianAwareFilter.hpp>
#include "Exif.hpp"

void JpegGeoTagger::GeoTagFile(LPCTSTR filename, T_fnFindCoordinateByDate fnFindCoordinateByDate)
{
   Stream::FileStream streamIn(
      filename,
      Stream::FileStream::modeOpen,
      Stream::FileStream::accessRead,
      Stream::FileStream::shareRead);

   CString outputFilename = CString(filename) + _T(".geotagtemp");

   Stream::FileStream streamOut(
      outputFilename,
      Stream::FileStream::modeCreateNew,
      Stream::FileStream::accessWrite,
      Stream::FileStream::shareRead);

   JpegGeoTagger geoTagger(streamIn, streamOut, fnFindCoordinateByDate);
   geoTagger.Start();

   // TODO rename source file
   // TODO rename output file
   // TODO delete source file
   // TODO transfer file date attributes
}

JpegGeoTagger::JpegGeoTagger(Stream::IStream& streamIn, Stream::IStream& streamOut, T_fnFindCoordinateByDate fnFindCoordinateByDate)
   :JFIFRewriter(streamIn, streamOut),
   m_fnFindCoordinateByDate(fnFindCoordinateByDate)
{
}

void JpegGeoTagger::OnBlock(BYTE marker, WORD length)
{
   if (marker != JFIFRewriter::APP1)
   {
      JFIFRewriter::OnBlock(marker, length);
      return;
   }

   // read exif tag
   std::vector<BYTE> exifData(length);
   DWORD numReadBytes = 0;
   if (!m_streamIn.Read(&exifData[0], length, numReadBytes) || length != numReadBytes)
      throw Exception(_T("couldn't read exif data from APP1 block"), __FILE__, __LINE__);

   DateTime imageCreateDateTime;
   try
   {
      imageCreateDateTime = ReadDateTimeExif(exifData);

      // find geo coordinates
      GPS::WGS84::Coordinate coord = m_fnFindCoordinateByDate(imageCreateDateTime);

#ifdef _DEBUG
      CString textLatitude, textLongitude;
      coord.ToString(GPS::WGS84::Coordinate::fmtDDD_MM_aph_ss_dot_sss, textLatitude, textLongitude);
      ATLTRACE(_T(" found coordinates: %s, %s\n"), textLatitude.GetString(), textLongitude.GetString());
#endif

      // write exif tag
      if (coord.IsValid())
      {
         AddExifGPSInfo(exifData, coord);
      }
      else
      {
         JFIFRewriter::OnBlock(marker, length);
         return;
      }
   }
   catch (const Exception& ex)
   {
      ATLTRACE(_T(" error geotagging: %s\n"), ex.Message().GetString());
      throw;
   }

   if (exifData.size() + 2 > 0xffff)
      throw Exception(_T("exif data to write is too large"), __FILE__, __LINE__);

   Stream::EndianAwareFilter endianFilterOut(m_streamOut);
   endianFilterOut.Write16BE(static_cast<WORD>(exifData.size() + 2));

   // write out data
   DWORD numWrittenBytes = 0;
   m_streamOut.Write(&exifData[0], static_cast<DWORD>(exifData.size() + 2), numWrittenBytes);
   ATLASSERT(numWrittenBytes == exifData.size() + 2);
}

DateTime JpegGeoTagger::ReadDateTimeExif(const std::vector<BYTE>& data)
{
   // load Exif data
   Exif::Data d(&data[0], static_cast<unsigned int>(data.size()));
   if (d.IsContentIfdAvail(EXIF_IFD_EXIF))
   {
      Exif::Content c = d.GetContent(EXIF_IFD_EXIF);

      Exif::Entry entryDateTime = c.GetEntry(EXIF_TAG_DATE_TIME_ORIGINAL);
      if (EXIF_FORMAT_ASCII != entryDateTime.Format())
         _tprintf(_T("wrong date/time format\n"));
      else
      {
         CString textDateTime = entryDateTime.GetAscii();
         _tprintf(_T("date/time: %s\n"), textDateTime.GetString());

         // parse date/time, e.g.: 2007:02:17 11:00:58
         DateTime dateTime(
            static_cast<unsigned int>(_tcstoul(textDateTime.Left(4), NULL, 10)),
            static_cast<unsigned int>(_tcstoul(textDateTime.Mid(5, 2), NULL, 10)),
            static_cast<unsigned int>(_tcstoul(textDateTime.Mid(8, 2), NULL, 10)),
            static_cast<unsigned int>(_tcstoul(textDateTime.Mid(11, 2), NULL, 10)),
            static_cast<unsigned int>(_tcstoul(textDateTime.Mid(14, 2), NULL, 10)),
            static_cast<unsigned int>(_tcstoul(textDateTime.Mid(17, 2), NULL, 10)));

         return dateTime;
      }
   }

   throw Exception(_T("couldn't extract original date/time from exif data"), __FILE__, __LINE__);
}

void JpegGeoTagger::AddExifGPSInfo(std::vector<BYTE>& data, const GPS::WGS84::Coordinate& coord)
{
   Exif::Data d(&data[0], static_cast<unsigned int>(data.size()));
   if (d.IsContentIfdAvail(EXIF_IFD_EXIF))
   {
      Exif::Content c = d.GetContent(EXIF_IFD_GPS);

      // write latitude
      {
         Exif::Entry entryLatitudeRef = c.CreateEntry(ExifTag(EXIF_TAG_GPS_LATITUDE_REF));
         entryLatitudeRef.SetAscii(coord.GetLatitude() >= 0.0 ? "N" : "S");

         Exif::Entry entryLatitude = c.CreateEntry(ExifTag(EXIF_TAG_GPS_LATITUDE));
         ExifRational adLatitude[3] =
         {
            { static_cast<ExifLong>(coord.GetDegreeLatitude()), 1},
            { coord.GetMinuteLatitude(), 1},
            { coord.GetSecondLatitude() * 1000 + coord.GetMilliSecondLatitude(), 1000}
         };
         entryLatitude.SetRational(adLatitude, 3);
      }

      // write longitude
      {
         Exif::Entry entryLongitudeRef = c.CreateEntry(ExifTag(EXIF_TAG_GPS_LONGITUDE_REF));
         entryLongitudeRef.SetAscii(coord.GetLongitude() >= 0.0 ? "E" : "W");

         Exif::Entry entryLongitude = c.CreateEntry(ExifTag(EXIF_TAG_GPS_LONGITUDE));
         ExifRational adLongitude[3] =
         {
            { static_cast<ExifLong>(coord.GetDegreeLongitude()), 1},
            { coord.GetMinuteLongitude(), 1},
            { coord.GetSecondLongitude() * 1000 + coord.GetMilliSecondLongitude(), 1000}
         };
         entryLongitude.SetRational(adLongitude, 3);
      }

      // write altitude TODO
      {
         //Exif::Entry entryAltitudeRef = c.CreateEntry(ExifTag(EXIF_TAG_GPS_ALTITUDE_REF));
         //Exif::Entry entryAltitude = c.CreateEntry(ExifTag(EXIF_TAG_GPS_ALTITUDE));
      }

      d.Save(data);
   }
}
