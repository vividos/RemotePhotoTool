//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SerialPort.hpp serial port classes
//
#pragma once

#include <boost/noncopyable.hpp>
#include <memory>
#include <functional>
#include <ulib/config/BoostAsio.hpp>
#include "SerialPortInfo.hpp"
#include "SerialPortConfig.hpp"
#include <vector>

/// \brief Serial port classes
namespace Serial
{
   /// \brief Serial port class
   /// Implements access to serial port, including synchronous reading and writing, as well as
   /// asynchronous reading. Serial port enumeration is supported via EnumPorts().
   class SerialPort : public boost::noncopyable
   {
   public:
      /// handler function type for asynchronous receive operation
      typedef std::function<void(int errorCode, const std::vector<BYTE>& data, size_t bytesTransferred)> T_fnAsyncReceiveHandler;

      /// ctor opens serial port
      SerialPort(LPCTSTR deviceName, boost::asio::io_service& service);

      /// dtor; closes serial port
      ~SerialPort();

      /// enumerates all available serial ports
      static std::vector<SerialPortInfo> EnumPorts();

      /// returns config object for this port
      SerialPortConfig Config();

      /// returns if socket is open
      bool IsOpen() const;

      /// sends a break sequence
      void SendBreak();

      /// sends data and returns when bytes are written
      void Send(const std::vector<BYTE>& data);

      /// receives available data
      void Receive(std::vector<BYTE>& data, unsigned int numMaxSize = 512);

      /// starts asynchronous receive operation; handler is called when operation finishes
      void AsyncReceive(T_fnAsyncReceiveHandler asyncReceiveHandler, unsigned int numMaxSize = 512);

      /// closes the port
      void Close();

   private:
      friend class SerialPortConfig;

      class Impl;

      /// implementation
      std::shared_ptr<Impl> m_spImpl;
   };

} // namespace Serial
