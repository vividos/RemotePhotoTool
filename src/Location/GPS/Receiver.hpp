//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file Receiver.hpp GPS receiver class
//
#pragma once

#include <ulib/Observer.hpp>

class DateTime;

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

      /// subject type for observing position update
      typedef Subject<void(const PositionInfo&)> T_SubjectOnPositionUpdate;
      // subject type for observing date/time update
      typedef Subject<void(const DateTime&)> T_SubjectOnDateTimeUpdate;
      // subject type for observing satellite info update
      typedef Subject<void(const std::vector<SatelliteInfo>&)> T_SubjectOnSatelliteInfoUpdate;

      /// returns subject for observing position update
      T_SubjectOnPositionUpdate& PositionUpdate();

      /// returns subject for observing date/time update
      T_SubjectOnDateTimeUpdate& DateTimeUpdate();

      /// returns subject for observing satellite info update
      T_SubjectOnSatelliteInfoUpdate& SatelliteInfoUpdate();

   private:
      /// runs I/O worker thread
      void RunWorkerThread();

      /// called when data from serial port has been received
      void OnReceivedData(int errorCode, const std::vector<BYTE>& data, size_t bytesTransferred);

      /// analyzes receive buffer for new NMEA sentences
      void AnalyzeBuffer();

      /// notify subscribed subjects of new data
      void NotifySubjects();

   private:
      struct Impl;

      /// receiver implementation
      std::unique_ptr<Impl> m_impl;
   };

} // namespace GPS
