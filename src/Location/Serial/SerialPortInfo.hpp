//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SerialPortInfo.hpp serial port infos
//
#pragma once

namespace Serial
{
   /// \brief serial port infos
   /// used in SerialPort::EnumPorts to return infos
   class SerialPortInfo
   {
   public:
      /// ctor
      SerialPortInfo(const CString& displayName,
         const CString& deviceName)
         :m_displayName(displayName),
         m_deviceName(deviceName)
      {
      }

      /// returns display name
      const CString& DisplayName() const { return m_displayName; }

      /// returns device name
      const CString& DeviceName() const { return m_deviceName; }

   private:
      CString m_displayName;  ///< display name
      CString m_deviceName;   ///< device name
   };

} // namespace Serial
