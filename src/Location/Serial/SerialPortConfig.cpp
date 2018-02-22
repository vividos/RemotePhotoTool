//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SerialPortConfig.cpp serial port configuration
//
#include "stdafx.h"
#include "SerialPortConfig.hpp"
#include "SerialPort.hpp"
#include "SerialPortImpl.hpp"

using Serial::SerialPortConfig;
using Serial::SerialPort;

SerialPortConfig::SerialPortConfig(SerialPort& port)
   :m_port(port)
{
}

unsigned int SerialPortConfig::Baudrate()
{
   return m_port.m_spImpl->GetBaudrate();
}

unsigned int SerialPortConfig::ByteSize()
{
   return m_port.m_spImpl->GetByteSize();
}

SerialPortConfig::T_enParity SerialPortConfig::Parity()
{
   return m_port.m_spImpl->GetParity();
}

unsigned int SerialPortConfig::StopBits()
{
   return m_port.m_spImpl->GetStopBits();
}

SerialPortConfig::T_enFlowControl SerialPortConfig::FlowControl()
{
   return m_port.m_spImpl->GetFlowControl();
}

void SerialPortConfig::SetPredefined(T_enPredefined predefined)
{
   switch (predefined)
   {
   case config4800_8N1_none:
      m_port.m_spImpl->SetBaudrate(4800);
      m_port.m_spImpl->SetByteSize(8);
      m_port.m_spImpl->SetParity(parityNone);
      m_port.m_spImpl->SetStopBits(1);
      m_port.m_spImpl->SetFlowControl(flowNone);
      break;

   case config8N1:
      m_port.m_spImpl->SetByteSize(8);
      m_port.m_spImpl->SetParity(parityNone);
      m_port.m_spImpl->SetStopBits(1);
      break;

   case config7E1:
      m_port.m_spImpl->SetByteSize(7);
      m_port.m_spImpl->SetParity(parityEven);
      m_port.m_spImpl->SetStopBits(1);
      break;

   default:
      ATLASSERT(false);
      break;
   }
}

void SerialPortConfig::Baudrate(unsigned int baudrate)
{
   m_port.m_spImpl->SetBaudrate(baudrate);
}

void SerialPortConfig::ByteSize(unsigned int byteSize)
{
   ATLASSERT(byteSize >= 4 && byteSize <= 8);

   m_port.m_spImpl->SetByteSize(byteSize);
}

void SerialPortConfig::Parity(T_enParity parity)
{
   m_port.m_spImpl->SetParity(parity);
}

void SerialPortConfig::StopBits(unsigned int stopBits)
{
   ATLASSERT(stopBits == 1 || stopBits == 2);

   m_port.m_spImpl->SetStopBits(stopBits);
}

void SerialPortConfig::FlowControl(T_enFlowControl flowControl)
{
   m_port.m_spImpl->SetFlowControl(flowControl);
}
