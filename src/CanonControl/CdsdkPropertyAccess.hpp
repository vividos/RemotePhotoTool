//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file CdsdkPropertyAccess.hpp CDSDK - Property access
//
#pragma once

// includes
#include "CdsdkCommon.hpp"
#include "Variant.hpp"
#include "ImageProperty.hpp"

namespace CDSDK
{

/// access to device properties
class DevicePropertyAccess
{
public:
   /// ctor
   DevicePropertyAccess(const cdHSource& hSource) throw()
      :m_hSource(hSource)
   {
   }

   /// returns device property
   Variant Get(cdDevicePropertyID propId) const
   {
      // first, get size of property
      cdUInt32 size = GetSize(propId);

      // now get the data bytes
      std::vector<BYTE> vecData(size);

      cdError err = CDGetDevicePropertyData(m_hSource, propId, &size, &vecData[0], 0);
      LOG_TRACE(_T("CDGetDevicePropertyData(%08x, propId = %08x, &size = %u, buffer, 0) returned %08x\n"),
         m_hSource, propId, size, err);
      CheckError(_T("CDGetDevicePropertyData"), err, __FILE__, __LINE__);

      // convert to variant
      Variant v;
      SetRawCdsdk(v, propId, vecData);
      return v;
   }

   /// sets new device property value
   void Set(cdDevicePropertyID propId, Variant v)
   {
      // generate data bytes from variant 
      std::vector<BYTE> vecData;
      GetRawCdsdk(v, propId, vecData);
      cdUInt32 size = static_cast<cdUInt32>(vecData.size());

      // now set the property
      cdError err = CDSetDevicePropertyData(m_hSource, propId, &size, &vecData[0]);
      LOG_TRACE(_T("CDSetDevicePropertyData(%08x, propId = %u, &size = %u, buffer) returned %08x\n"),
         m_hSource, propId, size, err);
      CheckError(_T("CDSetDevicePropertyData"), err, __FILE__, __LINE__);
   }

   /// returns size of value of property
   cdUInt32 GetSize(cdDevicePropertyID propId) const
   {
      cdUInt32 size = 0;
      cdError err = CDGetDevicePropertyData(m_hSource, propId, &size, nullptr, 0);
      LOG_TRACE(_T("CDGetDevicePropertyData(%08x, propId = %08x, &size = %u, nullptr, 0) returned %08x\n"),
         m_hSource, propId, size, err);
      CheckError(_T("CDGetDevicePropertyData"), err, __FILE__, __LINE__);

      return size;
   }

   /// sets variant value from raw data
   static void SetRawCdsdk(Variant& v, unsigned int propId, std::vector<unsigned char> vecData);

   /// returns raw data from variant value
   void GetRawCdsdk(const Variant& v, unsigned int propId, std::vector<unsigned char>& vecData);

   /// returns name for property id
   static CString NameFromId(unsigned int propId);

   /// returns display value from property id
   static CString ValueFromId(unsigned int /*propId*/, Variant value);

private:
   /// source device
   cdHSource m_hSource;
};

/// image property access
class ImagePropertyAccess
{
public:
   /// ctor
   ImagePropertyAccess(cdHSource& hSource) throw()
      :m_hSource(hSource)
   {
   }

   /// returns image property
   Variant Get(T_enImagePropertyType enImageProperty) const;

   /// sets image property
   void Set(T_enImagePropertyType enImageProperty, Variant val);

   /// enumerates image property
   void Enum(T_enImagePropertyType enImageProperty, std::vector<Variant>& vecValues);

   /// converts image property to name
   static CString NameFromId(T_enImagePropertyType /*enImageProperty*/);

   /// converts property value to string
   static CString ValueFromId(unsigned int /*enImageProperty*/, Variant value);

private:
   /// camera source
   cdHSource m_hSource;
};

} // namespace CDSDK
