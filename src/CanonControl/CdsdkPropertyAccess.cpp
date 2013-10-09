//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file CdsdkPropertyAccess.cpp CDSDK - Property access
//

// includes
#include "stdafx.h"
#include "CdsdkPropertyAccess.hpp"

using namespace CDSDK;

void DevicePropertyAccess::SetRawCdsdk(Variant& v, unsigned int propId, std::vector<unsigned char> vecData)
{
   boost::any variant;
   Variant::VariantType enType = Variant::typeInvalid;
   bool bIsArray = false;

   switch (propId)
   {
      // uint32
   case cdDEVICE_PROP_MODEL_ID:
   case cdDEVICE_PROP_DPOF_CAP:
   case cdDEVICE_PROP_RELEASE_CONTROL_CAP:
   case cdDEVICE_PROP_BATTERY_STATUS:
      // assuming little endian here...
      ATLASSERT(vecData.size() >= 4);
      {
         unsigned int uiValue =
            static_cast<unsigned int>(vecData[0]) |
            (static_cast<unsigned int>(vecData[1]) << 8) |
            (static_cast<unsigned int>(vecData[2]) << 16) |
            (static_cast<unsigned int>(vecData[3]) << 24);
         variant = uiValue;
         enType = Variant::typeUInt32;
      }
      break;

      // bool
   case cdDEVICE_PROP_UPLOAD_FILE_CAP:
   case cdDEVICE_PROP_ROTATION_CAP:
      ATLASSERT(vecData.size() >= 1);
      variant = static_cast<bool>(vecData[0] != 0);
      break;

      // uint8
   case cdDEVICE_PROP_DIRECT_TRANSFER_STATUS:
      ATLASSERT(vecData.size() >= 1);
      variant = static_cast<unsigned char>(vecData[0]);
      break;

      // String
   case cdDEVICE_PROP_MODEL_NAME:
   case cdDEVICE_PROP_OWNER_NAME:
   case cdDEVICE_PROP_FIRMWARE_VERSION:
      vecData.push_back(0);
      variant = CString(reinterpret_cast<const char*>(&vecData[0]));
      enType = Variant::typeString;
      break;

      // cdTime
   case cdDEVICE_PROP_TIME:
      //break;

      // custom data, as array of uint8
   case cdDEVICE_PROP_SLIDE_SHOW_CAP:
   case cdDEVICE_PROP_RAW_DEVELOP_FACULTIES:
   case cdDEVICE_PROP_PARSE_FACULTIES:
   case cdDEVICE_PROP_THUMB_VALID_AREA: // this property isn't described in cdType.h, but assume it's an array
   case 0x00000011: // the mysteriously unknown device property on my IXUS 430 
      variant = std::vector<unsigned char>(vecData);
      enType = Variant::typeUInt8;
      bIsArray = true;
      break;

   default:
      ATLASSERT(false);
   }

   v.SetRaw(variant, enType, bIsArray);
}

void DevicePropertyAccess::GetRawCdsdk(const Variant& v, unsigned int propId, std::vector<unsigned char>& vecData)
{
   switch (propId)
   {
      // String
   case cdDEVICE_PROP_MODEL_NAME:
   case cdDEVICE_PROP_OWNER_NAME:
   case cdDEVICE_PROP_FIRMWARE_VERSION:
      ATLASSERT(v.Type() == Variant::typeString);
      {
         USES_CONVERSION;
         CString cszText = v.Get<CString>();
         CStringA cszaText = cszText;
         LPCSTR pszaText = cszaText.GetString();
         vecData.assign(pszaText, pszaText+strlen(pszaText)+1);
      }
      break;

   default:
      ATLASSERT(false);
      break;
   }
}

CString DevicePropertyAccess::NameFromId(unsigned int propId)
{
   cdDevicePropertyID devicePropId = static_cast<cdDevicePropertyID>(propId);

   LPCTSTR pszName = _T("???");
   switch (devicePropId)
   {
   case cdDEVICE_PROP_INVALID:                  pszName = _T("Invalid property ID"); break;
   case cdDEVICE_PROP_MODEL_ID:                 pszName = _T("Camera model ID"); break;
   case cdDEVICE_PROP_MODEL_NAME:               pszName = _T("Camera model name"); break;
   case cdDEVICE_PROP_SLIDE_SHOW_CAP:           pszName = _T("Slide show capacity"); break;
   case cdDEVICE_PROP_UPLOAD_FILE_CAP:          pszName = _T("CDAddPicture support"); break;
   case cdDEVICE_PROP_ROTATION_CAP:             pszName = _T("Image rotation support"); break;
   case cdDEVICE_PROP_DPOF_CAP:                 pszName = _T("Supported DPOF version number"); break;
   case cdDEVICE_PROP_THUMB_VALID_AREA:         pszName = _T("Valid area in the DCF thumbnail data"); break;
   case cdDEVICE_PROP_RELEASE_CONTROL_CAP:      pszName = _T("Remote release control capability"); break;
   case cdDEVICE_PROP_RAW_DEVELOP_FACULTIES:    pszName = _T("RAW file specification data"); break;
   case cdDEVICE_PROP_PARSE_FACULTIES:          pszName = _T("File parser data"); break;
   case cdDEVICE_PROP_OWNER_NAME:               pszName = _T("Owner name"); break;
   case cdDEVICE_PROP_TIME:                     pszName = _T("Device time"); break;
   case cdDEVICE_PROP_FIRMWARE_VERSION:         pszName = _T("Firmware version"); break;
   case cdDEVICE_PROP_BATTERY_STATUS:           pszName = _T("Battery status"); break;
   case cdDEVICE_PROP_DIRECT_TRANSFER_STATUS:   pszName = _T("Direct transfer status"); break;
   case 0x00000011:                             pszName = _T("?? unknown ??"); break;
   }

   return pszName;
}

CString DevicePropertyAccess::ValueFromId(unsigned int /*propId*/, Variant value)
{
//      cdDevicePropertyID devicePropId = static_cast<cdDevicePropertyID>(propId);
   // TODO
   return value.ToString();
}


/// read property access
template <T_enImagePropertyType propId, typename T, T defaultVal>
class PropertyAccessRead
{
public:
   /// get function
   typedef std::function<cdError(cdHSource, T*)> T_fnGet;

   /// ctor
   PropertyAccessRead(T_fnGet fnGet)
      :m_fnGet(fnGet)
   {
   }

   bool CanRead() const { return true; }
   bool CanWrite() const { return false; }
   bool CanEnum() const { return false; }

   Variant Get(cdHSource hSource)
   {
      T val = defaultVal;

      cdError err = m_fnGet(hSource, &val);
      LOG_TRACE(_T("PropertyAccess::Get(propId = %04x, source = %08x, &val = %04x) returned %08x\n"), propId, hSource, val, err);
      CheckError(_T("PropertyAccess::Get"), err, __FILE__, __LINE__);

      Variant value;
      value.Set<cdUInt16>(val);
      value.SetType(Variant::typeUInt16);

      return value;
   }

private:
   T_fnGet m_fnGet;
};

/// read-write property access
template <T_enImagePropertyType propId, typename T, T defaultVal>
class PropertyAccessReadWrite: public PropertyAccessRead<propId, T, defaultVal>
{
   typedef PropertyAccessRead<propId, T, defaultVal> BaseClass;

public:
   /// set function
   typedef std::function<cdError(cdHSource, T)> T_fnSet;

   bool CanWrite() const { return true; }

   /// ctor
   PropertyAccessReadWrite(T_fnGet fnGet, T_fnSet fnSet)
      :BaseClass(fnGet),
       m_fnSet(fnSet)
   {
   }

   /// sets new value
   void Set(cdHSource hSource, Variant value)
   {
      T val = value.Get<cdUInt16>();

      cdError err = m_fnSet(hSource, val);
      LOG_TRACE(_T("PropertyAccess::Set(source = %08x, propId = %04x, value = %04x) returned %08x\n"), hSource, propId, val, err);
      CheckError(_T("PropertyAccess::Set"), err, __FILE__, __LINE__);
   }

private:
   T_fnSet m_fnSet;
};

/// read-write and enum property access
template <T_enImagePropertyType propId, typename T, T defaultVal>
class PropertyAccessReadWriteEnum: public PropertyAccessReadWrite<propId, T, defaultVal>
{
   typedef PropertyAccessReadWrite<propId, T, defaultVal> BaseClass;

public:
   typedef std::function<cdError(cdHSource, cdHEnum*)> T_fnEnumReset;
   typedef std::function<cdError(cdHEnum, cdUInt32*)> T_fnEnumCount;
   typedef std::function<cdError(cdHEnum, T*)> T_fnEnumNext;
   typedef std::function<cdError(cdHEnum)> T_fnEnumRelease;

   /// ctor
   PropertyAccessReadWriteEnum(T_fnGet fnGet, T_fnSet fnSet,
      T_fnEnumReset fnEnumReset, T_fnEnumCount fnEnumCount, T_fnEnumNext fnEnumNext, T_fnEnumRelease fnEnumRelease)
      :BaseClass(fnGet, fnSet),
       m_fnEnumReset(fnEnumReset), m_fnEnumCount(fnEnumCount), m_fnEnumNext(fnEnumNext), m_fnEnumRelease(fnEnumRelease)
   {
   }

   bool CanEnum() const { return true; }

   void Enum(cdHSource hSource, std::vector<Variant>& vecValues)
   {
      // start enumerating
      cdHEnum hEnum = 0;
      cdError err = m_fnEnumReset(hSource, &hEnum);
      LOG_TRACE(_T("PropertyAccess::Enum::Reset(source = %08x, propId = %04x, &hEnum = %08x) returned %08x\n"), hSource, propId, hEnum, err);
      CheckError(_T("PropertyAccess::Enum::Reset"), err, __FILE__, __LINE__);

      // get count
      cdUInt32 uiCount = 0;
      err = m_fnEnumCount(hEnum, &uiCount);
      LOG_TRACE(_T("PropertyAccess::Enum::Count(hEnum = %08x, &count = %u) returned %08x\n"), hEnum, uiCount, err);
      CheckError(_T("PropertyAccess::Enum::Count"), err, __FILE__, __LINE__);

      // get all values
      for(cdUInt32 ui=0; ui<uiCount; ui++)
      {
         T val = defaultVal;
         err = m_fnEnumNext(hEnum, &val);
         LOG_TRACE(_T("PropertyAccess::Enum::Next(hEnum = %08x, &value = %04x) returned %08x\n"), hEnum, val, err);

         if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
            break; // end of list

         CheckError(_T("PropertyAccess::Enum::Next"), err, __FILE__, __LINE__);

         // add to list
         Variant value;
         value.Set<cdUInt16>(val);
         value.SetType(Variant::typeUInt16);

         vecValues.push_back(value);
      }

      err = m_fnEnumRelease(hEnum);
      LOG_TRACE(_T("PropertyAccess::Enum::Release(hEnum = %08x) returned %08x\n"), hEnum, err);
      CheckError(_T("PropertyAccess::Enum::Release"), err, __FILE__, __LINE__);
   }

private:
   T_fnEnumReset m_fnEnumReset;
   T_fnEnumCount m_fnEnumCount;
   T_fnEnumNext m_fnEnumNext;
   T_fnEnumRelease m_fnEnumRelease;
   
/*
   // image format

   virtual ImageFormat GetImageFormat() const
   {
      cdHSource hSource = GetSource();

      cdCompQuality compQuality = cdCOMP_QUALITY_UNKNOWN;
      cdImageSize imageSize = cdIMAGE_SIZE_UNKNOWN;

      cdError err = CDGetImageFormatAttribute(hSource, &compQuality, &imageSize);

      ImageFormat::T_enImageSize enImageSize =
         imageSize == cdIMAGE_SIZE_UNKNOWN ? ImageFormat::sizeUnknown :
            static_cast<ImageFormat::T_enImageSize>(imageSize);

      ImageFormat::T_enCompressQuality enCompressQuality =
         imageSize == cdCOMP_QUALITY_UNKNOWN ? ImageFormat::qualityUnknown :
            static_cast<ImageFormat::T_enCompressQuality>(imageSize);

      ATLTRACE(_T("CDGetImageFormatAttribute(%08x, &quality = %04x, &size = %04x \"%s\") returned %08x\n"),
         hSource, compQuality, imageSize, ImageFormat(enImageSize, enCompressQuality).ToString(), err);
      CheckError(_T("CDGetImageFormatAttribute"), err, __FILE__, __LINE__);

      return ImageFormat(enImageSize, enCompressQuality);
   }

   virtual void SetImageFormat(ImageFormat value)
   {
      ATLASSERT(value.ImageSize() != ImageFormat::sizeUnknown);
      ATLASSERT(value.CompressQuality() != ImageFormat::qualityUnknown);

      cdHSource hSource = GetSource();

      cdCompQuality compQuality = static_cast<cdCompQuality>(value.CompressQuality());
      cdImageSize imageSize = static_cast<cdImageSize>(value.ImageSize());

      cdError err = CDSetImageFormatAttribute(hSource, compQuality, imageSize);
      ATLTRACE(_T("CDSetImageFormatAttribute(%08x, quality = %04x, size = %04x \"%s\") returned %08x\n"),
         hSource, compQuality, imageSize, value.ToString(), err);
      CheckError(_T("CDSetImageFormatAttribute"), err, __FILE__, __LINE__);
   }

   virtual void EnumImageFormats(std::vector<ImageFormat>& vecImageFormatValues) const
   {
      cdHSource hSource = GetSource();

      cdHEnum hEnum = 0;
      cdError err = CDEnumImageFormatAttributeReset(hSource, &hEnum);
      ATLTRACE(_T("CDEnumImageFormatAttributeReset(%08x, &hEnum = %08x) returned %08x\n"), hSource, hEnum, err);
      CheckError(_T("CDEnumImageFormatAttributeReset"), err, __FILE__, __LINE__);

      for(;vecImageFormatValues.size() < 100;)
      {
         cdCompQuality compQuality = cdCOMP_QUALITY_UNKNOWN;
         cdImageSize imageSize = cdIMAGE_SIZE_UNKNOWN;

         err = CDEnumImageFormatAttributeNext(hEnum, &compQuality, &imageSize);

         ImageFormat::T_enImageSize enImageSize =
            imageSize == cdIMAGE_SIZE_UNKNOWN ? ImageFormat::sizeUnknown :
               static_cast<ImageFormat::T_enImageSize>(imageSize);

         ImageFormat::T_enCompressQuality enCompressQuality =
            imageSize == cdCOMP_QUALITY_UNKNOWN ? ImageFormat::qualityUnknown :
               static_cast<ImageFormat::T_enCompressQuality>(imageSize);

         ATLTRACE(_T("CDEnumImageFormatAttributeNext(%08x, &quality = %04x, &size = %04x \"%s\") returned %08x\n"),
            hEnum, compQuality, imageSize, ImageFormat(enImageSize, enCompressQuality).ToString(), err);

         if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
            break; // end of list

         CheckError(_T("CDEnumImageFormatAttributeNext"), err, __FILE__, __LINE__);

         vecImageFormatValues.push_back(ImageFormat(enImageSize, enCompressQuality));
      }

      err = CDEnumImageFormatAttributeRelease(hEnum);
      ATLTRACE(_T("CDEnumImageFormatAttributeRelease(%08x) returned %08x\n"), hEnum, err);
      CheckError(_T("CDEnumImageFormatAttributeRelease"), err, __FILE__, __LINE__);
   }
*/
};

#if 0
#define DEFINE_PROP2(PROPID, PROPTYPE, PROPDEFVAL, PROPFUNC) \
   static PropertyAccessReadWriteEnum<##PROPID, ##PROPTYPE, ##PROPDEFVAL> s_##PROPID( \
   ::CDGet##PROPFUNC, ::CDSet##PROPFUNC, ::CDEnum##PROPFUNC##Reset, ::CDGet##PROPFUNC##Count, ::CDEnum##PROPFUNC##Next, ::CDEnum##PROPFUNC##Release);

DEFINE_PROP2(propShootingMode, cdShootingMode, cdSHOOTING_MODE_INVALID, ShootingMode)
DEFINE_PROP2(propAv, cdRemoteSetAv, cdREMOTE_SET_AV_NA, AvValue);
DEFINE_PROP2(propTv, cdRemoteSetTv, cdREMOTE_SET_TV_NA, TvValue);
DEFINE_PROP2(propExposureCompensation, cdCompensation, cdCOMP_NA, ExposureComp);
DEFINE_PROP2(propWhiteBalance, cdWBLightSrc, cdWB_INVALID, WBSetting);
DEFINE_PROP2(propDriveMode, cdDriveMode, cdDRIVE_MODE_UNKNOWN, DriveMode);
//, , , ImageFormatAttribute
//, cdAFDistance, , AFDistanceSetting
//, , , FlashSetting             cdFlashMode, cdCompensation
//, cdRelCamSettingStruct, , RelCamSetting
#endif

Variant ImagePropertyAccess::Get(T_enImagePropertyType enImageProperty) const
{
   Variant v;

#define CASE_PROP_GET(PROPID) \
//case PROPID##: v = s_##PROPID.Get(m_hSource); break;

   switch (enImageProperty)
   {
   CASE_PROP_GET(propShootingMode)
   CASE_PROP_GET(propAv)
   CASE_PROP_GET(propTv)
   CASE_PROP_GET(propExposureCompensation)
   CASE_PROP_GET(propWhiteBalance)
   CASE_PROP_GET(propDriveMode)

/*
   case propISOSpeed: usValue = (); break;
   case propMeteringMode: usValue = (); break;
   case propAFMode: usValue = (); break;
   case propFlashExposureComp: usValue = (); break;
   case propFocalLength: usValue = (); break;
   case propFlashMode: usValue = (); break;
   case propAFDistance: usValue = (); break;
   case propCurrentZoomPos: usValue = (); break;
   case propMaxZoomPos: usValue = (); break;
   case propDigitalZoomMagnification: usValue = (); break;
   case propAvailableShots: usValue = (); break;
   case propSaveTo: usValue = (); break;
   case propBatteryLevel: usValue = (); break;
*/
   default:
      ATLASSERT(false);
      break;
   }

   return v;
}

void ImagePropertyAccess::Set(T_enImagePropertyType enImageProperty, Variant val)
{
#define CASE_PROP_SET(PROPID) \
//case PROPID##: s_##PROPID.Set(m_hSource, val); break;

   switch (enImageProperty)
   {
   CASE_PROP_SET(propShootingMode)
   CASE_PROP_SET(propAv)
   CASE_PROP_SET(propTv)
   CASE_PROP_SET(propExposureCompensation)
   CASE_PROP_SET(propWhiteBalance)
   CASE_PROP_SET(propDriveMode)

/*
   case propISOSpeed: (); break;
   case propMeteringMode: (); break;
   case propAFMode: (); break;
   case propFlashExposureComp: (); break;
   case propFocalLength: (); break;
   case propFlashMode: (); break;
   case propAFDistance: (); break;
   case propCurrentZoomPos: (); break;
   case propMaxZoomPos: (); break;
   case propDigitalZoomMagnification: (); break;
   case propAvailableShots: (); break;
   case propSaveTo: (); break;
   case propBatteryLevel: (); break;
*/
   }
}

void ImagePropertyAccess::Enum(T_enImagePropertyType enImageProperty, std::vector<Variant>& vecValues)
{
#define CASE_PROP_ENUM(PROPID) \
//case PROPID##: s_##PROPID.Enum(m_hSource, vecValues); break;

   switch (enImageProperty)
   {
   CASE_PROP_ENUM(propShootingMode)
   CASE_PROP_ENUM(propAv)
   CASE_PROP_ENUM(propTv)
   CASE_PROP_ENUM(propExposureCompensation)
   CASE_PROP_ENUM(propWhiteBalance)
   CASE_PROP_ENUM(propDriveMode)

/*
   case propISOSpeed: (); break;
   case propMeteringMode: (); break;
   case propAFMode: (); break;
   case propFlashExposureComp: (); break;
   case propFocalLength: (); break;
   case propFlashMode: (); break;
   case propAFDistance: (); break;
   case propCurrentZoomPos: (); break;
   case propMaxZoomPos: (); break;
   case propDigitalZoomMagnification: (); break;
   case propAvailableShots: (); break;
   case propSaveTo: (); break;
   case propBatteryLevel: (); break;
*/
   }
}

CString ImagePropertyAccess::NameFromId(T_enImagePropertyType /*enImageProperty*/)
{
   return _T(""); // TODO
}

CString ImagePropertyAccess::ValueFromId(unsigned int /*enImageProperty*/, Variant value)
{
   return value.ToString(); // TODO
}
