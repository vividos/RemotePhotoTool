//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file EdsdkPropertyAccess.hpp EDSDK - Property access
//
#pragma once

// includes
#include "EdsdkCommon.hpp"
#include "Variant.hpp"
#include "ImageProperty.hpp"
#include <boost/noncopyable.hpp>

namespace EDSDK
{
/// Body ID; property was removed in EDSDK 2.12
#define kEdsPropID_BodyID 0x00000003
/// unknown property (available on EOS 40D)
#define kEdsPropID_Unknown1   0x00000065
/// shutter counter property; can't be read out with newest SDK anymore
#define kEdsPropID_ShutterCounter 0x00000022

/// combined image or device property access
class PropertyAccess : public boost::noncopyable
{
public:
   /// ctor
   PropertyAccess(const Handle& h) throw()
      :m_h(h)
   {
   }

   /// gets property value
   Variant Get(EdsPropertyID propId, int iParam = 0) const;

   /// sets new property value
   void Set(EdsPropertyID propId, Variant value, int iParam = 0);

   /// enumerates possible property values
   void Enum(EdsPropertyID propId, std::vector<Variant>& vecValues, bool& bReadOnly);

   /// returns if property currently is read only
   bool IsReadOnly(EdsPropertyID propId) const;

   /// returns type and size of given property
   void GetTypeAndSize(EdsPropertyID propId, int iParam, EdsDataType& dataType, EdsUInt32& size) const;

   /// returns if property is available
   bool IsPropertyAvail(unsigned int uiPropId) const throw();

   /// enumerates device ids
   void EnumDeviceIds(std::vector<unsigned int>& vecDeviceIds);

   /// enumerates image ids
   void EnumImageIds(std::vector<unsigned int>& vecImageIds);

   /// sets raw bytes to given variant
   static void SetRawEdsdk(Variant& v, unsigned int datatype, std::vector<unsigned char> vecData);

   /// gets raw bytes from variant
   static void GetRawEdsdk(const Variant& v, unsigned int datatype, std::vector<unsigned char>& vecData);

   /// formats image format value
   static CString FormatImageFormatValue(unsigned int uiValue);

   /// maps property type to property id
   static EdsPropertyID MapToPropertyID(T_enImagePropertyType enProperty) throw();

   /// returns name from property id
   static LPCTSTR NameFromId(EdsPropertyID propertyId) throw();

   /// formats display text from id and value
   static CString DisplayTextFromIdAndValue(EdsPropertyID /*propertyId*/, Variant value);

private:
   /// handle to object
   const Handle& m_h;
};

} // namespace EDSDK
