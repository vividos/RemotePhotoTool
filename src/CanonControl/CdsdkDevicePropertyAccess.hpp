//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CdsdkDevicePropertyAccess.hpp CDSDK - Device property access
//
#pragma once

// includes
#include "CdsdkCommon.hpp"
#include "Variant.hpp"

/// the mysteriously unknown device property on my IXUS 430
#define cdDEVICE_PROP_UNKNOWN1   0x00000011

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
   Variant Get(cdDevicePropertyID propId) const;

   /// sets new device property value
   void Set(cdDevicePropertyID propId, Variant v);

   /// returns size of value of property
   cdUInt32 GetSize(cdDevicePropertyID propId) const;

   /// sets variant value from raw data
   static void SetRawCdsdk(Variant& v, unsigned int propId, std::vector<unsigned char> vecData);

   /// returns raw data from variant value
   void GetRawCdsdk(const Variant& v, unsigned int propId, std::vector<unsigned char>& vecData);

   /// returns name for property id
   static CString NameFromId(unsigned int propId) throw();

   /// returns display text from property id and value
   static CString DisplayTextFromIdAndValue(unsigned int propId, Variant value);

private:
   /// source device
   cdHSource m_hSource;
};

} // namespace CDSDK
