//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2026 Michael Fink
//
/// \file SerialPort.cpp serial port classes
//
#include "stdafx.h"
#include "SerialPort.hpp"
#include "SerialPortImpl.hpp"
#include "SerialPortConfig.hpp"
#include <atlstr.h>

using Serial::SerialPort;

SerialPort::SerialPort(LPCTSTR deviceName, asio::io_service& service)
{
   m_spImpl.reset(new SerialPort::Impl(CStringA(deviceName), service));
}

SerialPort::~SerialPort()
{
   try
   {
      m_spImpl->Close();
   }
   catch (...)
   {
   }
}

Serial::SerialPortConfig SerialPort::Config()
{
   return Serial::SerialPortConfig(*this);
}

bool SerialPort::IsOpen() const
try
{
   return m_spImpl->IsOpen();
}
catch (...)
{
   return false;
}

void SerialPort::SendBreak()
{
   m_spImpl->SendBreak();
}

void SerialPort::Send(const std::vector<BYTE>& data)
{
   m_spImpl->Send(data);
}

void SerialPort::Receive(std::vector<BYTE>& data, unsigned int numMaxSize)
{
   m_spImpl->Receive(data, numMaxSize);
}

void SerialPort::AsyncReceive(T_fnAsyncReceiveHandler asyncReceiveHandler, unsigned int numMaxSize)
{
   m_spImpl->AsyncReceive(asyncReceiveHandler, numMaxSize);
}

void SerialPort::Close()
{
   m_spImpl->Close();
}
