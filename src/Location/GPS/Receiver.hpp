//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file Receiver.hpp GPS receiver class
//
#pragma once

/// \brief GPS related classes
namespace GPS
{
   class PositionInfo;
   class SatelliteInfo;
   class Track;

   /// live GPS receiver
   class Receiver
   {
   public:
      /// ctor
      Receiver();
      /// dtor; stops receiving
      ~Receiver();

      /// configures receiver to use given serial port
      void Configure(const CString& serialPortDeviceName);

      /// starts receiving; points are store in the given track
      void Start(Track& track);

      /// stops receiving
      void Stop();

      /// current position info
      const PositionInfo& GetPositionInfo() const;

      /// current satellite infos
      const std::vector<SatelliteInfo>& GetSatelliteInfos() const;

   private:
      /// runs I/O worker thread
      void RunWorkerThread();

      /// called when data from serial port has been received
      void OnReceivedData(int errorCode, const std::vector<BYTE>& data, size_t bytesTransferred);

      /// analyzes receive buffer for new NMEA sentences
      void AnalyzeBuffer();

   private:
      struct Impl;

      /// receiver implementation
      std::unique_ptr<Impl> m_impl;
   };

} // namespace GPS
