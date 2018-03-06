//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006-2018 Michael Fink
//
/// \file Import/TrackImport.hpp track import
//
#pragma once

namespace GPS
{
   class Track;
}

namespace Import
{
   /// track import for various supported formats
   class TrackImport
   {
   public:
      /// imports track; format is determined from file extension
      static void ImportTrack(LPCTSTR filename, GPS::Track& track);

   private:
      TrackImport() = delete;
      TrackImport(const TrackImport&) = delete;
   };

} // namespace Import
