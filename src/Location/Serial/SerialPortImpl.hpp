//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SerialPortImpl.hpp serial port implementation
//
#pragma once

#include "stdafx.h"
#include "SerialPort.hpp"
#include <boost/asio/serial_port.hpp>
#include <ulib/SystemException.hpp>
#include "SharedBuffer.hpp"

using Serial::SerialPort;

/// serial port implementation class using boost::asio::serial_port
class SerialPort::Impl
{
public:
   /// ctor
   Impl(LPCSTR deviceName, boost::asio::io_service& service)
      :m_serialPort(service, deviceName)
   {
   }

   /// deleted copy ctor
   Impl(const Impl&) = delete;

   /// deleted copy assignment operator
   Impl& operator=(const Impl&) = delete;

   /// returns if serial port is open
   bool IsOpen() { return m_serialPort.is_open(); }

   /// sets serial port option
   template <typename Opt>
   void SetOption(Opt& opt)
   {
      boost::system::error_code ec;
      m_serialPort.set_option(opt, ec);
      if (ec)
         throw SystemException(_T("SetOption"), ec.value(), __FILE__, __LINE__);
   }

   /// returns serial port option value
   template <typename Opt>
   void GetOption(Opt& opt)
   {
      boost::system::error_code ec;
      m_serialPort.get_option(opt, ec);
      if (ec)
         throw SystemException(_T("GetOption"), ec.value(), __FILE__, __LINE__);
   }

   /// returns current baudrate
   unsigned int GetBaudrate()
   {
      boost::asio::serial_port_base::baud_rate opt;
      GetOption(opt);
      return opt.value();
   }

   /// returns current byte size in bits
   unsigned int GetByteSize()
   {
      boost::asio::serial_port_base::character_size opt;
      GetOption(opt);
      return opt.value();
   }

   /// returns current parity value
   SerialPortConfig::T_enParity GetParity()
   {
      boost::asio::serial_port_base::parity opt;
      GetOption(opt);
      return opt.value() == boost::asio::serial_port_base::parity::none ? SerialPortConfig::parityNone :
         opt.value() == boost::asio::serial_port_base::parity::even ? SerialPortConfig::parityEven :
         SerialPortConfig::parityOdd;
   }

   /// returns current stop bits value
   /// \retval 2 two stop bits
   /// \retval 1 one stop bit
   /// \retval 15 1.5 stop bits
   unsigned int GetStopBits()
   {
      boost::asio::serial_port_base::stop_bits opt;
      GetOption(opt);
      return opt.value() == boost::asio::serial_port_base::stop_bits::two ? 2 :
         opt.value() == boost::asio::serial_port_base::stop_bits::one ? 1 : 15;
   }

   /// returns current flow control value
   SerialPortConfig::T_enFlowControl GetFlowControl()
   {
      boost::asio::serial_port_base::flow_control opt;
      GetOption(opt);
      return opt.value() == boost::asio::serial_port_base::flow_control::none ? SerialPortConfig::flowNone :
         opt.value() == boost::asio::serial_port_base::flow_control::software ? SerialPortConfig::flowSoftware :
         SerialPortConfig::flowHardware;
   }

   /// sets new baudrate value
   void SetBaudrate(unsigned int baudrate)
   {
      boost::asio::serial_port_base::baud_rate opt(baudrate);
      SetOption(opt);
   }

   /// sets new byte size value, in bits
   void SetByteSize(unsigned int byteSize)
   {
      boost::asio::serial_port_base::character_size opt(byteSize);
      SetOption(opt);
   }

   /// sets new parity value
   void SetParity(SerialPortConfig::T_enParity parity)
   {
      boost::asio::serial_port_base::parity opt(
         parity == SerialPortConfig::parityNone ? boost::asio::serial_port_base::parity::none :
         parity == SerialPortConfig::parityEven ? boost::asio::serial_port_base::parity::even :
         boost::asio::serial_port_base::parity::odd);
      SetOption(opt);
   }

   /// sets new number of stop bits
   void SetStopBits(unsigned int stopBits)
   {
      boost::asio::serial_port_base::stop_bits opt(
         stopBits == 1 ? boost::asio::serial_port_base::stop_bits::one :
         stopBits == 2 ? boost::asio::serial_port_base::stop_bits::two :
         boost::asio::serial_port_base::stop_bits::onepointfive);
      SetOption(opt);
   }

   /// sets new flow control value
   void SetFlowControl(SerialPortConfig::T_enFlowControl flowControl)
   {
      boost::asio::serial_port_base::flow_control opt(
         flowControl == SerialPortConfig::flowNone ? boost::asio::serial_port_base::flow_control::none :
         flowControl == SerialPortConfig::flowSoftware ? boost::asio::serial_port_base::flow_control::software :
         boost::asio::serial_port_base::flow_control::hardware);
      SetOption(opt);
   }

   /// sends a break command
   void SendBreak()
   {
      boost::system::error_code ec;
      m_serialPort.send_break(ec);
      if (ec)
         throw SystemException(_T("SendBreak: ") + CString(ec.message().c_str()), ec.value(), __FILE__, __LINE__);
   }

   /// sends some data
   void Send(const std::vector<BYTE>& data)
   {
      boost::system::error_code ec;
      boost::asio::const_buffers_1 buffer(&data[0], data.size());
      boost::asio::write(m_serialPort, buffer);
      if (ec)
         throw SystemException(_T("Receive"), ec.value(), __FILE__, __LINE__);
   }

   /// reveices data
   void Receive(std::vector<BYTE>& data, unsigned int numMaxSize)
   {
      boost::system::error_code ec;

      data.resize(numMaxSize);
      boost::asio::mutable_buffers_1 buffer(&data[0], data.size());

      std::size_t uiRead = m_serialPort.read_some(buffer, ec);
      if (ec)
         throw SystemException(_T("Receive"), ec.value(), __FILE__, __LINE__);

      data.resize(uiRead);
   }

   /// async handler for receiving data
   static void AsyncReceiveHandler(
      const boost::system::error_code& error,
      std::size_t bytesTransferred,
      SharedMutableBuffer recvBuffer,
      T_fnAsyncReceiveHandler asyncReceiveHandler)
   {
      ATLASSERT(asyncReceiveHandler != nullptr);

      asyncReceiveHandler(error.value(), recvBuffer.Data(), bytesTransferred);
   }

   /// starts receiving data bytes; the async handler is called when bytes arrive
   void AsyncReceive(T_fnAsyncReceiveHandler asyncReceiveHandler, unsigned int numMaxSize)
   {
      SharedMutableBuffer buffer(numMaxSize);

      m_serialPort.async_read_some(buffer,
         std::bind(&Impl::AsyncReceiveHandler, std::placeholders::_1, std::placeholders::_2, buffer, asyncReceiveHandler));
   }

   /// closes the serial port
   void Close()
   {
      m_serialPort.close();
   }

private:
   /// serial port object
   boost::asio::serial_port m_serialPort;
};
