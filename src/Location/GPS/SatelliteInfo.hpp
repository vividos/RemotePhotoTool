//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SatelliteInfo.hpp Satellite info
//
#pragma once

namespace GPS
{
   /// \brief infos about one GPS satellite
   /// infos may be extracted from the following NMEA sentences: GSA, GSV
   class SatelliteInfo
   {
   public:
      /// ctor
      SatelliteInfo(unsigned int satID,
         unsigned int azimuth = 0,
         unsigned int elevation = 0,
         unsigned int signalNoiseRatio = 0,
         bool usedForFix = false)
         :m_satID(satID),
         m_usedForFix(usedForFix),
         m_azimuth(azimuth),
         m_elevation(elevation),
         m_signalNoiseRatio(signalNoiseRatio)
      {}

      // get methods

      /// returns satellite id
      unsigned int SatID() const { return m_satID; }

      /// returns if sat is used for fix
      bool UsedForFix() const { return m_usedForFix; }

      /// returns azimuth angle of sat position
      unsigned int Azimuth() const { return m_azimuth; }

      /// returns elevation angle of sat position
      unsigned int Elevation() const { return m_elevation; }

      /// returns signal to noise ratio
      unsigned int SignalNoiseRatio() const { return m_signalNoiseRatio; }

      // set methods

      /// sets flag if sat was used for fix
      void UsedForFix(bool usedForFix) { m_usedForFix = usedForFix; }

      /// sets azimuth angle of sat position
      void Azimuth(unsigned int azimuth) { m_azimuth = azimuth; }

      /// sets elevation angle of sat position
      void Elevation(unsigned int elevation) { m_elevation = elevation; }

      /// returns signal to noise ratio
      void SignalNoiseRatio(unsigned int signalNoiseRatio) { m_signalNoiseRatio = signalNoiseRatio; }

      // compare operator

      /// less comparison, by sat id
      bool operator<(const SatelliteInfo& rhs) const
      {
         return m_satID < rhs.m_satID;
      }

   private:
      unsigned int m_satID;       ///< satellite id (or pseudorandom number)
      bool m_usedForFix;          ///< indicates if sat is used for fix
      unsigned int m_azimuth;     ///< azimuth angle of sat pos in the sky; [0°;360°[
      unsigned int m_elevation;   ///< elevation angle of sat pos in the sky; [0°;90°[
      unsigned int m_signalNoiseRatio; ///< signal to noise ratio of sat reception, in db; the higher the better
   };

} // namespace GPS
