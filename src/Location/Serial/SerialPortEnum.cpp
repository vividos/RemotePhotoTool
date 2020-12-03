//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SerialPortEnum.cpp serial port enumeration
//
#include "stdafx.h"
#include "SerialPort.hpp"
#include "RegEnumKey.hpp"

using Serial::SerialPortInfo;
using Serial::SerialPort;

/// registry helper function
bool QueryRegKeyValue(CRegKey& key, LPCTSTR name, CString& value)
{
   // find out length
   DWORD bufferCountInCb = 0;
   LONG result = RegQueryValueEx(key, name, NULL, NULL, NULL, &bufferCountInCb);
   if (result != ERROR_SUCCESS)
      return false;

   DWORD bufferLength = (bufferCountInCb / sizeof(TCHAR)) + 1;
   result = key.QueryStringValue(name, value.GetBuffer(bufferLength), &bufferCountInCb);
   value.ReleaseBuffer();

   return result == ERROR_SUCCESS;
}

std::vector<SerialPortInfo> SerialPort::EnumSerialPorts()
{
   std::vector<SerialPortInfo> infoList;

#ifdef _WIN32_WCE
   // http://www.codeproject.com/KB/mobile/WMEnumComPorts.aspx
   // http://www.codeproject.com/KB/system/listports.aspx
   CString keyName;

   // go through all active drivers and figure out which ones are com ports
   for (unsigned int i = 0; i < 255; i++)
   {
      keyName.Format(_T("Drivers\\Active\\%02d"), i);

      CRegKey regKey;
      if (ERROR_SUCCESS != regKey.Open(HKEY_LOCAL_MACHINE, keyName, KEY_READ))
         continue;

      // get name
      CString name;
      if (!QueryRegKeyValue(regKey, _T("Name"), name))
         continue;

      CString nameTest(name);
      nameTest.MakeLower();

      if (nameTest.Find(_T("com")) == 0)
      {
         // get key
         CString key;
         if (!QueryRegKeyValue(regKey, _T("Key"), key))
            continue;

         // open registry key specified in "Key"
         CRegKey regKeyDriver;
         if (ERROR_SUCCESS != regKeyDriver.Open(HKEY_LOCAL_MACHINE, key, KEY_READ))
            continue;

         // get prefix
         CString prefix;
         if (!QueryRegKeyValue(regKeyDriver, _T("Prefix"), prefix))
            continue;

         // get index
         DWORD index = 0;
         if (ERROR_SUCCESS != regKeyDriver.QueryDWORDValue(_T("Index"), index))
            continue;

         // add info to vector
         CString displayName, deviceName;
         displayName.Format(_T("%s (%s)"), name.GetString(), key.GetString());
         deviceName.Format(_T("%s%u:"), prefix.GetString(), index);

         infoList.push_back(SerialPortInfo(displayName, deviceName));
      }

      regKey.Close();
   }

#else // WIN32

   static const TCHAR portsClass[] = _T("PORTS");
   static const TCHAR portsClassGUID[] = _T("{4D36E978-E325-11CE-BFC1-08002BE10318}");

   // search in reg key HKEY_LOCAL_MACHINE\System\CurrentControlSet\Enum
   CRegKey keyEnum;
   if (ERROR_SUCCESS == keyEnum.Open(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Enum"), KEY_ENUMERATE_SUB_KEYS))
   {
      CRegEnumKey enumDevices(keyEnum);
      CRegKey keyDevice;

      while (enumDevices.NextKey(keyDevice, KEY_ENUMERATE_SUB_KEYS))
      {
         CRegEnumKey enumSubDevices(keyDevice);

         CRegKey keySubDevice;
         while (enumSubDevices.NextKey(keySubDevice, KEY_ENUMERATE_SUB_KEYS))
         {
            CRegEnumKey enumSubSubDevices(keySubDevice);

            CRegKey keySubSubDevice;
            while (enumSubSubDevices.NextKey(keySubSubDevice, KEY_QUERY_VALUE))
            {
               CString className;
               if (QueryRegKeyValue(keySubSubDevice, _T("Class"), className))
               {
                  if (className.CompareNoCase(portsClass) != 0)
                     continue;
               }
               else
                  if (QueryRegKeyValue(keySubSubDevice, _T("Classguid"), className))
                  {
                     if (className.CompareNoCase(portsClassGUID) != 0)
                        continue;
                  }
                  else
                     continue;

               // at this point we either have a class or classguid key
               CString portname;
               if (!QueryRegKeyValue(keySubSubDevice, _T("Portname"), portname))
               {
                  CRegKey keyDeviceParams;
                  if (ERROR_SUCCESS != keyDeviceParams.Open(keySubSubDevice, _T("Device Parameters"), KEY_READ))
                     continue;

                  if (!QueryRegKeyValue(keyDeviceParams, _T("Portname"), portname))
                     continue;
               }

               // at this point we have retrieved the portname
               if (portname.Left(3) != _T("COM"))
                  continue; // skip non-serial ports

               CString friendlyName;
               if (!QueryRegKeyValue(keySubSubDevice, _T("Friendlyname"), friendlyName))
                  continue;

               // create entries
               CString deviceName;
               deviceName.Format(_T("\\\\.\\%s"), portname.GetString());
               infoList.push_back(SerialPortInfo(friendlyName, deviceName));
            }
         }
      }
   }

#endif // WIN32_WCE

   return infoList;
}
