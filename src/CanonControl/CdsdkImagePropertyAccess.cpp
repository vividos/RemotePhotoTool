//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CdsdkImagePropertyAccess.cpp CDSDK - Image property access
//

// includes
#include "stdafx.h"
#include "CdsdkImagePropertyAccess.hpp"
#include "CdsdkDevicePropertyAccess.hpp"
#include "CommonPropertyAccess.hpp"

using namespace CDSDK;

/// all image property descriptions for CDSDK
static PropIdDisplayInfo g_aPropIdDisplayInfo[] =
{
   {
      cdREL_SET_SELF_TIMER,
      {
         { 0, _T("Off") },
         { 1, _T("On") },
         { 0xff, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_EZOOM,
      {
         { 0, _T("No teleconverter") },
         { 1, _T("Continuous (Smooth)") },
         { 0xffff, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_MLWEI_MODE,
      {
         { cdREL_VAL_MLWEI_MODE_CENTER_WEI_AVERAGE, _T("Center-weighted averaging") },
         { cdREL_VAL_MLWEI_MODE_SPOT, _T("Spot") },
         { cdREL_VAL_MLWEI_MODE_AVERAGE, _T("Averaging") },
         { cdREL_VAL_MLWEI_MODE_EVALUATIVE, _T("Evaluative") },
         { cdREL_VAL_MLWEI_MODE_PART, _T("Partial") },
         { cdREL_VAL_MLWEI_MODE_CENTER_WEI_AVERAGE_2, _T("Center-weighted averaging 2") },
         { cdREL_VAL_MLWEI_MODE_UNKNOWN, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_SLOW_SHUTTER,
      {
         { 0, _T("Off") },
         { 1, _T("Night scene") },
         { 2, _T("On") },
         { 3, _T("Not existing") },
         { 0xffff, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_AF_MODE,
      {
         { 0, _T("One-Shot AF / Single AF") },
         { 1, _T("AI Servo AF / Continuous AF") },
         { 2, _T("AI Focus AF") },
         { 3, _T("Manual Focus") },
         { 0xffff, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_CONTRAST,
      {
         { cdUInt8(cdREL_VAL_CONTRAST_LOW), _T("Low") },
         { cdREL_VAL_CONTRAST_NORMAL, _T("Normal") },
         { cdREL_VAL_CONTRAST_HIGH, _T("High") },
         { cdREL_VAL_CONTRAST_INVALID, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_COLOR_GAIN,
      {
         { cdUInt8(cdREL_VAL_COLOR_GAIN_LOW), _T("Low") },
         { cdREL_VAL_COLOR_GAIN_NORMAL, _T("Normal") },
         { cdREL_VAL_COLOR_GAIN_HIGH, _T("High") },
         { cdREL_VAL_COLOR_GAIN_INVALID, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_SHARPNESS,
      {
         { cdUInt8(cdREL_VAL_SHARPNESS_LOW), _T("Low") },
         { cdREL_VAL_SHARPNESS_NORMAL, _T("Normal") },
         { cdREL_VAL_SHARPNESS_HIGH, _T("High") },
         { cdREL_VAL_SHARPNESS_INVALID, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_DISP_AV,
      {
         { 0, _T("APEX value") },
         { 1, _T("1/10 value") },
         { 0xff, _T("Invalid") },
         { 0xffff, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_FOCAL_ML_SPOT_POS,
      {
         { 0, _T("Center") },
         { 1, _T("AF point") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_DISP_AV_MAX,
      {
         { 0, _T("APEX value") },
         { 1, _T("1/10 value") },
         { 0xff, _T("Invalid") },
         { 0xffff, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_PHOTO_EFFECT,
      {
         { cdPHOTO_EFFECT_OFF, _T("Off") },
         { cdPHOTO_EFFECT_VIVID, _T("Vivid") },
         { cdPHOTO_EFFECT_NEUTRAL, _T("Neutral") },
         { cdPHOTO_EFFECT_LOW_SHARPENING, _T("Low sharpening") },
         { cdPHOTO_EFFECT_SEPIA, _T("Sepia") },
         { cdPHOTO_EFFECT_BW, _T("Black & white") },
         { cdPHOTO_EFFECT_CUSTOM, _T("Custom") },
         { cdPHOTO_EFFECT_MY_COLOR, _T("My color data") },
         { cdPHOTO_EFFECT_UNKNOWN, _T("Unknown") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_AF_LIGHT,
      {
         { 0, _T("Off") },
         { 1, _T("On") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_ROTATION_ANGLE,
      {
         { 0, _T("0 degree") },
         { 1, _T("90 degrees") },
         { 2, _T("180 degrees") },
         { 3, _T("270 degrees") },
         { 0xffff, _T("Unknown") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_ROTATION_SENSE,
      {
         { 0, _T("Enable") },
         { 1, _T("Disable") },
         { 2, _T("") },
         { 3, _T("") },
         { 0xffff, _T("Not available") },
         { 0, nullptr }
      }
   },

   {
      cdREL_SET_COLOR_MATRIX,
      {
         { cdCOLOR_MATRIX_ATCAPTURE, _T("As shot") },
         { cdCOLOR_MATRIX_1, _T("Color matrix 1") },
         { cdCOLOR_MATRIX_2, _T("Color matrix 2") },
         { cdCOLOR_MATRIX_3, _T("Color matrix 3") },
         { cdCOLOR_MATRIX_4, _T("Color matrix 4") },
         { cdCOLOR_MATRIX_5, _T("Color matrix 5") },
         { cdCOLOR_MATRIX_UNKNOWN, _T("Unknown") },
         { 0, nullptr }
      }
   },
};


/// converts property type enum (T_enImagePropertyType) to property id.
/// since there are some release settings in the lower area, add a large enough constant
#define TYPE_TO_PROP_ID(x) (x+0x00010000)

/// template to determine variant type from template type
template <typename T>
Variant::VariantType VariantTypeFromType(T = 0);

/// template specialisation for unsigned char
template <>
Variant::VariantType VariantTypeFromType(unsigned char)
{
   return Variant::typeUInt8;
}

/// template specialisation for unsigned short
template <>
Variant::VariantType VariantTypeFromType(unsigned short)
{
   return Variant::typeUInt16;
}

/// template specialisation for unsigned int
template <>
Variant::VariantType VariantTypeFromType(unsigned int)
{
   return Variant::typeUInt32;
}

/// template specialisation for unsigned long
template <>
Variant::VariantType VariantTypeFromType(unsigned long)
{
   static_assert(sizeof(unsigned long) == 4, "unsigned long must be 4 bytes large to be correct here");
   return Variant::typeUInt32;
}


/// \brief read property access
/// \tparam propId image property id
/// \tparam T type for property value
/// \tparam defaultVal default value for property
template <T_enImagePropertyType propId, typename T, T defaultVal>
class PropertyAccessRead
{
public:
   /// get function type
   typedef std::function<cdError(cdHSource, T*)> T_fnGet;

   /// ctor
   PropertyAccessRead(T_fnGet fnGet)
      :m_fnGet(fnGet)
   {
   }

   bool CanRead() const { return true; }     ///< returns if value read is possible
   bool CanWrite() const { return false; }   ///< returns if value write is possible
   bool CanEnum() const { return false; }    ///< returns if value enumeration is possible

   /// returns property value
   Variant Get(cdHSource hSource)
   {
      T val = defaultVal;

      cdError err = m_fnGet(hSource, &val);
      LOG_TRACE(_T("PropertyAccess::Get(propId = %04x, source = %08x, &val = %04x) returned %08x\n"), propId, hSource, val, err);
      CheckError(_T("PropertyAccess::Get"), err, __FILE__, __LINE__);

      Variant value;
      value.Set<T>(val);
      value.SetType(VariantTypeFromType<T>());

      return value;
   }

private:
   /// get function
   T_fnGet m_fnGet;
};

/// \brief read-write property access
/// \tparam propId image property id
/// \tparam T type for property value
/// \tparam defaultVal default value for property
template <T_enImagePropertyType propId, typename T, T defaultVal>
class PropertyAccessReadWrite: public PropertyAccessRead<propId, T, defaultVal>
{
   /// base class typedef
   typedef PropertyAccessRead<propId, T, defaultVal> BaseClass;

public:
   /// set function
   typedef std::function<cdError(cdHSource, T)> T_fnSet;

   bool CanWrite() const { return true; } ///< returns if value write is possible

   /// ctor
   PropertyAccessReadWrite(T_fnGet fnGet, T_fnSet fnSet)
      :BaseClass(fnGet),
       m_fnSet(fnSet)
   {
   }

   /// sets new value
   void Set(cdHSource hSource, Variant value)
   {
      T val = value.Get<T>();

      cdError err = m_fnSet(hSource, val);
      LOG_TRACE(_T("PropertyAccess::Set(source = %08x, propId = %04x, value = %04x) returned %08x\n"), hSource, propId, val, err);
      CheckError(_T("PropertyAccess::Set"), err, __FILE__, __LINE__);
   }

private:
   /// set function
   T_fnSet m_fnSet;
};

/// \brief read-write and enum property access
/// \tparam propId image property id
/// \tparam T type for property value
/// \tparam defaultVal default value for property
template <T_enImagePropertyType propId, typename T, T defaultVal>
class PropertyAccessReadWriteEnum: public PropertyAccessReadWrite<propId, T, defaultVal>
{
   /// base class typedef
   typedef PropertyAccessReadWrite<propId, T, defaultVal> BaseClass;

public:
   typedef std::function<cdError(cdHSource, cdHEnum*)> T_fnEnumReset;   ///< function to reset enumeration
   typedef std::function<cdError(cdHEnum, cdUInt32*)> T_fnEnumCount;    ///< function to return count of values
   typedef std::function<cdError(cdHEnum, T*)> T_fnEnumNext;   ///< function to get next value
   typedef std::function<cdError(cdHEnum)> T_fnEnumRelease;    ///< function to release enumeration handle

   /// ctor
   PropertyAccessReadWriteEnum(T_fnGet fnGet, T_fnSet fnSet,
      T_fnEnumReset fnEnumReset, T_fnEnumCount fnEnumCount, T_fnEnumNext fnEnumNext, T_fnEnumRelease fnEnumRelease)
      :BaseClass(fnGet, fnSet),
       m_fnEnumReset(fnEnumReset), m_fnEnumCount(fnEnumCount), m_fnEnumNext(fnEnumNext), m_fnEnumRelease(fnEnumRelease)
   {
   }

   bool CanEnum() const { return true; }  ///< returns if value enumeration is possible

   /// enumerates property values
   void Enum(cdHSource hSource, std::vector<Variant>& vecValues)
   {
      // start enumerating
      cdHEnum hEnum = 0;
      cdError err = m_fnEnumReset(hSource, &hEnum);
      if (err != cdOK) LOG_TRACE(_T("PropertyAccess::Enum::Reset(source = %08x, propId = %04x, &hEnum = %08x) returned %08x\n"), hSource, propId, hEnum, err);
      CheckError(_T("PropertyAccess::Enum::Reset"), err, __FILE__, __LINE__);

      // get count
      cdUInt32 uiCount = 0;
      err = m_fnEnumCount(hEnum, &uiCount);
      if (err != cdOK) LOG_TRACE(_T("PropertyAccess::Enum::Count(hEnum = %08x, &count = %u) returned %08x\n"), hEnum, uiCount, err);
      CheckError(_T("PropertyAccess::Enum::Count"), err, __FILE__, __LINE__);

      // get all values
      for(cdUInt32 ui=0; ui<uiCount; ui++)
      {
         T val = defaultVal;
         err = m_fnEnumNext(hEnum, &val);

         if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
            break; // end of list

         if (err != cdOK) LOG_TRACE(_T("PropertyAccess::Enum::Next(hEnum = %08x, &value = %04x) returned %08x\n"), hEnum, val, err);

         CheckError(_T("PropertyAccess::Enum::Next"), err, __FILE__, __LINE__);

         // add to list
         Variant value;
         value.Set<T>(val);
         value.SetType(VariantTypeFromType<T>());

         vecValues.push_back(value);
      }

      err = m_fnEnumRelease(hEnum);
      if (err != cdOK) LOG_TRACE(_T("PropertyAccess::Enum::Release(hEnum = %08x) returned %08x\n"), hEnum, err);
      CheckError(_T("PropertyAccess::Enum::Release"), err, __FILE__, __LINE__);
   }

private:
   T_fnEnumReset m_fnEnumReset;     ///< resets enumeration
   T_fnEnumCount m_fnEnumCount;     ///< returns count of values
   T_fnEnumNext m_fnEnumNext;       ///< returns next value
   T_fnEnumRelease m_fnEnumRelease; ///< releases enumeration handle
};

/// flash settings property access
class FlashSettingsPropertyAccess
{
public:
   /// ctor
   /// \param bAccessFlashMode indicates if flash mode (true) or compensation (false) should be get/set/enumerated
   FlashSettingsPropertyAccess(bool bAccessFlashMode)
      :m_bAccessFlashMode(bAccessFlashMode)
   {
   }

   bool CanRead() const { return true; }     ///< returns if value read is possible
   bool CanWrite() const { return true; }    ///< returns if value write is possible
   /// returns if value enumeration is possible; can only enumerate flash modes
   bool CanEnum() const { return m_bAccessFlashMode; }

   /// returns property value
   Variant Get(cdHSource hSource)
   {
      cdFlashMode flashMode = 0;
      cdCompensation compensation = 0;

      cdError err = CDGetFlashSetting(hSource, &flashMode, &compensation);
      LOG_TRACE(_T("CDGetFlashSetting(source = %08x, &flashMode = %04x, &compensation = %04x) returned %08x\n"),
         hSource, flashMode, compensation, err);
      CheckError(_T("CDGetFlashSetting"), err, __FILE__, __LINE__);

      Variant value;
      value.Set<cdUInt16>(m_bAccessFlashMode ? flashMode : compensation);
      value.SetType(Variant::typeUInt16);

      return value;
   }

   /// sets new value
   void Set(cdHSource hSource, Variant value)
   {
      cdFlashMode flashMode = value.Get<cdUInt16>();
      cdCompensation compensation = value.Get<cdUInt16>();

      cdError err = CDSetFlashSetting(hSource,
         m_bAccessFlashMode ? flashMode : cdFLASH_MODE_NA,
         m_bAccessFlashMode ? cdCOMP_NA : compensation);
      LOG_TRACE(_T("CDSetFlashSetting(source = %08x, flashMode = %04x, compensation = %04x) returned %08x\n"),
         hSource, flashMode, compensation, err);
      CheckError(_T("CDSetFlashSetting"), err, __FILE__, __LINE__);
   }

   /// enumerates property values
   void Enum(cdHSource hSource, std::vector<Variant>& vecValues)
   {
      if (!m_bAccessFlashMode)
         return;

      // start enumerating
      cdHEnum hEnum = 0;
      cdError err = CDEnumFlashSettingReset(hSource, &hEnum);
      if (err != cdOK) LOG_TRACE(_T("CDEnumFlashSettingReset(source = %08x, &hEnum = %08x) returned %08x\n"), hSource, hEnum, err);
      CheckError(_T("CDEnumFlashSettingReset"), err, __FILE__, __LINE__);

      // get count
      cdUInt32 uiCount = 0;
      err = CDGetFlashSettingCount(hEnum, &uiCount);
      if (err != cdOK) LOG_TRACE(_T("CDGetFlashSettingCount(hEnum = %08x, &count = %u) returned %08x\n"), hEnum, uiCount, err);
      CheckError(_T("CDGetFlashSettingCount"), err, __FILE__, __LINE__);

      // get all values
      for(cdUInt32 ui=0; ui<uiCount; ui++)
      {
         cdFlashMode flashMode = 0;

         err = CDEnumFlashSettingNext(hEnum, &flashMode);
         if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
            break; // end of list

         if (err != cdOK) LOG_TRACE(_T("CDEnumFlashSettingNext(hEnum = %08x, &flashMode = %04x) returned %08x\n"),
            hEnum, flashMode, err);

         CheckError(_T("CDEnumFlashSettingNext"), err, __FILE__, __LINE__);

         // add to list
         Variant value;
         value.Set<cdUInt16>(flashMode);
         value.SetType(Variant::typeUInt16);

         vecValues.push_back(value);
      }

      err = CDEnumFlashSettingRelease(hEnum);
      if (err != cdOK) LOG_TRACE(_T("CDEnumFlashSettingRelease(hEnum = %08x) returned %08x\n"), hEnum, err);
      CheckError(_T("CDEnumFlashSettingRelease"), err, __FILE__, __LINE__);
   }

private:
   /// indicates if flash mode should be accessed
   bool m_bAccessFlashMode;
};

/// image format property access
class ImageFormatPropertyAccess
{
public:
   bool CanRead() const { return true; }     ///< returns if value read is possible
   bool CanWrite() const { return true; }    ///< returns if value write is possible
   bool CanEnum() const { return true; }     ///< returns if value enumeration is possible

   /// returns property value
   Variant Get(cdHSource hSource)
   {
      cdCompQuality compQuality = cdCOMP_QUALITY_UNKNOWN;
      cdImageSize imageSize = cdIMAGE_SIZE_UNKNOWN;

      cdError err = CDGetImageFormatAttribute(hSource, &compQuality, &imageSize);

      std::vector<cdUInt16> vecRawValues;
      vecRawValues.push_back(compQuality);
      vecRawValues.push_back(imageSize);

      Variant value;
      value.SetArray(vecRawValues);
      value.SetType(Variant::typeUInt16);

      LOG_TRACE(_T("CDGetImageFormatAttribute(source = %08x, &compQuality = %04x, &imageSize = %04x \"%s\") returned %08x\n"),
         hSource, compQuality, imageSize,
         ImagePropertyAccess::FormatImageFormatValue(value).GetString(),
         err);
      CheckError(_T("CDGetImageFormatAttribute"), err, __FILE__, __LINE__);

      return value;
   }

   /// sets new value
   void Set(cdHSource hSource, Variant value)
   {
      ATLASSERT(value.IsArray() == true);
      ATLASSERT(value.Type() == Variant::typeUInt16);

      std::vector<cdUInt16> vecValues = value.GetArray<cdUInt16>();
      ATLASSERT(vecValues.size() == 2);

      cdCompQuality compQuality = static_cast<cdCompQuality>(vecValues[0]);
      cdImageSize imageSize = static_cast<cdImageSize>(vecValues[1]);

      cdError err = CDSetImageFormatAttribute(hSource, compQuality, imageSize);
      LOG_TRACE(_T("CDSetImageFormatAttribute(%08x, quality = %04x, size = %04x \"%s\") returned %08x\n"),
         hSource, compQuality, imageSize,
         ImagePropertyAccess::FormatImageFormatValue(value).GetString(),
         err);
      CheckError(_T("CDSetImageFormatAttribute"), err, __FILE__, __LINE__);
   }

   /// enumerates property values
   void Enum(cdHSource hSource, std::vector<Variant>& vecValues)
   {
      cdHEnum hEnum = 0;
      cdError err = CDEnumImageFormatAttributeReset(hSource, &hEnum);
      if (err != cdOK) LOG_TRACE(_T("CDEnumImageFormatAttributeReset(%08x, &hEnum = %08x) returned %08x\n"), hSource, hEnum, err);
      CheckError(_T("CDEnumImageFormatAttributeReset"), err, __FILE__, __LINE__);

      for (; vecValues.size() < 100;)
      {
         cdCompQuality compQuality = cdCOMP_QUALITY_UNKNOWN;
         cdImageSize imageSize = cdIMAGE_SIZE_UNKNOWN;

         err = CDEnumImageFormatAttributeNext(hEnum, &compQuality, &imageSize);

         std::vector<cdUInt16> vecRawValues;
         vecRawValues.push_back(compQuality);
         vecRawValues.push_back(imageSize);

         Variant value;
         value.SetArray(vecRawValues);
         value.SetType(Variant::typeUInt16);

         if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
            break; // end of list

         if (err != cdOK) LOG_TRACE(_T("CDEnumImageFormatAttributeNext(%08x, &quality = %04x, &size = %04x \"%s\") returned %08x\n"),
            hEnum, compQuality, imageSize,
            ImagePropertyAccess::FormatImageFormatValue(value).GetString(),
            err);

         CheckError(_T("CDEnumImageFormatAttributeNext"), err, __FILE__, __LINE__);

         vecValues.push_back(value);
      }

      err = CDEnumImageFormatAttributeRelease(hEnum);
      if (err != cdOK) LOG_TRACE(_T("CDEnumImageFormatAttributeRelease(%08x) returned %08x\n"), hEnum, err);
      CheckError(_T("CDEnumImageFormatAttributeRelease"), err, __FILE__, __LINE__);
   }
};

/// implements read-only property using PropertyAccessRead class
#define IMPL_PROP_R(PROPID, PROPTYPE, PROPDEFVAL, PROPFUNC) \
   static PropertyAccessRead<##PROPID, ##PROPTYPE, ##PROPDEFVAL> s_##PROPID( \
   ::CDGet##PROPFUNC);

/// implements read-write property using PropertyAccessReadWrite class
#define IMPL_PROP_RW(PROPID, PROPTYPE, PROPDEFVAL, PROPFUNC) \
   static PropertyAccessReadWrite<##PROPID, ##PROPTYPE, ##PROPDEFVAL> s_##PROPID( \
   ::CDGet##PROPFUNC, ::CDSet##PROPFUNC);

/// implements read-write-enum using PropertyAccessReadWriteEnum property
#define IMPL_PROP_RWE(PROPID, PROPTYPE, PROPDEFVAL, PROPFUNC) \
   static PropertyAccessReadWriteEnum<##PROPID, ##PROPTYPE, ##PROPDEFVAL> s_##PROPID( \
   ::CDGet##PROPFUNC, ::CDSet##PROPFUNC, ::CDEnum##PROPFUNC##Reset, ::CDGet##PROPFUNC##Count, ::CDEnum##PROPFUNC##Next, ::CDEnum##PROPFUNC##Release);

IMPL_PROP_RWE(propShootingMode, cdShootingMode, cdSHOOTING_MODE_INVALID, ShootingMode)
IMPL_PROP_RWE(propAv, cdRemoteSetAv, cdREMOTE_SET_AV_NA, AvValue)
IMPL_PROP_RWE(propTv, cdRemoteSetTv, cdREMOTE_SET_TV_NA, TvValue)
IMPL_PROP_RWE(propExposureCompensation, cdCompensation, cdCOMP_NA, ExposureComp)
IMPL_PROP_RWE(propWhiteBalance, cdWBLightSrc, cdWB_INVALID, WBSetting)
IMPL_PROP_RWE(propDriveMode, cdDriveMode, cdDRIVE_MODE_UNKNOWN, DriveMode)
FlashSettingsPropertyAccess s_propFlashExposureComp(false); ///< property access for flash exposure compensation
FlashSettingsPropertyAccess s_propFlashMode(true); ///< property access for flash mode
IMPL_PROP_RWE(propAFDistance, cdAFDistance, cdAF_DISTANCE_AUTO, AFDistanceSetting)
IMPL_PROP_RW(propCurrentZoomPos, cdUInt32, 0, ZoomPos)
ImageFormatPropertyAccess s_propImageFormat; ///< property access for image format


std::vector<unsigned int> ImagePropertyAccess::EnumImageProperties() const
{
   std::vector<unsigned int> vecPropIds;

   // add stock property ids
   vecPropIds.push_back(TYPE_TO_PROP_ID(propShootingMode));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propAv));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propTv));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propExposureCompensation));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propWhiteBalance));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propDriveMode));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propFlashExposureComp));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propFocalLength));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propFlashMode));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propAFDistance));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propCurrentZoomPos));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propMaxZoomPos));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propSaveTo));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propAvailableShots));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propBatteryLevel));
   vecPropIds.push_back(TYPE_TO_PROP_ID(propImageFormat));

   EnumAvailReleaseSettings(vecPropIds);

   return vecPropIds;
}

Variant ImagePropertyAccess::Get(unsigned int uiPropId) const
{
   Variant v;

/// switch-case for getting a property
#define CASE_PROP_GET(PROPID) \
   case TYPE_TO_PROP_ID(PROPID##): ATLASSERT(true == s_##PROPID.CanRead()); v = s_##PROPID.Get(m_hSource); break;

   switch (uiPropId)
   {
   CASE_PROP_GET(propShootingMode)
   CASE_PROP_GET(propAv)
   CASE_PROP_GET(propTv)
   CASE_PROP_GET(propExposureCompensation)
   CASE_PROP_GET(propWhiteBalance)
   CASE_PROP_GET(propDriveMode)

   case TYPE_TO_PROP_ID(propISOSpeed): v = GetReleaseSetting(cdREL_SET_ISO_SPEED_RATINGS); break;
   case TYPE_TO_PROP_ID(propMeteringMode): v = GetReleaseSetting(cdREL_SET_MLWEI_MODE); break;
   case TYPE_TO_PROP_ID(propAFMode): v = GetReleaseSetting(cdREL_SET_AF_MODE); break;

   CASE_PROP_GET(propFlashExposureComp)

   case TYPE_TO_PROP_ID(propFocalLength):
      {
         Variant focalLengthNum = GetReleaseSetting(cdREL_SET_FOCAL_LENGTH);
         Variant focalLengthDenom = GetReleaseSetting(cdREL_SET_FOCAL_LENGTH_DENOMINATOR);
         double dVal = double(focalLengthNum.Get<cdUInt16>()) / focalLengthDenom.Get<cdUInt16>();

         v.Set(dVal);
         v.SetType(Variant::typeDouble);
      }
      break;

   CASE_PROP_GET(propFlashMode)
   CASE_PROP_GET(propAFDistance)
   CASE_PROP_GET(propCurrentZoomPos)

   case TYPE_TO_PROP_ID(propMaxZoomPos):
      {
         cdUInt32 maxZoomPos = 0;
         cdUInt32 maxOpticalZoomPos = 0;

         cdError err = CDGetMaximumZoomPos(m_hSource, &maxZoomPos, &maxOpticalZoomPos);
         LOG_TRACE(_T("CDGetMaximumZoomPos(source = %08x, &maxZoomPos = %04x, &maxOpticalZoomPos = %04x) returned %08x\n"),
            m_hSource, maxZoomPos, maxOpticalZoomPos, err);
         CheckError(_T("CDGetMaximumZoomPos"), err, __FILE__, __LINE__);

         v.Set(maxOpticalZoomPos);
         v.SetType(Variant::typeUInt32);
      }
      break;

   case TYPE_TO_PROP_ID(propAvailableShots):
      {
         cdUInt32 numShots = 0;

         // may return cdINVALID_HANDLE, cdINVALID_PARAMETER
         cdError err = CDGetNumAvailableShot(m_hSource, &numShots);
         LOG_TRACE(_T("CDGetNumAvailableShot(source = %08x, &numShots = %08x) returned %08x\n"),
            m_hSource, numShots, err);
         CheckError(_T("CDGetNumAvailableShot"), err, __FILE__, __LINE__);

         v.Set<cdUInt32>(numShots);
         v.SetType(Variant::typeUInt32);
      }
      break;

   // note: propSaveTo is a special case, since the value can't be retrieved.
   // Getting the value is handled in RemoteReleaseControlImpl

   case TYPE_TO_PROP_ID(propBatteryLevel):
      {
         DevicePropertyAccess access(m_hSource);
         v = access.Get(cdDEVICE_PROP_BATTERY_STATUS);
      }
      break;

   CASE_PROP_GET(propImageFormat)

   default:
      ATLASSERT(false);
      break;
   }
#undef CASE_PROP_GET

   return v;
}

void ImagePropertyAccess::Set(unsigned int uiPropId, Variant val)
{
/// switch-case for setting a property
#define CASE_PROP_SET(PROPID) \
   case TYPE_TO_PROP_ID(PROPID##): ATLASSERT(true == s_##PROPID.CanWrite()); s_##PROPID.Set(m_hSource, val); break;

   switch (uiPropId)
   {
   CASE_PROP_SET(propShootingMode)
   CASE_PROP_SET(propAv)
   CASE_PROP_SET(propTv)
   CASE_PROP_SET(propExposureCompensation)
   CASE_PROP_SET(propWhiteBalance)
   CASE_PROP_SET(propDriveMode)

   case propISOSpeed: SetReleaseSetting(cdREL_SET_ISO_SPEED_RATINGS, val); break;
   case propMeteringMode: SetReleaseSetting(cdREL_SET_MLWEI_MODE, val); break;
   case propAFMode: SetReleaseSetting(cdREL_SET_AF_MODE, val); break;

   CASE_PROP_SET(propFlashExposureComp)
   CASE_PROP_SET(propFlashMode)
   CASE_PROP_SET(propAFDistance)
   CASE_PROP_SET(propCurrentZoomPos)

   case propSaveTo:
      {
         cdRelDataKind dataKind = val.Get<cdUInt16>();

         cdError err = CDSelectReleaseDataKind(m_hSource, dataKind);
         LOG_TRACE(_T("CDSelectReleaseDataKind(source = %08x, dataKind = %04x) returned %08x\n"),
            m_hSource, dataKind, err);
         CheckError(_T("CDSelectReleaseDataKind"), err, __FILE__, __LINE__);
      }
      break;

   CASE_PROP_SET(propImageFormat)

   default:
      ATLASSERT(false);
      break;
   }
#undef CASE_PROP_SET
}

void ImagePropertyAccess::Enum(unsigned int uiPropId, std::vector<Variant>& vecValues)
{
/// switch-case for enumerating values of a property
#define CASE_PROP_ENUM(PROPID) \
   case TYPE_TO_PROP_ID(PROPID##): ATLASSERT(true == s_##PROPID.CanEnum()); s_##PROPID.Enum(m_hSource, vecValues); break;

   switch (uiPropId)
   {
   CASE_PROP_ENUM(propShootingMode)
   CASE_PROP_ENUM(propAv)
   CASE_PROP_ENUM(propTv)
   CASE_PROP_ENUM(propExposureCompensation)
   CASE_PROP_ENUM(propWhiteBalance)
   CASE_PROP_ENUM(propDriveMode)

   case propISOSpeed: EnumReleaseSettingValues(cdREL_SET_ISO_SPEED_RATINGS, vecValues); break;
   case propMeteringMode: EnumReleaseSettingValues(cdREL_SET_MLWEI_MODE, vecValues); break;
   case propAFMode: EnumReleaseSettingValues(cdREL_SET_AF_MODE, vecValues); break;

   CASE_PROP_ENUM(propFlashMode)
   CASE_PROP_ENUM(propAFDistance)

   case propSaveTo:
      {
         Variant value;
         value.SetType(Variant::typeUInt16);

         value.Set<cdUInt16>(cdREL_KIND_THUMB_TO_PC);  vecValues.push_back(value);
         value.Set<cdUInt16>(cdREL_KIND_PICT_TO_PC);   vecValues.push_back(value);
         value.Set<cdUInt16>(cdREL_KIND_THUMB_TO_CAM); vecValues.push_back(value);
         value.Set<cdUInt16>(cdREL_KIND_PICT_TO_CAM);  vecValues.push_back(value);
      }
      break;

   CASE_PROP_ENUM(propImageFormat)

   default:
      ATLASSERT(false);
      break;
   }
#undef CASE_PROP_ENUM
}

bool ImagePropertyAccess::IsReadOnly(unsigned int uiPropId)
{
/// switch-case for checking if a property is read-only
#define CASE_PROP_ISREADONLY(PROPID) \
   case TYPE_TO_PROP_ID(PROPID##): if (s_##PROPID.CanWrite()) return true;

   switch (uiPropId)
   {
   CASE_PROP_ISREADONLY(propShootingMode)
   CASE_PROP_ISREADONLY(propAv)
   CASE_PROP_ISREADONLY(propTv)
   CASE_PROP_ISREADONLY(propExposureCompensation)
   CASE_PROP_ISREADONLY(propWhiteBalance)
   CASE_PROP_ISREADONLY(propDriveMode)

   case propISOSpeed: IsReadOnlyReleaseSetting(cdREL_SET_ISO_SPEED_RATINGS); break;
   case propMeteringMode: IsReadOnlyReleaseSetting(cdREL_SET_MLWEI_MODE); break;
   case propAFMode: IsReadOnlyReleaseSetting(cdREL_SET_AF_MODE); break;
   case propFocalLength: return true;

   CASE_PROP_ISREADONLY(propFlashMode)
   CASE_PROP_ISREADONLY(propAFDistance)
   CASE_PROP_ISREADONLY(propCurrentZoomPos)
   case propMaxZoomPos: return true;
   case propSaveTo: return false;
   case propBatteryLevel: return true;
   CASE_PROP_ISREADONLY(propImageFormat)
   }

   return false;
}

void ImagePropertyAccess::EnumAvailReleaseSettings(std::vector<unsigned int>& vecPropIds) const
{
   cdHEnum hEnum = 0;

   // may return cdINVALID_HANDLE, cdINVALID_PARAMETER
   cdError err = CDEnumRelCamSettingReset(m_hSource, &hEnum);
   if (err != cdOK) LOG_TRACE(_T("CDEnumRelCamSettingReset(%08x, &hEnum = %08x) returned %08x\n"), m_hSource, hEnum, err);
   CheckError(_T("CDEnumRelCamSettingReset"), err, __FILE__, __LINE__);

   for (unsigned int uiCount = 0; uiCount < 200; uiCount++)
   {
      cdRelCamSettingStruct relCamSetting = { 0 };

      // may return cdINVALID_HANDLE, cdINVALID_PARAMETER, cdENUM_NA
      err = CDEnumRelCamSettingNext(hEnum, &relCamSetting);
      if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
         break; // end of list

      if (err != cdOK) LOG_TRACE(_T("CDEnumRelCamSettingNext(%08x, &relCamSetting = { propId = %08x \"%s\", readOnly = %s}) returned %08x\n"),
         hEnum,
         relCamSetting.SettingID,
         ImagePropertyAccess::NameFromId(relCamSetting.SettingID).GetString(),
         (relCamSetting.Access & cdATTRIB_WRITE) != 0 ? _T("false") : _T("true"),
         err);

      LOG_TRACE(_T("Available release setting: \"%s\" (%08x)\n"),
         ImagePropertyAccess::NameFromId(relCamSetting.SettingID).GetString(),
         relCamSetting.SettingID);

      CheckError(_T("CDEnumRelCamSettingNext"), err, __FILE__, __LINE__);

      vecPropIds.push_back(relCamSetting.SettingID);
   }

   // may return cdINVALID_HANDLE, cdINVALID_FN_CALL
   err = CDEnumRelCamSettingRelease(hEnum);
   if (err != cdOK) LOG_TRACE(_T("CDEnumRelCamSettingRelease(%08x) returned %08x\n"), hEnum, err);
   CheckError(_T("CDEnumRelCamSettingRelease"), err, __FILE__, __LINE__);
}

Variant ImagePropertyAccess::GetReleaseSetting(cdRelCamSettingID propId) const
{
   // find out required buffer size
   cdUInt32 uiBufferSize = 0;

   // may return cdINVALID_HANDLE, cdINVALID_PARAMETER, cdINVALID_POINTER, cdNOT_SUPPORTED,
   // cdINVALID_ID
   cdError err = CDGetRelCamSettingData(m_hSource, propId, &uiBufferSize, nullptr);
   LOG_TRACE(_T("CDGetRelCamSettingData(%08x, propId = %08x, &size = %u, data = null) returned %08x\n"),
      m_hSource, propId, uiBufferSize, err);
   CheckError(_T("CDGetRelCamSettingData"), err, __FILE__, __LINE__);

   // retrieve data
   std::vector<BYTE> vecData(uiBufferSize, 0);
   ATLASSERT(uiBufferSize == vecData.size());

   err = CDGetRelCamSettingData(m_hSource, propId, &uiBufferSize, &vecData[0]);
   LOG_TRACE(_T("CDGetRelCamSettingData(%08x, propId = %08x, size = %u, data = {...}) returned %08x\n"),
      m_hSource, propId, vecData.size(), err);
   CheckError(_T("CDGetRelCamSettingData"), err, __FILE__, __LINE__);

   Variant value;
   SetRawCdsdk(value, propId, vecData);

   return value;
}

void ImagePropertyAccess::SetReleaseSetting(cdRelCamSettingID propId, Variant value)
{
   std::vector<BYTE> vecData;
   GetRawCdsdk(value, propId, vecData);

   // may return cdINVALID_HANDLE, cdINVALID_PARAMETER, cdINVALID_POINTER, cdNOT_SUPPORTED,
   // cdINVALID_ID
   cdError err = CDSetRelCamSettingData(m_hSource, propId, vecData.size(), vecData.data());
   LOG_TRACE(_T("CDSetRelCamSettingData(%08x, propId = %08x, data = { %u bytes }) returned %08x\n"),
      m_hSource, propId, vecData.size(), err);
   CheckError(_T("CDSetRelCamSettingData"), err, __FILE__, __LINE__);
}

void ImagePropertyAccess::EnumReleaseSettingValues(cdRelCamSettingID propId, std::vector<Variant>& vecValues) const
{
   cdHEnum hEnum = 0;
   cdUInt32 uiBufSize = 0;

   cdError err = CDEnumRelCamSettingDataReset(m_hSource, propId, &hEnum, &uiBufSize);
   if (err != cdOK) LOG_TRACE(_T("CDEnumRelCamSettingDataReset(%08x, propId = %08x, &hEnum = %08x, &bufSize = %u) returned %08x\n"),
      m_hSource, propId, hEnum, uiBufSize, err);
   CheckError(_T("CDEnumRelCamSettingDataReset"), err, __FILE__, __LINE__);

   std::vector<BYTE> vecData(uiBufSize, 0);

   for (unsigned int uiCount = 0; uiCount < 100; uiCount++)
   {
      err = CDEnumRelCamSettingDataNext(hEnum, uiBufSize, &vecData[0]);
      if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
         break; // end of list

      if (err != cdOK) LOG_TRACE(_T("CDEnumRelCamSettingDataNext(%08x, &buffer) returned %08x\n"),
         hEnum, err);

      CheckError(_T("CDEnumRelCamSettingDataNext"), err, __FILE__, __LINE__);

      // add value to array
      Variant value;
      SetRawCdsdk(value, propId, vecData);

      vecValues.push_back(value);
   }

   err = CDEnumRelCamSettingDataRelease(hEnum);
   if (err != cdOK) LOG_TRACE(_T("CDEnumRelCamSettingDataRelease(%08x) returned %08x\n"), hEnum, err);
   CheckError(_T("CDEnumRelCamSettingDataRelease"), err, __FILE__, __LINE__);
}

bool ImagePropertyAccess::IsReadOnlyReleaseSetting(cdRelCamSettingID propId) const
{
   // for some properties, the cdType.h states they are read-only; filter them out here
   switch (propId)
   {
   case cdREL_SET_SELF_TIMER:
   case cdREL_SET_EZOOM:
   case cdREL_SET_SLOW_SHUTTER:
   case cdREL_SET_AF_MODE:
   case cdREL_SET_PARAMETER_SET:
   case cdREL_SET_AEB_EXPOSURE_COMP:
   case cdREL_SET_EF_LENS_ID:
   case cdREL_SET_AV_OPEN: // cdType.h states this property is invalid; assume read-only
   case cdREL_SET_AV_MAX:
   case cdREL_SET_FOCAL_LENGTH:
   case cdREL_SET_FOCAL_LENGTH_TELE:
   case cdREL_SET_FOCAL_LENGTH_WIDE:
   case cdREL_SET_FOCAL_LENGTH_DENOMINATOR:
   case cdREL_SET_DISP_AV:
   case cdREL_SET_AV_OPEN_APEX:
   case cdREL_SET_EZOOM_SIZE:
   case cdREL_SET_FOCAL_ML_SPOT_POS:
   case cdREL_SET_DISP_AV_MAX:
   case cdREL_SET_AV_MAX_APEX:
   case cdREL_SET_FOCAL_LENGTH_OF_TELE:
   case cdREL_SET_EZOOM_SIZE_TELE:
   case cdREL_SET_FLASH_QUANTITY_COUNT:
   case cdREL_SET_ROTATION_ANGLE:
   case cdREL_SET_ROTATION_SENSE:
      return true;
   }

   // TODO enumerate all properties and check if Access value permits write access
   return false;
}

void ImagePropertyAccess::SetRawCdsdk(Variant& value, unsigned int propId, const std::vector<unsigned char>& vecData) const
{
   switch (propId)
   {
      //case cdREL_SET_AV_OPEN:

#if 0
      // 4-byte unsigned int
      if (vecData.size() == 4)
      {
         ATLASSERT(vecData.size() == 4);
         unsigned int uiValue =
            static_cast<unsigned int>(vecData[0]) |
            (static_cast<unsigned int>(vecData[1]) << 8) |
            (static_cast<unsigned int>(vecData[2]) << 16) |
            (static_cast<unsigned int>(vecData[3]) << 24);
         value.Set(uiValue);
         value.Type(Variant::typeUInt32);
      }
      break;
#endif

   case cdREL_SET_SELF_TIMER:
   case cdREL_SET_FOCUS_POINT:
   case cdREL_SET_PARAMETER_SET:
   case cdREL_SET_ISO_SPEED_RATINGS:
   case cdREL_SET_AV_MAX:
   case cdREL_SET_FOCAL_LENGTH:
   case cdREL_SET_FOCAL_LENGTH_TELE:
   case cdREL_SET_FOCAL_LENGTH_WIDE:
   case cdREL_SET_FOCAL_LENGTH_DENOMINATOR:
   case cdREL_SET_DISP_AV:
   case cdREL_SET_AV_OPEN_APEX:
   case cdREL_SET_EZOOM_SIZE:
   case cdREL_SET_FOCAL_ML_SPOT_POS:
   case cdREL_SET_DISP_AV_MAX:
   case cdREL_SET_AV_MAX_APEX:
   case cdREL_SET_FOCAL_LENGTH_OF_TELE:
   case cdREL_SET_EZOOM_SIZE_TELE:
   case cdREL_SET_PHOTO_EFFECT:
   case cdREL_SET_AF_LIGHT:
   case cdREL_SET_FLASH_QUANTITY_COUNT:
   case cdREL_SET_FLASH_QUANTITY:
   case cdREL_SET_ROTATION_ANGLE:
   case cdREL_SET_ROTATION_SENSE:
   case cdREL_SET_KELVIN_VALUE:
   case cdREL_SET_COLOR_MATRIX:
      if (vecData.size() == 2)
      {
         // 2-byte unsigned int
         ATLASSERT(vecData.size() == 2);
         unsigned short usValue =
            static_cast<unsigned int>(vecData[0]) |
            (static_cast<unsigned int>(vecData[1]) << 8);

         value.Set(usValue);
         value.SetType(Variant::typeUInt16);
      }
      break;

   case cdREL_SET_BEEP:
   case cdREL_SET_EZOOM:
   case cdREL_SET_MLWEI_MODE:
   case cdREL_SET_SLOW_SHUTTER:
   case cdREL_SET_AF_MODE:
   case cdREL_SET_CONTRAST:
   case cdREL_SET_COLOR_GAIN:
   case cdREL_SET_SHARPNESS:
   case cdREL_SET_FLASH_COMP:
   case cdREL_SET_AEB_EXPOSURE_COMP:
   case cdREL_SET_EF_LENS_ID:
      if (vecData.size() == 1)
      {
         // byte
         ATLASSERT(vecData.size() == 1);
         unsigned short usValue =
            static_cast<unsigned int>(vecData[0]) |
            (static_cast<unsigned int>(vecData[1]) << 8);

         value.Set(usValue);
         value.SetType(Variant::typeUInt8);
      }
      break;

   default:
      // just store array of bytes
      value.SetArray<unsigned char>(vecData);
      value.SetType(Variant::typeUInt8);
      break;
   }

   if (value.Type() == Variant::typeInvalid)
   {
      value.SetArray<unsigned char>(vecData);
      value.SetType(Variant::typeUInt8);
   }
}

void ImagePropertyAccess::GetRawCdsdk(const Variant& value, unsigned int /*propId*/, std::vector<unsigned char>& vecData) const
{
   if (value.IsArray())
   {
      ATLASSERT(value.Type() == Variant::typeUInt8);
      vecData = value.GetArray<unsigned char>();

      return;
   }

   if (value.Type() == Variant::typeUInt8)
   {
      vecData.push_back(value.Get<unsigned char>());
   }
   else
   if (value.Type() == Variant::typeUInt16)
   {
      unsigned short usValue = value.Get<unsigned short>();
      vecData.push_back(static_cast<unsigned char>(usValue & 0xff));
      vecData.push_back(static_cast<unsigned char>((usValue >> 8) & 0xff));
   }
   else
   if (value.Type() == Variant::typeUInt32)
   {
      unsigned int uiValue = value.Get<unsigned int>();
      vecData.push_back(static_cast<unsigned char>(uiValue & 0xff));
      vecData.push_back(static_cast<unsigned char>((uiValue >> 8) & 0xff));
      vecData.push_back(static_cast<unsigned char>((uiValue >> 16) & 0xff));
      vecData.push_back(static_cast<unsigned char>((uiValue >> 24) & 0xff));
   }
   else
      ATLASSERT(false);
}

unsigned int ImagePropertyAccess::MapToPropertyID(T_enImagePropertyType enPropertyType) throw()
{
   // map the type values using macro, so that they don't clash with cdRelCamSettingID
   return TYPE_TO_PROP_ID(static_cast<unsigned int>(enPropertyType));
}

CString ImagePropertyAccess::NameFromId(unsigned int propId)
{
   LPCTSTR pszName = _T("???");
   switch (propId)
   {
      // properties from T_enImagePropertyType
   case TYPE_TO_PROP_ID(propShootingMode): pszName = _T("Shooting mode"); break;
   case TYPE_TO_PROP_ID(propDriveMode): pszName = _T("Drive mode"); break;
   case TYPE_TO_PROP_ID(propAv): pszName = _T("Av value"); break;
   case TYPE_TO_PROP_ID(propTv): pszName = _T("Tv value"); break;
   case TYPE_TO_PROP_ID(propExposureCompensation): pszName = _T("Exposure compensation"); break;
   case TYPE_TO_PROP_ID(propFlashExposureComp): pszName = _T("Flash exposure compensation"); break;
   case TYPE_TO_PROP_ID(propFocalLength): pszName = _T("Focal length"); break;
   case TYPE_TO_PROP_ID(propFlashMode): pszName = _T("Flash mode"); break;
   case TYPE_TO_PROP_ID(propWhiteBalance): pszName = _T("White balance"); break;
   case TYPE_TO_PROP_ID(propAFDistance): pszName = _T("AF distance"); break;
   case TYPE_TO_PROP_ID(propCurrentZoomPos): pszName = _T("Current zoom pos"); break;
   case TYPE_TO_PROP_ID(propMaxZoomPos): pszName = _T("Max. zoom pos"); break;
   case TYPE_TO_PROP_ID(propAvailableShots): pszName = _T("Available shots"); break;
   case TYPE_TO_PROP_ID(propSaveTo): pszName = _T("Save to"); break;
   case TYPE_TO_PROP_ID(propBatteryLevel): pszName = _T("Battery level"); break;
   case TYPE_TO_PROP_ID(propImageFormat): pszName = _T("Image format"); break;

      // cdRelCamSettingID properties
   case cdREL_SET_SELF_TIMER: pszName = _T("Self timer"); break;
   case cdREL_SET_BEEP: pszName = _T("Beep setting"); break;
   case cdREL_SET_EZOOM: pszName = _T("Digital teleconverter"); break;
   case cdREL_SET_MLWEI_MODE: pszName = _T("Measure light weight mode"); break;
   case cdREL_SET_FOCUS_POINT: pszName = _T("Focus point"); break;
   case cdREL_SET_SLOW_SHUTTER: pszName = _T("Slow shutter"); break;
   case cdREL_SET_AF_MODE: pszName = _T("Focus mode"); break;
   case cdREL_SET_CONTRAST: pszName = _T("Contrast"); break;
   case cdREL_SET_COLOR_GAIN: pszName = _T("Color saturation"); break;
   case cdREL_SET_SHARPNESS: pszName = _T("Sharpness"); break;
   case cdREL_SET_PARAMETER_SET: pszName = _T("Parameters Set information"); break;
   case cdREL_SET_ISO_SPEED_RATINGS: pszName = _T("ISO speed"); break;
   case cdREL_SET_FLASH_COMP: pszName = _T("Flash compensation"); break;
   case cdREL_SET_AEB_EXPOSURE_COMP: pszName = _T("AEB exposure compensation"); break;
   case cdREL_SET_EF_LENS_ID: pszName = _T("EF lens ID"); break;
   case cdREL_SET_AV_OPEN: pszName = _T("invalid (cdREL_SET_AV_OPEN)"); break;
   case cdREL_SET_AV_MAX: pszName = _T("Maximum Av value"); break;
   case cdREL_SET_FOCAL_LENGTH: pszName = _T("Focal length numerator"); break;
   case cdREL_SET_FOCAL_LENGTH_TELE: pszName = _T("Focal length numerator for tele"); break;
   case cdREL_SET_FOCAL_LENGTH_WIDE: pszName = _T("Focal length numerator for wide"); break;
   case cdREL_SET_FOCAL_LENGTH_DENOMINATOR: pszName = _T("Focal length denominator"); break;
   case cdREL_SET_DISP_AV: pszName = _T("Type of smallest aperture value"); break;
   case cdREL_SET_AV_OPEN_APEX: pszName = _T("Smallest aperture value"); break;
   case cdREL_SET_EZOOM_SIZE: pszName = _T("Digital zoom size of CCD"); break;
   case cdREL_SET_FOCAL_ML_SPOT_POS: pszName = _T("Spot AE point"); break;
   case cdREL_SET_DISP_AV_MAX: pszName = _T("Type of maximum aperture value"); break;
   case cdREL_SET_AV_MAX_APEX: pszName = _T("Maximum aperture value"); break;
   case cdREL_SET_FOCAL_LENGTH_OF_TELE: pszName = _T("Focal length for tele"); break;
   case cdREL_SET_EZOOM_SIZE_TELE: pszName = _T("Digital zoom size of CCD for tele"); break;
   case cdREL_SET_PHOTO_EFFECT: pszName = _T("Photo effect mode"); break;
   case cdREL_SET_AF_LIGHT: pszName = _T("AF-assist beam"); break;
   case cdREL_SET_FLASH_QUANTITY_COUNT: pszName = _T("Number of levels of flash luminescence"); break;
   case cdREL_SET_FLASH_QUANTITY: pszName = _T("Quantity of flash luminescence"); break;
   case cdREL_SET_ROTATION_ANGLE: pszName = _T("Rotation angle"); break;
   case cdREL_SET_ROTATION_SENSE: pszName = _T("Rotation sensing active"); break;
   case cdREL_SET_KELVIN_VALUE: pszName = _T("White balance kelvin value"); break;
   case cdREL_SET_COLOR_MATRIX: pszName = _T("Color matrix"); break;
   default:
      ATLASSERT(false);
      pszName = _T("unknown");
      break;
   }

   return pszName;
}

CString ImagePropertyAccess::DisplayTextFromIdAndValue(unsigned int propId, Variant value)
{
   CString cszText;

   switch (propId)
   {
      // TODO cdREL_SET_FOCUS_POINT
      // TODO cdREL_SET_PARAMETER_SET

   case cdREL_SET_ISO_SPEED_RATINGS:
      cszText = FormatIsoValue(value);
      break;

   case TYPE_TO_PROP_ID(propTv):
      cszText = FormatShutterSpeedValue(value);
      break;

   case cdREL_SET_FLASH_COMP:
   case cdREL_SET_AEB_EXPOSURE_COMP:
   case TYPE_TO_PROP_ID(propExposureCompensation):
      cszText = FormatCompensationValue(value, false);
      break;

   case TYPE_TO_PROP_ID(propAv):
   case cdREL_SET_AV_MAX:
      cszText = FormatApexValue(value);
      break;

      // TODO cdREL_SET_AV_OPEN_APEX
      // TODO cdREL_SET_AV_MAX_APEX

   case cdREL_SET_KELVIN_VALUE:
      cszText.Format(_T("%u K"), value.Get<cdUInt16>());
      break;

   default:
      if (!FormatValueById(g_aPropIdDisplayInfo, sizeof(g_aPropIdDisplayInfo) / sizeof(*g_aPropIdDisplayInfo),
          propId, value, cszText))
         cszText = value.ToString();
      break;
   }

   return cszText;
}

CString ImagePropertyAccess::FormatImageFormatValue(const Variant& value)
{
   ATLASSERT(value.IsArray() == true);
   ATLASSERT(value.Type() == Variant::typeUInt16);

   std::vector<cdUInt16> vecValues = value.GetArray<cdUInt16>();
   ATLASSERT(vecValues.size() == 2);

   cdCompQuality compQuality = static_cast<cdCompQuality>(vecValues[0]);
   cdImageSize imageSize = static_cast<cdImageSize>(vecValues[1]);

   CString cszText;
   switch (compQuality)
   {
   case cdCOMP_QUALITY_UNKNOWN: cszText = _T("Unknown"); break;
   case cdCOMP_QUALITY_ECONOMY: cszText = _T("Economy"); break;
   case cdCOMP_QUALITY_NORMAL: cszText = _T("Normal"); break;
   case cdCOMP_QUALITY_FINE: cszText = _T("Fine"); break;
   //case cdCOMP_QUALITY_LOSSLESS: same as cdCOMP_QUALITY_RAW
   case cdCOMP_QUALITY_RAW: cszText = _T("Raw"); break;
   case cdCOMP_QUALITY_SUPERFINE: cszText = _T("Superfine"); break;
   default:
      ATLASSERT(false);
      cszText = _T("???");
      break;
   }

   cszText += _T(" ");
   switch (imageSize)
   {
   case cdIMAGE_SIZE_UNKNOWN: cszText += _T("Unknown"); break;
   case cdIMAGE_SIZE_LARGE:   cszText += _T("Large"); break;
   case cdIMAGE_SIZE_MEDIUM:  cszText += _T("Medium"); break;
   case cdIMAGE_SIZE_SMALL:   cszText += _T("Small"); break;
   case cdIMAGE_SIZE_MEDIUM1: cszText += _T("Medium1"); break;
   case cdIMAGE_SIZE_MEDIUM2: cszText += _T("Medium2"); break;
   case cdIMAGE_SIZE_MEDIUM3: cszText += _T("Medium3"); break;
   default:
      ATLASSERT(false);
      break;
   }

   return cszText;
}
