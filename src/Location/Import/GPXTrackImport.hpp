//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file Import/GPXTrackImport.hpp GPX track import
//
#pragma once

namespace GPS
{
   class Track;
}

namespace Import
{
   /// track import for GPX format
   class GPXTrackImport
   {
   public:
      /// ctor
      GPXTrackImport(LPCTSTR filename);

      /// imports track
      void ImportTrack(GPS::Track& track);

   private:
      /// filename of track to import
      CString m_filename;
   };

} // namespace Import
