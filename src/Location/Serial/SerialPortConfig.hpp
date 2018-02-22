//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SerialPortConfig.hpp serial port configuration
//
#pragma once

namespace Serial
{
   class SerialPort;

   /// \brief configuration for serial port
   /// \note all set functions directly set the property on the serial port
   class SerialPortConfig
   {
   public:
      /// parity settings
      enum T_enParity
      {
         parityNone, ///< no parity
         parityOdd,  ///< odd parity
         parityEven, ///< even parity
      };

      /// flow control settings
      enum T_enFlowControl
      {
         flowNone,      ///< no flow control
         flowSoftware,  ///< software flow control using XOn/XOff control characters
         flowHardware   ///< hardware flow control using serial port lines
      };

      /// predefined settings
      enum T_enPredefined
      {
         config4800_8N1_none, ///< 4800 baud, 8 bit, no parity, no stop bit, no hardware control
         config8N1, ///< 8 bit, no parity, no stop bit
         config7E1, ///< 7 bit, even parity, one stop bit
      };

      // get functions

      /// returns baudrate
      unsigned int Baudrate();

      /// returns size of byte (4..8 bit)
      unsigned int ByteSize();

      /// returns parity
      T_enParity Parity();

      /// returns number of stop bits (1 or 2, or 15 for 1.5 stop bits)
      unsigned int StopBits();

      /// returns current flow control type
      T_enFlowControl FlowControl();

      // set functions

      /// sets a predefined config set
      void SetPredefined(T_enPredefined predefined);

      /// sets baudrate
      void Baudrate(unsigned int baudrate);

      /// sets size of byte (4..8 bit)
      void ByteSize(unsigned int byteSize);

      /// sets parity
      void Parity(T_enParity parity);

      /// sets number of stop bits (1 or 2; use 15 to specify 1.5 stop bits)
      void StopBits(unsigned int stopBits);

      /// sets flow control type
      void FlowControl(T_enFlowControl flowControl);

   private:
      friend class SerialPort;

      /// ctor
      SerialPortConfig(SerialPort& port);

   private:
      SerialPort & m_port;  ///< serial port to get/set options from/to
   };

} // namespace Serial
