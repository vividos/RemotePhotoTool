//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file EdsdkPropertyAccess.hpp EDSDK - Property access
//
#pragma once

#include "EdsdkCommon.hpp"
#include "Variant.hpp"
#include "ImageProperty.hpp"

namespace EDSDK
{
/// Body ID; property was removed in EDSDK 2.12
#define kEdsPropID_BodyID 0x00000003
/// unknown property (available on EOS 40D)
#define kEdsPropID_Unknown1   0x00000065
/// shutter counter property; can't be read out with newest SDK anymore
#define kEdsPropID_ShutterCounter 0x00000022

// some more property IDs removed with EDSDK 3.x
#define kEdsPropID_MyMenu		          0x0000000e
#define kEdsPropID_JpegQuality            0x00000101
#define kEdsPropID_DigitalExposure        0x00000105
#define kEdsPropID_Contrast               0x00000109
#define kEdsPropID_ColorSaturation        0x0000010a
#define kEdsPropID_ColorTone              0x0000010b
#define kEdsPropID_Sharpness              0x0000010c
#define kEdsPropID_ToneCurve              0x0000010e
#define kEdsPropID_PhotoEffect            0x0000010f
#define kEdsPropID_FilterEffect           0x00000110
#define kEdsPropID_ToningEffect           0x00000111
#define kEdsPropID_ParameterSet           0x00000112
#define kEdsPropID_ColorMatrix            0x00000113
#define kEdsPropID_Linear                 0x00000300
#define kEdsPropID_ClickWBPoint           0x00000301
#define kEdsPropID_WBCoeffs               0x00000302
#define kEdsPropID_AtCapture_Flag         0x80000000
#define kEdsPropID_FlashCompensation      0x00000408
#define kEdsPropID_DepthOfField	         0x0000041b
#define kEdsPropID_EFCompensation         0x0000041e
#define kEdsPropID_Evf_FocusAid           0x00000509

// since there are no equivalent properties for retrieving some values, but MapToPropertyID()
// should return a property id, so that PropertyAccess::Get() can return a value, define "fake"
// property ids here.

/// base value for "meta" property ids
const EdsUInt32 kEdsPropID_Meta_Base = 0x10000000;

/// max. zoom pos
const EdsUInt32 kEdsPropID_Meta_MaxZoomPos = kEdsPropID_Meta_Base + 0;

/// combined image or device property access
class PropertyAccess
{
public:
   /// ctor
   PropertyAccess(const Handle& h)
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
   bool IsPropertyAvail(unsigned int uiPropId, int iParam) const;

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
   static EdsPropertyID MapToPropertyID(T_enImagePropertyType enProperty);

   /// returns name from property id
   static LPCTSTR NameFromId(EdsPropertyID propertyId);

   /// formats display text from id and value
   static CString DisplayTextFromIdAndValue(EdsPropertyID /*propertyId*/, Variant value);

private:
   /// deleted copy ctor
   PropertyAccess(const PropertyAccess&) = delete;

   /// deleted assignment operator
   PropertyAccess& operator=(const PropertyAccess&) = delete;

   /// handle to object
   const Handle& m_h;
};

} // namespace EDSDK
