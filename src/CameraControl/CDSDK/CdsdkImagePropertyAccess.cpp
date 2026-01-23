//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file CdsdkImagePropertyAccess.cpp CDSDK - Image property access
//
#include "stdafx.h"
#include "CdsdkImagePropertyAccess.hpp"
#include "CdsdkDevicePropertyAccess.hpp"
#include "CommonPropertyAccess.hpp"

using namespace CDSDK;

/// converts property type enum (T_enImagePropertyType) to property id.
/// since there are some release settings in the lower area, add a large enough constant
#define TYPE_TO_PROP_ID(x) (x+0x00010000)


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
      cdREL_SET_BEEP,
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
         { 0xff, _T("Invalid") },
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

   {
      TYPE_TO_PROP_ID(propShootingMode),
      {
         { cdSHOOTING_MODE_INVALID, _T("Invalid") },
         { cdSHOOTING_MODE_AUTO, _T("Auto") },
         { cdSHOOTING_MODE_PROGRAM, _T("Program") },
         { cdSHOOTING_MODE_TV, _T("Tv") },
         { cdSHOOTING_MODE_AV, _T("Av") },
         { cdSHOOTING_MODE_MANUAL, _T("Manual") },
         { cdSHOOTING_MODE_A_DEP, _T("A Dep") },
         { cdSHOOTING_MODE_M_DEP, _T("M Dep") },
         { cdSHOOTING_MODE_BULB, _T("Bulb") },
         { cdSHOOTING_MODE_MANUAL_2, _T("Manual 2") },
         { cdSHOOTING_MODE_FAR_SCENE, _T("Far scene") },
         { cdSHOOTING_MODE_FAST_SHUTTER, _T("Fast shutter") },
         { cdSHOOTING_MODE_SLOW_SHUTTER, _T("Show shutter") },
         { cdSHOOTING_MODE_NIGHT_SCENE, _T("Night scene") },
         { cdSHOOTING_MODE_GRAY_SCALE, _T("Grayscale") },
         { cdSHOOTING_MODE_SEPIA, _T("Sepia") },
         { cdSHOOTING_MODE_PORTRAIT, _T("Portrait") },
         { cdSHOOTING_MODE_SPOT, _T("Spot") },
         { cdSHOOTING_MODE_MACRO, _T("Macro") },
         { cdSHOOTING_MODE_BW, _T("B/W") },
         { cdSHOOTING_MODE_PANFOCUS, _T("Pan focus") },
         { cdSHOOTING_MODE_VIVID, _T("Vivid") },
         { cdSHOOTING_MODE_NEUTRAL, _T("Neutral") },
         { cdSHOOTING_MODE_FLASH_OFF, _T("Flash off") },
         { cdSHOOTING_MODE_LONG_SHUTTER, _T("Long shutter") },
         { cdSHOOTING_MODE_SUPER_MACRO, _T("Super macro") },
         { cdSHOOTING_MODE_FOLIAGE, _T("Foilage") },
         { cdSHOOTING_MODE_INDOOR, _T("Indoor") },
         { cdSHOOTING_MODE_FIREWORKS, _T("Fireworks") },
         { cdSHOOTING_MODE_BEACH, _T("Beach") },
         { cdSHOOTING_MODE_UNDERWATER, _T("Underwater") },
         { cdSHOOTING_MODE_SNOW, _T("Snow") },
         { cdSHOOTING_MODE_KIDS_AND_PETS, _T("Kids and pets") },
         { cdSHOOTING_MODE_NIGHT_SNAPSHOT, _T("Night snapshot") },
         { cdSHOOTING_MODE_DIGITAL_MACRO, _T("Digital macro") },
         { cdSHOOTING_MODE_MYCOLORS, _T("My colors") },
         { cdSHOOTING_MODE_PHOTO_IN_MOVIE, _T("Photo in movie") },
         { 0, nullptr }
      }
   },

   {
      TYPE_TO_PROP_ID(propWhiteBalance),
      {
         { cdWB_ATCAPTURE, _T("As shot value") },
         { cdWB_BY_POS, _T("White Point position") },
         { cdWB_USE_COEFFICIENT, _T("WB coefficient") },
         { cdWB_INVALID, _T("Invalid value") },
         { cdWB_AUTO, _T("Auto") },
         { cdWB_DAY_LIGHT, _T("Daylight") },
         { cdWB_CLOUDY, _T("Cloudy") },
         { cdWB_TUNGSTEN, _T("Tungsten") },
         { cdWB_FLUORESCENT, _T("Fluorescent") },
         { cdWB_FLASH, _T("Flash") },
         { cdWB_CUSTOM, _T("Custom") },
         { cdWB_BW, _T("B/W") },
         { cdWB_SHADE, _T("Shade for Type B  (and Type C)") },
         { cdWB_KELVIN, _T("Kelvin for Type B  (and Type C)") },
         { cdWB_PCSET1, _T("PC Set1 for Type B") },
         { cdWB_PCSET2, _T("PC Set2 for Type B") },
         { cdWB_PCSET3, _T("PC Set3 for Type B") },
         { cdWB_FLUORESCENT_H, _T("Fluorescent H") },
         { cdWB_CUSTOM_1, _T("Custom 1") },
         { cdWB_CUSTOM_2, _T("Custom 2") },
         { 0, nullptr }
      }
   },

   {
      TYPE_TO_PROP_ID(propDriveMode),
      {
         { cdDRIVE_MODE_UNKNOWN, _T("Unknown") },
         { cdDRIVE_MODE_SINGLE, _T("Single") },
         { cdDRIVE_MODE_CONT, _T("Continuous") },
         { cdDRIVE_MODE_MOVIE, _T("Movie") },
         { cdDRIVE_MODE_CONT_SPEED_PRIORITY, _T("Continuous; speed priority") },
         { cdDRIVE_MODE_CONT_LOW, _T("Continuous; low") },
         { cdDRIVE_MODE_CONT_HIGHT, _T("Continuous; high") },
         { cdDRIVE_MODE_SELFTIMER, _T("Self timer") },
         { 0, nullptr }
      }
   },

   {
      TYPE_TO_PROP_ID(propFlashMode),
      {
         { cdFLASH_MODE_OFF, _T("Off") },
         { cdFLASH_MODE_AUTO, _T("Auto") },
         { cdFLASH_MODE_ON, _T("On") },
         { cdFLASH_MODE_RED_EYE, _T("Red eye") },
         { cdFLASH_MODE_SLOW_SYNC, _T("Slow sync") },
         { cdFLASH_MODE_AUTO_PLUS_RED_EYE, _T("Auto + Red eye") },
         { cdFLASH_MODE_ON_PLUS_RED_EYE, _T("On + Red eye") },
         { 0, nullptr }
      }
   },

   {
      TYPE_TO_PROP_ID(propAFDistance),
      {
         { cdAF_DISTANCE_MANUAL, _T("Manual") },
         { cdAF_DISTANCE_AUTO, _T("Auto") },
         { cdAF_DISTANCE_UNKNOWN, _T("Unknown") },
         { cdAF_DISTANCE_CLOSE_UP, _T("Close up") },
         { cdAF_DISTANCE_VERY_CLOSE, _T("Very close") },
         { cdAF_DISTANCE_CLOSE, _T("Close") },
         { cdAF_DISTANCE_MIDDLE, _T("Middle") },
         { cdAF_DISTANCE_FAR, _T("Far") },
         { cdAF_DISTANCE_PAN_FOCUS, _T("Pan focus") },
         { cdAF_DISTANCE_SUPER_MACRO, _T("Super macro") },
         { cdAF_DISTANCE_INFINITY, _T("Infinity") },
         { cdAF_DISTANCE_SUPER_MACRO_0CM, _T("Super macro 0 cm") },
         { cdAF_DISTANCE_NA, _T("N/A") },
         { 0, nullptr }
      }
   },

   {
      TYPE_TO_PROP_ID(propSaveTo),
      {
         { cdREL_KIND_THUMB_TO_PC, _T("Thumbnail to PC") },
         { cdREL_KIND_PICT_TO_PC, _T("Image to PC") },
         { cdREL_KIND_THUMB_TO_CAM, _T("Thumbnail on camera") },
         { cdREL_KIND_PICT_TO_CAM, _T("Image on camera") },
         { 0, nullptr }
      }
   },

   {
      TYPE_TO_PROP_ID(propBatteryLevel),
      {
         { BATTERY_LEVEL_NORMAL, _T("Normal") },
         { BATTERY_LEVEL_WEAK, _T("Weak") },
         { BATTERY_LEVEL_SAFETY_LOW, _T("Safety low") },
         { BATTERY_LEVEL_LB, _T("LB") },
         { 0, nullptr }
      }

   },
};


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
      LOG_TRACE(_T("PropertyAccess::Get(source = %08x, propId = %08x \"%s\", &value = %04x) returned %08x\n"),
         hSource, TYPE_TO_PROP_ID(propId), ImagePropertyAccess::NameFromId(TYPE_TO_PROP_ID(propId)), val, err);
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
class PropertyAccessReadWrite : public PropertyAccessRead<propId, T, defaultVal>
{
   /// base class typedef
   typedef PropertyAccessRead<propId, T, defaultVal> BaseClass;

public:
   /// get function
   typedef BaseClass::T_fnGet T_fnGet;

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
      LOG_TRACE(_T("PropertyAccess::Set(source = %08x, propId = %08x \"%s\", value = %04x) returned %08x\n"),
         hSource, TYPE_TO_PROP_ID(propId), ImagePropertyAccess::NameFromId(TYPE_TO_PROP_ID(propId)), val, err);
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
class PropertyAccessReadWriteEnum : public PropertyAccessReadWrite<propId, T, defaultVal>
{
   /// base class typedef
   typedef PropertyAccessReadWrite<propId, T, defaultVal> BaseClass;

public:
   typedef BaseClass::T_fnGet T_fnGet; ///< get function
   typedef BaseClass::T_fnSet T_fnSet; ///< set function
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
      if (err != cdOK) LOG_TRACE(_T("PropertyAccess::Enum::Reset(source = %08x, propId = %08x \"%s\", &hEnum = %08x) returned %08x\n"),
         hSource, TYPE_TO_PROP_ID(propId), ImagePropertyAccess::NameFromId(TYPE_TO_PROP_ID(propId)), hEnum, err);
      CheckError(_T("PropertyAccess::Enum::Reset"), err, __FILE__, __LINE__);

      // get count
      cdUInt32 uiCount = 0;
      err = m_fnEnumCount(hEnum, &uiCount);
      if (err != cdOK) LOG_TRACE(_T("PropertyAccess::Enum::Count(hEnum = %08x, &count = %u) returned %08x\n"), hEnum, uiCount, err);
      CheckError(_T("PropertyAccess::Enum::Count"), err, __FILE__, __LINE__);

      // get all values
      for (cdUInt32 ui = 0; ui < uiCount; ui++)
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
      for (cdUInt32 ui = 0; ui < uiCount; ui++)
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

/// switch-case for getting a property
#define CASE_PROP_GET(PROPID) \
   case TYPE_TO_PROP_ID(PROPID##): ATLASSERT(true == s_##PROPID.CanRead()); v = s_##PROPID.Get(m_hSource); break;

Variant ImagePropertyAccess::Get(unsigned int uiPropId) const
{
   Variant v;

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
      // might be a release setting
      v = GetReleaseSetting(uiPropId);
      break;
   }

   return v;
}
#undef CASE_PROP_GET

/// switch-case for setting a property
#define CASE_PROP_SET(PROPID) \
   case TYPE_TO_PROP_ID(PROPID##): ATLASSERT(true == s_##PROPID.CanWrite()); s_##PROPID.Set(m_hSource, val); break;

void ImagePropertyAccess::Set(unsigned int uiPropId, Variant val)
{
   switch (uiPropId)
   {
   CASE_PROP_SET(propShootingMode)
   CASE_PROP_SET(propAv)
   CASE_PROP_SET(propTv)
   CASE_PROP_SET(propExposureCompensation)
   CASE_PROP_SET(propWhiteBalance)
   CASE_PROP_SET(propDriveMode)

   case TYPE_TO_PROP_ID(propISOSpeed): SetReleaseSetting(cdREL_SET_ISO_SPEED_RATINGS, val); break;
   case TYPE_TO_PROP_ID(propMeteringMode): SetReleaseSetting(cdREL_SET_MLWEI_MODE, val); break;
   case TYPE_TO_PROP_ID(propAFMode): SetReleaseSetting(cdREL_SET_AF_MODE, val); break;

   CASE_PROP_SET(propFlashExposureComp)
   CASE_PROP_SET(propFlashMode)
   CASE_PROP_SET(propAFDistance)
   CASE_PROP_SET(propCurrentZoomPos)

   case TYPE_TO_PROP_ID(propSaveTo):
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
      // might be a release setting
      SetReleaseSetting(uiPropId, val);
      break;
   }
}
#undef CASE_PROP_SET

/// switch-case for enumerating values of a property
#define CASE_PROP_ENUM(PROPID) \
   case TYPE_TO_PROP_ID(PROPID##): ATLASSERT(true == s_##PROPID.CanEnum()); s_##PROPID.Enum(m_hSource, vecValues); break;

void ImagePropertyAccess::Enum(unsigned int uiPropId, std::vector<Variant>& vecValues)
{
   switch (uiPropId)
   {
      // these properties have special cases when they throw a "Not supported" error
   case TYPE_TO_PROP_ID(propShootingMode):
      try
      {
         s_propShootingMode.Enum(m_hSource, vecValues);
      }
      catch (const CameraException& ex)
      {
         if (ex.ErrorCode() == 0x0c000007)
            AddCameraModelShootingMode(vecValues);
         else
            throw;
      }
      break;

   case TYPE_TO_PROP_ID(propAv):
      try
      {
         s_propAv.Enum(m_hSource, vecValues);
      }
      catch (const CameraException& ex)
      {
         if (ex.ErrorCode() == 0x0c000007)
            AddCameraModelAvValues(vecValues);
         else
            throw;
      }
      break;

   case TYPE_TO_PROP_ID(propTv):
      try
      {
         s_propTv.Enum(m_hSource, vecValues);
      }
      catch (const CameraException& ex)
      {
         if (ex.ErrorCode() == 0x0c000007)
            AddCameraModelTvValues(vecValues);
         else
            throw;
      }
      break;

   CASE_PROP_ENUM(propExposureCompensation)
   CASE_PROP_ENUM(propWhiteBalance)
   CASE_PROP_ENUM(propDriveMode)

   case TYPE_TO_PROP_ID(propISOSpeed): EnumReleaseSettingValues(cdREL_SET_ISO_SPEED_RATINGS, vecValues); break;
   case TYPE_TO_PROP_ID(propMeteringMode): EnumReleaseSettingValues(cdREL_SET_MLWEI_MODE, vecValues); break;
   case TYPE_TO_PROP_ID(propAFMode): EnumReleaseSettingValues(cdREL_SET_AF_MODE, vecValues); break;

   CASE_PROP_ENUM(propFlashMode)
   CASE_PROP_ENUM(propAFDistance)

   case TYPE_TO_PROP_ID(propSaveTo):
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

         // there's no enum function for zoom positions; add the values by ourselves
   case TYPE_TO_PROP_ID(propCurrentZoomPos):
      {
         Variant v;
         v.SetType(Variant::typeUInt32);

         cdUInt32 uiMax = Get(TYPE_TO_PROP_ID(propMaxZoomPos)).Get<cdUInt32>();
         for (cdUInt32 ui = 0; ui <= uiMax; ui++)
         {
            v.Set(ui);
            vecValues.push_back(v);
         }
      }
      break;

      // these properties make no sense to enumerate
   case cdREL_SET_EZOOM:
      break;

   default:
      ATLASSERT(false);
      break;
   }
}
#undef CASE_PROP_ENUM

/// switch-case for checking if a property is read-only
#define CASE_PROP_ISREADONLY(PROPID) \
   case TYPE_TO_PROP_ID(PROPID##): return !s_##PROPID.CanWrite();

bool ImagePropertyAccess::IsReadOnly(unsigned int uiPropId)
{
   switch (uiPropId)
   {
   CASE_PROP_ISREADONLY(propShootingMode)
   CASE_PROP_ISREADONLY(propAv)
   CASE_PROP_ISREADONLY(propTv)
   CASE_PROP_ISREADONLY(propExposureCompensation)
   CASE_PROP_ISREADONLY(propWhiteBalance)
   CASE_PROP_ISREADONLY(propDriveMode)

   case TYPE_TO_PROP_ID(propISOSpeed): IsReadOnlyReleaseSetting(cdREL_SET_ISO_SPEED_RATINGS); break;
   case TYPE_TO_PROP_ID(propMeteringMode): IsReadOnlyReleaseSetting(cdREL_SET_MLWEI_MODE); break;
   case TYPE_TO_PROP_ID(propAFMode): IsReadOnlyReleaseSetting(cdREL_SET_AF_MODE); break;
   case TYPE_TO_PROP_ID(propFocalLength): return true;

   CASE_PROP_ISREADONLY(propFlashMode)
   CASE_PROP_ISREADONLY(propAFDistance)
   CASE_PROP_ISREADONLY(propCurrentZoomPos)
   case TYPE_TO_PROP_ID(propMaxZoomPos): return true;
   case TYPE_TO_PROP_ID(propSaveTo): return false;
   case TYPE_TO_PROP_ID(propBatteryLevel): return true;
   CASE_PROP_ISREADONLY(propImageFormat)
   default:
      return IsReadOnlyReleaseSetting(uiPropId);
   }

   return true;
}
#undef CASE_PROP_ISREADONLY

void ImagePropertyAccess::EnumAvailReleaseSettings(std::vector<unsigned int>& vecPropIds) const
{
   cdHEnum hEnum = 0;

   // may return cdINVALID_HANDLE, cdINVALID_PARAMETER
   cdError err = CDEnumRelCamSettingReset(m_hSource, &hEnum);
   if (err != cdOK) LOG_TRACE(_T("CDEnumRelCamSettingReset(source = %08x, &hEnum = %08x) returned %08x\n"), m_hSource, hEnum, err);
   CheckError(_T("CDEnumRelCamSettingReset"), err, __FILE__, __LINE__);

   for (unsigned int uiCount = 0; uiCount < 200; uiCount++)
   {
      cdRelCamSettingStruct relCamSetting = { 0 };

      // may return cdINVALID_HANDLE, cdINVALID_PARAMETER, cdENUM_NA
      err = CDEnumRelCamSettingNext(hEnum, &relCamSetting);
      if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
         break; // end of list

      if (err != cdOK) LOG_TRACE(_T("CDEnumRelCamSettingNext(hEnum = %08x, &relCamSetting = { propId = %08x \"%s\", readOnly = %s}) returned %08x\n"),
         hEnum,
         relCamSetting.SettingID,
         NameFromId(relCamSetting.SettingID),
         (relCamSetting.Access & cdATTRIB_WRITE) != 0 ? _T("false") : _T("true"),
         err);

      LOG_TRACE(_T("Available image property: \"%s\" (%08x)\n"),
         NameFromId(relCamSetting.SettingID),
         relCamSetting.SettingID);

      CheckError(_T("CDEnumRelCamSettingNext"), err, __FILE__, __LINE__);

      vecPropIds.push_back(relCamSetting.SettingID);
   }

   // may return cdINVALID_HANDLE, cdINVALID_FN_CALL
   err = CDEnumRelCamSettingRelease(hEnum);
   if (err != cdOK) LOG_TRACE(_T("CDEnumRelCamSettingRelease(hEnum = %08x) returned %08x\n"), hEnum, err);
   CheckError(_T("CDEnumRelCamSettingRelease"), err, __FILE__, __LINE__);
}

Variant ImagePropertyAccess::GetReleaseSetting(cdRelCamSettingID propId) const
{
   // find out required buffer size
   cdUInt32 uiBufferSize = 0;

   // may return cdINVALID_HANDLE, cdINVALID_PARAMETER, cdINVALID_POINTER, cdNOT_SUPPORTED,
   // cdINVALID_ID
   cdError err = CDGetRelCamSettingData(m_hSource, propId, &uiBufferSize, nullptr);
   if (err != cdOK) LOG_TRACE(_T("CDGetRelCamSettingData(source = %08x, propId = %08x \"%s\", &size = %u, data = null) returned %08x\n"),
      m_hSource, propId, NameFromId(propId), uiBufferSize, err);
   CheckError(_T("CDGetRelCamSettingData"), err, __FILE__, __LINE__);

   // retrieve data
   std::vector<BYTE> vecData(uiBufferSize, 0);
   ATLASSERT(uiBufferSize == vecData.size());

   err = CDGetRelCamSettingData(m_hSource, propId, &uiBufferSize, &vecData[0]);
   if (err != cdOK) LOG_TRACE(_T("CDGetRelCamSettingData(source = %08x, propId = %08x \"%s\", size = %u, data = {...}) returned %08x\n"),
      m_hSource, propId, NameFromId(propId), vecData.size(), err);
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
   LOG_TRACE(_T("CDSetRelCamSettingData(source = %08x, propId = %08x \"%s\", data = { %u bytes }) returned %08x\n"),
      m_hSource, propId, NameFromId(propId), vecData.size(), err);
   CheckError(_T("CDSetRelCamSettingData"), err, __FILE__, __LINE__);
}

void ImagePropertyAccess::EnumReleaseSettingValues(cdRelCamSettingID propId, std::vector<Variant>& vecValues) const
{
   cdHEnum hEnum = 0;
   cdUInt32 uiBufSize = 0;

   cdError err = CDEnumRelCamSettingDataReset(m_hSource, propId, &hEnum, &uiBufSize);
   if (err != cdOK) LOG_TRACE(_T("CDEnumRelCamSettingDataReset(source = %08x, propId = %08x \"%s\", &hEnum = %08x, &bufSize = %u) returned %08x\n"),
      m_hSource, propId, NameFromId(propId), hEnum, uiBufSize, err);
   CheckError(_T("CDEnumRelCamSettingDataReset"), err, __FILE__, __LINE__);

   std::vector<BYTE> vecData(uiBufSize, 0);

   for (unsigned int uiCount = 0; uiCount < 100; uiCount++)
   {
      err = CDEnumRelCamSettingDataNext(hEnum, uiBufSize, &vecData[0]);
      if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
         break; // end of list

      if (err != cdOK) LOG_TRACE(_T("CDEnumRelCamSettingDataNext(hEnum = %08x, &buffer) returned %08x\n"),
         hEnum, err);

      CheckError(_T("CDEnumRelCamSettingDataNext"), err, __FILE__, __LINE__);

      // add value to array
      Variant value;
      SetRawCdsdk(value, propId, vecData);

      vecValues.push_back(value);
   }

   err = CDEnumRelCamSettingDataRelease(hEnum);
   if (err != cdOK) LOG_TRACE(_T("CDEnumRelCamSettingDataRelease(hEnum = %08x) returned %08x\n"), hEnum, err);
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

   // enumerate all properties and check if Access value permits write access
   bool isReadOnly = true;
   cdHEnum hEnum = 0;

   // may return cdINVALID_HANDLE, cdINVALID_PARAMETER
   cdError err = CDEnumImageItemPropertyReset(m_hSource, &hEnum);
   if (err != cdOK)
      LOG_TRACE(_T("CDEnumImageItemPropertyReset(source = %08x, &hEnum = %08x) returned %08x\n"), m_hSource, hEnum, err);
   CheckError(_T("CDEnumImageItemPropertyReset"), err, __FILE__, __LINE__);

   for (unsigned int uiCount = 0; uiCount < 200; uiCount++)
   {
      cdImagePropertyStruct imagePropertyStruct = { 0 };

      // may return cdINVALID_HANDLE, cdINVALID_PARAMETER, cdENUM_NA
      err = CDEnumImageItemPropertyNext(hEnum, &imagePropertyStruct);
      if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
         break; // end of list

      if (err != cdOK)
         LOG_TRACE(_T("CDEnumImageItemPropertyNext(hEnum = %08x, &imagePropertyStruct = { PicPropID = %08x \"%s\", readOnly = %s}) returned %08x\n"),
            hEnum,
            imagePropertyStruct.PicPropID,
            NameFromId(imagePropertyStruct.PicPropID),
            (imagePropertyStruct.Access & cdATTRIB_WRITE) != 0 ? _T("false") : _T("true"),
            err);

      LOG_TRACE(_T("Available image property: \"%s\" (%08x)\n"),
         NameFromId(imagePropertyStruct.PicPropID),
         imagePropertyStruct.PicPropID);

      CheckError(_T("CDEnumImageItemPropertyNext"), err, __FILE__, __LINE__);

      if (imagePropertyStruct.PicPropID == propId)
      {
         isReadOnly = (imagePropertyStruct.Access & cdATTRIB_WRITE) == 0;
         break;
      }
   }

   // may return cdINVALID_HANDLE, cdINVALID_FN_CALL
   err = CDEnumImageItemPropertyRelease(hEnum);
   if (err != cdOK) LOG_TRACE(_T("CDEnumImageItemPropertyRelease(hEnum = %08x) returned %08x\n"), hEnum, err);
   CheckError(_T("CDEnumImageItemPropertyRelease"), err, __FILE__, __LINE__);

   return isReadOnly;
}

void ImagePropertyAccess::SetRawCdsdk(Variant& value, unsigned int /*propId*/, const std::vector<unsigned char>& vecData)
{
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
      value.SetType(Variant::typeUInt32);
   }
   else if (vecData.size() == 2)
   {
      // 2-byte unsigned int
      ATLASSERT(vecData.size() == 2);
      unsigned short usValue =
         static_cast<unsigned int>(vecData[0]) |
         (static_cast<unsigned int>(vecData[1]) << 8);

      value.Set(usValue);
      value.SetType(Variant::typeUInt16);
   }
   else if (vecData.size() == 1)
   {
      // byte
      ATLASSERT(vecData.size() == 1);
      unsigned char usValue = vecData[0];

      value.Set(usValue);
      value.SetType(Variant::typeUInt8);
   }
   else
   {
      // just store array of bytes
      value.SetArray<unsigned char>(vecData);
      value.SetType(Variant::typeUInt8);
   }
}

void ImagePropertyAccess::GetRawCdsdk(const Variant& value, unsigned int /*propId*/, std::vector<unsigned char>& vecData)
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
   else if (value.Type() == Variant::typeUInt16)
   {
      unsigned short usValue = value.Get<unsigned short>();
      vecData.push_back(static_cast<unsigned char>(usValue & 0xff));
      vecData.push_back(static_cast<unsigned char>((usValue >> 8) & 0xff));
   }
   else if (value.Type() == Variant::typeUInt32)
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

CString ImagePropertyAccess::CameraModel() const
{
   CString cszModel;
   try
   {
      DevicePropertyAccess devicePropAccess(m_hSource);
      cszModel = devicePropAccess.Get(cdDEVICE_PROP_MODEL_NAME).Get<CString>();
   }
   catch (...)
   {
      LOG_TRACE(_T("unknown exception during CameraModel() call\n"));
   }
   return cszModel;
}

void ImagePropertyAccess::AddCameraModelShootingMode(std::vector<Variant>& vecValues)
{
   CString cszModel = CameraModel();

   std::vector<cdShootingMode> vecShootingModes;

   // this corresponds to the table in Appendix A,
   // "Values that can be set in CDSetShootingMode()".
   // Since not all model names are known, every camera model must be
   // tested before this is considered to be working.
   if (cszModel == _T("PowerShot G1") || cszModel.Find(_T("Pro90")) != -1)
   {
      vecShootingModes.push_back(cdSHOOTING_MODE_AUTO);
      vecShootingModes.push_back(cdSHOOTING_MODE_PROGRAM);
      vecShootingModes.push_back(cdSHOOTING_MODE_TV);
      vecShootingModes.push_back(cdSHOOTING_MODE_AV);
      vecShootingModes.push_back(cdSHOOTING_MODE_MANUAL);
      vecShootingModes.push_back(cdSHOOTING_MODE_FAR_SCENE);
      vecShootingModes.push_back(cdSHOOTING_MODE_NIGHT_SCENE);
      vecShootingModes.push_back(cdSHOOTING_MODE_PORTRAIT);
      vecShootingModes.push_back(cdSHOOTING_MODE_BW);
      vecShootingModes.push_back(cdSHOOTING_MODE_PANFOCUS);
   }
   else if (cszModel == _T("PowerShot G2"))
   {
      vecShootingModes.push_back(cdSHOOTING_MODE_AUTO);
      vecShootingModes.push_back(cdSHOOTING_MODE_PROGRAM);
      vecShootingModes.push_back(cdSHOOTING_MODE_TV);
      vecShootingModes.push_back(cdSHOOTING_MODE_AV);
      vecShootingModes.push_back(cdSHOOTING_MODE_MANUAL);
      vecShootingModes.push_back(cdSHOOTING_MODE_FAR_SCENE);
      vecShootingModes.push_back(cdSHOOTING_MODE_NIGHT_SCENE);
      vecShootingModes.push_back(cdSHOOTING_MODE_SEPIA);
      vecShootingModes.push_back(cdSHOOTING_MODE_PORTRAIT);
      vecShootingModes.push_back(cdSHOOTING_MODE_BW);
      vecShootingModes.push_back(cdSHOOTING_MODE_PANFOCUS);
      vecShootingModes.push_back(cdSHOOTING_MODE_VIVID);
      vecShootingModes.push_back(cdSHOOTING_MODE_NEUTRAL);
   }
   else if (cszModel == _T("PowerShot S30") || cszModel == _T("PowerShot S40"))
   {
      vecShootingModes.push_back(cdSHOOTING_MODE_AUTO);
      vecShootingModes.push_back(cdSHOOTING_MODE_PROGRAM);
      vecShootingModes.push_back(cdSHOOTING_MODE_TV);
      vecShootingModes.push_back(cdSHOOTING_MODE_AV);
      vecShootingModes.push_back(cdSHOOTING_MODE_MANUAL);
      vecShootingModes.push_back(cdSHOOTING_MODE_FAR_SCENE);
      vecShootingModes.push_back(cdSHOOTING_MODE_FAST_SHUTTER);
      vecShootingModes.push_back(cdSHOOTING_MODE_SLOW_SHUTTER);
      vecShootingModes.push_back(cdSHOOTING_MODE_NIGHT_SCENE);
      vecShootingModes.push_back(cdSHOOTING_MODE_BW);
      vecShootingModes.push_back(cdSHOOTING_MODE_VIVID);
      vecShootingModes.push_back(cdSHOOTING_MODE_SEPIA);
      vecShootingModes.push_back(cdSHOOTING_MODE_PORTRAIT);
      vecShootingModes.push_back(cdSHOOTING_MODE_NEUTRAL);
   }
   else if (cszModel.Find(_T("A30")) != -1 ||
      cszModel.Find(_T("A40")) != -1 ||
      cszModel.Find(_T("S200")) != -1 ||
      cszModel.Find(_T("S300a")) != -1 ||
      cszModel.Find(_T("IXUS v2")) != -1 ||
      cszModel.Find(_T("330")) != -1 ||
      cszModel.Find(_T("200a")) != -1 ||
      cszModel.Find(_T("300a")) != -1)
   {
      vecShootingModes.push_back(cdSHOOTING_MODE_AUTO);
      vecShootingModes.push_back(cdSHOOTING_MODE_PROGRAM);
      vecShootingModes.push_back(cdSHOOTING_MODE_MANUAL);
   }
   else if (cszModel.Find(_T("A100")) != -1)
   {
      vecShootingModes.push_back(cdSHOOTING_MODE_AUTO);
      vecShootingModes.push_back(cdSHOOTING_MODE_PROGRAM);
   }

   std::for_each(vecShootingModes.begin(), vecShootingModes.end(), [&](cdShootingMode shootingMode)
      {
         Variant val;
         val.Set(shootingMode);
         val.SetType(Variant::typeUInt16);
         vecValues.push_back(val);
      });
}

void ImagePropertyAccess::AddCameraModelAvValues(std::vector<Variant>& vecValues)
{
   CString cszModel = CameraModel();

   cdRemoteSetAv minValue = 0;
   cdRemoteSetAv maxValue = 0;
   bool bAddOpen = false;

   // this corresponds to the table in Appendix A,
   // "Values that can be set in CDSetAvValue() and CDSetTvValue()".
   // Since not all model names are known, every camera model must be
   // tested before this is considered to be working.
   if (cszModel == _T("PowerShot G1"))
   {
      minValue = cdREMOTE_SET_AV3_20;
      maxValue = cdREMOTE_SET_AV3_80;
   }
   else if (cszModel.Find(_T("Pro90")) != -1)
   {
      minValue = cdREMOTE_SET_AV3_28;
      maxValue = cdREMOTE_SET_AV3_80;
   }
   else if (cszModel == _T("PowerShot G2") ||
      cszModel == _T("PowerShot S30") ||
      cszModel == _T("PowerShot S40"))
   {
      minValue = cdREMOTE_SET_AV3_20;
      maxValue = cdREMOTE_SET_AV3_80;
      bAddOpen = true;
   }
   else if (cszModel.Find(_T("A30")) != -1 ||
      cszModel.Find(_T("A40")) != -1 ||
      cszModel.Find(_T("S200")) != -1 ||
      cszModel.Find(_T("S300a")) != -1 ||
      cszModel.Find(_T("IXUS v2")) != -1 ||
      cszModel.Find(_T("330")) != -1 ||
      cszModel.Find(_T("200a")) != -1 ||
      cszModel.Find(_T("300a")) != -1)
   {
      minValue = maxValue = cdREMOTE_SET_AV_Max;
      bAddOpen = true;
   }

   if (minValue == 0 || maxValue == 0)
      return; // cannot be set

   for (cdRemoteSetAv av = minValue; av <= maxValue;)
   {
      Variant val;
      val.Set(av);
      val.SetType(Variant::typeUInt16);
      vecValues.push_back(val);

      unsigned int remainder = av & 7;
      if (remainder == 0 || remainder == 5)
         av += 3;
      else
         av += 2;
   }

   if (bAddOpen)
   {
      Variant val;
      val.Set<cdRemoteSetAv>(cdREMOTE_SET_AV_Open);
      val.SetType(Variant::typeUInt16);
      vecValues.push_back(val);
   }
}

void ImagePropertyAccess::AddCameraModelTvValues(std::vector<Variant>& vecValues)
{
   CString cszModel = CameraModel();

   cdRemoteSetTv minValue = 0;
   cdRemoteSetTv maxValue = 0;
   cdRemoteSetTv additionalValue = 0;

   // this corresponds to the table in Appendix A,
   // "Values that can be set in CDSetAvValue() and CDSetTvValue()".
   // Since not all model names are known, every camera model must be
   // tested before this is considered to be working.
   if (cszModel == _T("PowerShot G1") || cszModel.Find(_T("Pro90")) != -1)
   {
      minValue = cdREMOTE_SET_TV3_8sec;
      maxValue = cdREMOTE_SET_TV3_1000;
   }
   else if (cszModel == _T("PowerShot G2"))
   {
      minValue = cdREMOTE_SET_TV3_15sec;
      maxValue = cdREMOTE_SET_TV3_1000;
   }
   else if (cszModel == _T("PowerShot S30") ||
      cszModel == _T("PowerShot S40") ||
      cszModel.Find(_T("S200")) != -1 ||
      cszModel.Find(_T("S300a")) != -1 ||
      cszModel.Find(_T("IXUS v2")) != -1 ||
      cszModel.Find(_T("330")) != -1 ||
      cszModel.Find(_T("200a")) != -1 ||
      cszModel.Find(_T("300a")) != -1)
   {
      minValue = cdREMOTE_SET_TV3_15sec;
      maxValue = cdREMOTE_SET_TV3_1000;
      additionalValue = cdREMOTE_SET_TV2_1500;
   }
   else if (cszModel.Find(_T("A30")) != -1 ||
      cszModel.Find(_T("A40")) != -1)
   {
      minValue = cdREMOTE_SET_TV2_15sec;
      maxValue = cdREMOTE_SET_TV3_1000;
      additionalValue = cdREMOTE_SET_TV2_1500;
   }

   if (minValue == 0 || maxValue == 0)
      return; // cannot be set

   for (cdRemoteSetTv tv = minValue; tv <= maxValue;)
   {
      Variant val;
      val.Set(tv);
      val.SetType(Variant::typeUInt16);
      vecValues.push_back(val);

      unsigned int remainder = tv & 7;
      if (remainder == 0 || remainder == 5)
         tv += 3;
      else
         tv += 2;
   }

   if (additionalValue != 0)
   {
      Variant val;
      val.Set<cdRemoteSetTv>(additionalValue);
      val.SetType(Variant::typeUInt16);
      vecValues.push_back(val);
   }
}

unsigned int ImagePropertyAccess::MapToPropertyID(T_enImagePropertyType enPropertyType)
{
   // map the type values using macro, so that they don't clash with cdRelCamSettingID
   return TYPE_TO_PROP_ID(static_cast<unsigned int>(enPropertyType));
}

LPCTSTR ImagePropertyAccess::NameFromId(unsigned int propId)
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
   case cdREL_SET_AV_OPEN: pszName = _T("Open aperture Av value"); break;
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
   case cdREL_SET_ISO_SPEED_RATINGS:
      cszText = FormatIsoValue(value);
      break;

   case TYPE_TO_PROP_ID(propTv):
      cszText = FormatShutterSpeedValue(value);
      break;

   case cdREL_SET_FLASH_COMP:
   case cdREL_SET_AEB_EXPOSURE_COMP:
   case TYPE_TO_PROP_ID(propExposureCompensation):
   case TYPE_TO_PROP_ID(propFlashExposureComp):
      cszText = FormatCompensationValue(value, false);
      break;

   case TYPE_TO_PROP_ID(propAv):
   case cdREL_SET_AV_OPEN:
   case cdREL_SET_AV_MAX:
   case cdREL_SET_AV_OPEN_APEX:
   case cdREL_SET_AV_MAX_APEX:
      cszText = FormatApexValue(value);
      break;

   case cdREL_SET_KELVIN_VALUE:
      cszText.Format(_T("%u K"), value.Get<cdUInt16>());
      break;

   case cdREL_SET_SELF_TIMER:
      cszText.Format(_T("%u s"), value.Get<cdUInt16>());
      break;

   case cdREL_SET_FOCUS_POINT:
      cszText = FormatFocusPoint(value);
      break;

   case cdREL_SET_PARAMETER_SET:
      cszText = FormatParameterSet(value);
      break;

   case TYPE_TO_PROP_ID(propImageFormat):
      cszText = FormatImageFormatValue(value);
      break;

      // values simply formatted as numeric
   case TYPE_TO_PROP_ID(propCurrentZoomPos):
   case TYPE_TO_PROP_ID(propMaxZoomPos):
   case TYPE_TO_PROP_ID(propAvailableShots):
      cszText.Format(_T("%u"), value.Get<unsigned int>());
      break;

   case cdREL_SET_EZOOM_SIZE:
   case cdREL_SET_EZOOM_SIZE_TELE:
   case cdREL_SET_FOCAL_LENGTH:
   case cdREL_SET_FOCAL_LENGTH_TELE:
   case cdREL_SET_FOCAL_LENGTH_WIDE:
   case cdREL_SET_FOCAL_LENGTH_DENOMINATOR:
      cszText.Format(_T("%u"), value.Get<unsigned short>());
      break;

   default:
      if (!FormatValueById(g_aPropIdDisplayInfo, sizeof(g_aPropIdDisplayInfo) / sizeof(*g_aPropIdDisplayInfo),
         propId, value, cszText))
         cszText = value.ToString();
      break;
   }

   return cszText;
}

LPCTSTR ImagePropertyAccess::FormatFocusPoint(const Variant& value)
{
   cdUInt16 focusPoint = 0xFFFF;
   try
   {
      focusPoint = value.Get<cdUInt16>();
   }
   catch (...)
   {
      LOG_TRACE(_T("unknown exception during FormatFocusPoint() call\n"));
   }

   if ((focusPoint & 0xF000) == cdREL_VAL_FOCUS_POINT_NA)
      return _T("Invalid");
   else if ((focusPoint & 0xF000) == cdREL_VAL_FOCUS_POINT_CENTER_ONLY)
      return _T("Center only");
   else if ((focusPoint & 0xF000) == cdREL_VAL_FOCUS_POINT_3_POINTS)
      return
         (focusPoint & 0x0FFF) == cdREL_VAL_FOCUS_POINT_MF ? _T("Manual") :
         (focusPoint & 0x0FFF) == cdREL_VAL_FOCUS_POINT_AUTO ? _T("Auto") :
         (focusPoint & 0x0FFF) == cdREL_VAL_FOCUS_POINT_RIGHT ? _T("Right") :
         (focusPoint & 0x0FFF) == cdREL_VAL_FOCUS_POINT_CENTER ? _T("Center") :
         (focusPoint & 0x0FFF) == cdREL_VAL_FOCUS_POINT_LEFT ? _T("Left") : _T("???");

   return _T("???");
}

CString ImagePropertyAccess::FormatParameterSet(const Variant& value)
{
   cdUInt16 paramSet = value.Get<cdUInt16>();
   if (paramSet == 0xffff)
      return _T("Invalid");

   CString cszText;

   cdUInt16 setNum = paramSet & 15;
   if (setNum == 0)
      cszText = _T("Standard set");
   else if (setNum < 4)
      cszText.Format(_T("Set %u"), setNum);
   else
      cszText = _T("Invalid set number");

   cszText.AppendFormat(_T("; selectable: {%i,%i,%i,%i}"),
      (paramSet & (1 << 4)) != 0 ? 1 : 0,
      (paramSet & (1 << 5)) != 0 ? 1 : 0,
      (paramSet & (1 << 6)) != 0 ? 1 : 0,
      (paramSet & (1 << 7)) != 0 ? 1 : 0);

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
   case cdCOMP_QUALITY_RAW: // same as cdCOMP_QUALITY_LOSSLESS
      cszText = _T("Raw"); break;
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
