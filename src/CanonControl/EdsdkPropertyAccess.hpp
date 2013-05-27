//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file EdsdkPropertyAccess.hpp EDSDK - Property access
//
#pragma once

// includes
#include "EdsdkCommon.hpp"
#include "Variant.hpp"
#include "ImageProperty.hpp"

namespace EDSDK
{

/// unknown property (available on EOS 40D)
#define kEdsPropID_Unknown1   0x00000065
/// shutter counter property; can't be read out with newest SDK anymore
#define kEdsPropID_ShutterCounter 0x00000022

/// combined image or device property access
class PropertyAccess
{
public:
   /// ctor
   PropertyAccess(const Handle& h) throw()
      :m_h(h)
   {
   }

   /// gets property value
   Variant Get(EdsPropertyID propId, int iParam = 0) const
   {
      // first, get type and size of property
      EdsDataType dataType = kEdsDataType_Unknown;
      EdsUInt32 size = 0;
      GetTypeAndSize(propId, iParam, dataType, size);

      // now get the data bytes
      std::vector<BYTE> vecData(size);
      EdsError err = EdsGetPropertyData(m_h, propId, iParam, size, &vecData[0]);
      //LOG_TRACE(_T("EdsGetPropertyData(ref = %08x, id = %04x, param = %u, size = %u, &data) returned %08x\n"),
      //   m_h.Get(), propId, iParam, size, err);
      EDSDK::CheckError(_T("EdsGetPropertyData"), err, __FILE__, __LINE__);

      // convert to variant
      Variant v;
      SetRawEdsdk(v, dataType, vecData);
      return v;
   }

   /// sets new property value
   void Set(EdsPropertyID propId, Variant value, int iParam = 0)
   {
      // first, get type and size of property
      EdsDataType dataType = kEdsDataType_Unknown;
      EdsUInt32 size = 0;
      GetTypeAndSize(propId, iParam, dataType, size);

      // then generate data bytes from variant 
      std::vector<BYTE> vecData;
      GetRawEdsdk(value, dataType, vecData);

      LOG_TRACE(_T("Property [%s] set to value [%s]\n"),
         PropertyAccess::NameFromId(propId),
         PropertyAccess::DisplayTextFromIdAndValue(propId, value));

      // now set the property
      EdsError err = EdsSetPropertyData(m_h, propId, iParam, size, &vecData[0]);
      if (err != EDS_ERR_OK) // only log errors here
         LOG_TRACE(_T("EdsSetPropertyData(ref = %08x, id = %04x, param = %u, size = %u, &data) returned %08x\n"),
            m_h.Get(), propId, iParam, size, err);
      EDSDK::CheckError(_T("EdsSetPropertyData"), err, __FILE__, __LINE__);
   }

   /// enumerates possible property values
   void Enum(EdsPropertyID propId, std::vector<Variant>& vecValues, bool& bReadOnly)
   {
      //ATLASSERT(propId == kEdsPropID_AEMode ||
      //   propId == kEdsPropID_AEModeSelect ||
      //   propId == kEdsPropID_ISOSpeed ||
      //   propId == kEdsPropID_MeteringMode ||
      //   propId == kEdsPropID_Av ||
      //   propId == kEdsPropID_Tv ||
      //   propId == kEdsPropID_ExposureCompensation ||
      //   propId == kEdsPropID_WhiteBalance ||
      //   propId == kEdsPropID_DriveMode ||
      //   propId == kEdsPropID_ImageQuality ||
      //   propId == kEdsPropID_Evf_OutputDevice);

      // note: since all properties that can be retrieved are of type UInt8, assume that
      EdsPropertyDesc propDesc = {0};
      EdsError err = EdsGetPropertyDesc(m_h, propId, &propDesc);
      //LOG_TRACE(_T("EdsGetPropertyDesc(ref = %08x, id = %04x, &numProp = %u) returned %08x\n"),
      //   m_h.Get(), propId, propDesc.numElements, err);
      EDSDK::CheckError(_T("EdsGetPropertyDesc"), err, __FILE__, __LINE__);

      // note: this reinterpret cast assumes that EdsInt32 and EdsUInt32 have the same size
      static_assert(sizeof(propDesc.propDesc[0]) == sizeof(EdsUInt32), "propDesc must have type and size EdsUInt32");

      if (propDesc.access == 0)
         bReadOnly = true;

      if (propDesc.numElements > 0)
      {
         unsigned int* puiData = reinterpret_cast<unsigned int*>(propDesc.propDesc);
         for (EdsInt32 i=0; i<propDesc.numElements; i++)
         {
            Variant v;
            v.Set(puiData[i]);
            v.SetType(Variant::typeUInt32);
            vecValues.push_back(v);
         }
      }
   }

   /// returns if property currently is read only
   bool IsReadOnly(EdsPropertyID propId) const
   {
      EdsPropertyDesc propDesc = {0};
      EdsError err = EdsGetPropertyDesc(m_h, propId, &propDesc);
      //LOG_TRACE(_T("EdsGetPropertyDesc(ref = %08x, id = %04x, &numProp = %u) returned %08x\n"),
      //   m_h.Get(), propId, propDesc.numElements, err);
      EDSDK::CheckError(_T("EdsGetPropertyDesc"), err, __FILE__, __LINE__);

      // note: this reinterpret cast assumes that EdsInt32 and EdsUInt32 have the same size
      static_assert(sizeof(propDesc.propDesc[0]) == sizeof(EdsUInt32), "propDesc must have type and size EdsUInt32");

      return (propDesc.access == 0);
   }

   /// returns type and size of given property
   void GetTypeAndSize(EdsPropertyID propId, int iParam, EdsDataType& dataType, EdsUInt32& size) const
   {
      dataType = kEdsDataType_Unknown;
      size = 0;

      EdsError err = EdsGetPropertySize(m_h, propId, iParam, &dataType, &size);
      //LOG_TRACE(_T("EdsGetPropertySize(ref = %08x, id = %04x, param = %u, &type = %u, &size = %u) returned %08x\n"),
      //   m_h.Get(), propId, iParam, dataType, size, err);
      EDSDK::CheckError(_T("EdsGetPropertySize"), err, __FILE__, __LINE__);

      // it seems white balance returns the wrong data type (Int32) here; fix this
      if (propId == kEdsPropID_WhiteBalance)
         dataType = kEdsDataType_UInt32;
   }

   /// returns if property is available
   bool IsPropertyAvail(unsigned int uiPropId) const
   {
      // check if property exists by retrieving type and size
      EdsDataType dataType = kEdsDataType_Unknown;
      EdsUInt32 size = 0;
      EdsError err = EdsGetPropertySize(m_h.Get(), uiPropId, 0, &dataType, &size);
      return (err == EDS_ERR_OK && dataType != kEdsDataType_Unknown);
   }

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
   static EdsPropertyID MapToPropertyID(T_enImagePropertyType enProperty)
   {
      switch(enProperty)
      {
      case propShootingMode:        return kEdsPropID_AEMode;//kEdsPropID_AEModeSelect;
      case propDriveMode:     return kEdsPropID_DriveMode;
      case propISOSpeed:      return kEdsPropID_ISOSpeed;
      case propMeteringMode:  return kEdsPropID_MeteringMode;
      case propAFMode:        return kEdsPropID_AFMode;
      case propAv:            return kEdsPropID_Av;
      case propTv:            return kEdsPropID_Tv;
      case propExposureCompensation:   return kEdsPropID_ExposureCompensation;
      case propFlashExposureComp:      return kEdsPropID_FlashCompensation;
      case propFocalLength:            return kEdsPropID_FocalLength;
      case propFlashMode:              return kEdsPropID_FlashMode;
      case propWhiteBalance:           return kEdsPropID_WhiteBalance;
      case propAFDistance:             return kEdsPropID_Unknown;
      case propCurrentZoomPos:         return kEdsPropID_Unknown;
      case propMaxZoomPos:             return kEdsPropID_Unknown;
      case propDigitalZoomMagnification: return kEdsPropID_Unknown;
      case propAvailableShots:         return kEdsPropID_AvailableShots;
      case propSaveTo:                 return kEdsPropID_SaveTo;
      case propBatteryLevel:           return kEdsPropID_BatteryQuality;
      default:
         ATLASSERT(false);
         return kEdsPropID_Unknown;
      }
   }

   /// returns name from property id
   static CString NameFromId(EdsPropertyID propertyId);

   /// formats display text from id and value
   static CString DisplayTextFromIdAndValue(EdsPropertyID /*propertyId*/, Variant value);

private:
   /// handle to object
   const Handle& m_h;
};

} // namespace EDSDK
