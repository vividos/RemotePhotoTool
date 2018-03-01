//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file Receiver.cpp GPS receiver class
//
#include "stdafx.h"
#include "Receiver.hpp"
#include "Track.hpp"
#include <ulib/DateTime.hpp>
#include "Coordinate.hpp"
#include "SatelliteInfo.hpp"
#include "PositionInfo.hpp"
#include "Serial/SerialPort.hpp"
#include "NMEA0183/Parser.hpp"
#include <ulib/thread/Event.hpp>
#include <ulib/thread/Thread.hpp>
#include <boost/optional.hpp>

using GPS::Receiver;
using GPS::Track;

/// receiver implementation details
struct Receiver::Impl
{
public:
   Impl()
      :m_evtHandlerCalled(false)
   {
   }

   /// name of serial port to use
   CString m_serialPortDeviceName;

   /// Asio I/O service to use for serial port
   boost::asio::io_service m_ioService;

   /// serial port to use for receiving GPS data
   std::unique_ptr<Serial::SerialPort> m_serialPort;

   /// worker thread for I/O
   std::unique_ptr<std::thread> m_workerThread;

   /// event to signal handler function abort
   AutoResetEvent m_evtHandlerCalled;

   /// buffer with received data
   CString m_receiveBuffer;

   /// parser for NMEA 0183 sentences
   std::unique_ptr<NMEA0183::Parser> m_parser;

   /// track to fill with track points
   boost::optional<std::reference_wrapper<Track>> m_track;

   /// current position info
   PositionInfo m_positionInfo;

   /// current satellite infos
   std::vector<SatelliteInfo> m_satelliteInfos;
};

Receiver::Receiver()
{
}

Receiver::~Receiver()
{
   try
   {
      Stop();
   }
   catch (...)
   {
   }
}

void Receiver::Configure(const CString& serialPortDeviceName)
{
   m_impl->m_serialPortDeviceName = serialPortDeviceName;
}

void Receiver::Start(Track& track)
{
   m_impl->m_track = boost::optional<std::reference_wrapper<Track>>(std::ref(track));
   m_impl->m_parser = std::make_unique<NMEA0183::Parser>();

   m_impl->m_serialPort = std::make_unique<Serial::SerialPort>(m_impl->m_serialPortDeviceName, m_impl->m_ioService);

   if (m_impl->m_serialPort->IsOpen())
   {
      m_impl->m_serialPort->Config().SetPredefined(Serial::SerialPortConfig::T_enPredefined::config4800_8N1_none);

      m_impl->m_serialPort->AsyncReceive(
         std::bind(&Receiver::OnReceivedData, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

      m_impl->m_workerThread = std::make_unique<std::thread>(
         std::bind(&Receiver::RunWorkerThread, this));
   }
   else
   {
      m_impl->m_serialPort.reset();
   }
}

void Receiver::Stop()
{
   if (m_impl->m_serialPort != nullptr)
   {
      m_impl->m_serialPort->Close();
   }

   if (m_impl->m_workerThread != nullptr)
   {
      m_impl->m_workerThread->join();
   }

   m_impl->m_parser.reset();
   m_impl->m_track.reset();
}

const GPS::PositionInfo& Receiver::GetPositionInfo() const
{
   return m_impl->m_positionInfo;
}

const std::vector<GPS::SatelliteInfo>& Receiver::GetSatelliteInfos() const
{
   return m_impl->m_satelliteInfos;
}

void Receiver::RunWorkerThread()
{
   Thread::SetName(_T("GPS::Receiver I/O thread"));

   m_impl->m_ioService.run();
}

void Receiver::OnReceivedData(int errorCode, const std::vector<BYTE>& data, size_t bytesTransferred)
{
   if (errorCode != 0)
   {
      ATLTRACE(_T("OnReceivedData: error occured! error = %u\n"), errorCode);

      m_impl->m_evtHandlerCalled.Set();

      return;
   }

   CString textData(reinterpret_cast<LPCSTR>(&data[0]), static_cast<int>(bytesTransferred));

   ATLTRACE(_T("%s"), textData.GetString());

   m_impl->m_receiveBuffer += textData;

   AnalyzeBuffer();

   // continue receiving data
   m_impl->m_serialPort->AsyncReceive(
      std::bind(&Receiver::OnReceivedData, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void Receiver::AnalyzeBuffer()
{
   ATLASSERT(m_impl->m_parser != nullptr);

   for (;;)
   {
      // search for next newline
      int pos = m_impl->m_receiveBuffer.Find(_T('\n'));

      // no more newlines
      if (pos == -1)
      {
         // check if the buffer is longer than a sentence might be
         if (m_impl->m_receiveBuffer.GetLength() > 80)
            m_impl->m_receiveBuffer.Empty(); // throw buffer away

         return;
      }

      int length = pos;
      if (m_impl->m_receiveBuffer[pos - 1] == _T('\r'))
         --length;

      // new sentence
      CString sentence = m_impl->m_receiveBuffer.Left(length);
      m_impl->m_receiveBuffer.Delete(0, pos + 1);

      m_impl->m_parser->ParseLine(sentence);
      m_impl->m_positionInfo = m_impl->m_parser->GetPositionInfo();
      m_impl->m_satelliteInfos = m_impl->m_parser->GetSatelliteInfos();
   }
}