//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PsrecPropertyAccess.cpp PS-ReC - device/image property access
//

// includes
#include "stdafx.h"
#include "PsrecPropertyAccess.hpp"
#include "CommonPropertyAccess.hpp"

using namespace PSREC;

/// all property descriptions for PSREC
static PropIdDisplayInfo g_aPropIdDisplayInfo[] =
{
   {
      prPTP_DEV_PROP_BUZZER,
      {
         { 0x0000, _T("On") },
         { 0x0001, _T("Off") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_BATTERY_KIND,
      {
         { 0x0000, _T("Unknown") },
         { 0x0001, _T("AC power supply") },
         { 0x0002, _T("Lithium ion battery") },
         { 0x0003, _T("Nickel hydride battery") },
         { 0x0004, _T("Nickel cadmium battery") },
         { 0x0005, _T("Alkaline manganese battery") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_BATTERY_STATUS,
      {
         { 0x00000000, _T("Not defined") },
         { 0x00000001, _T("Normal") },
         { 0x00000002, _T("Warning Level 1") },
         { 0x00000003, _T("Emergency") },
         { 0x00000004, _T("Warning Level 0") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_COMP_QUALITY,
      {
         { 0x00, _T("Not defined") },
         { 0x01, _T("Economy") },
         { 0x02, _T("Normal") },
         { 0x03, _T("Fine") },
         { 0x04, _T("Lossless") },
         { 0x05, _T("SuperFine") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_FULLVIEW_FILE_FORMAT,
      {
         { 0x00, _T("Not defined") },
         { 0x01, _T("JPEG") },
         { 0x02, _T("CRW") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_IMAGE_SIZE,
      {
         { 0x00, _T("Large") },
         { 0x01, _T("Medium 1") },
         { 0x02, _T("Small") },
         { 0x03, _T("Medium 2") },
         { 0x07, _T("Medium 3") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_STROBE_SETTING,
      {
         { 0x00, _T("Off") },
         { 0x01, _T("Auto") },
         { 0x02, _T("On") },
         { 0x03, _T("Red Eye Suppression") },
         { 0x04, _T("Low-speed Synchronization") },
         { 0x05, _T("Auto + Red Eye Suppression") },
         { 0x06, _T("On + Red Eye Suppression") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_BEEP,
      {
         { 0x00, _T("Off") },
         { 0x01, _T("On") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_EXPOSURE_MODE,
      {
         { 0x00, _T("Auto") },
         { 0x01, _T("P") },
         { 0x02, _T("Tv") },
         { 0x03, _T("Av") },
         { 0x04, _T("M") },
         { 0x05, _T("A Dep") },
         { 0x06, _T("M Dep") },
         { 0x07, _T("Bulb") },
         { 0x80, _T("Camera M") },
         { 0x81, _T("My Color") },
         { 0x82, _T("Portrait") },
         { 0x83, _T("Landscape") },
         { 0x84, _T("Night Scene") },
         { 0x85, _T("Forest") },
         { 0x86, _T("Snow") },
         { 0x87, _T("Beach") },
         { 0x88, _T("Fireworks") },
         { 0x89, _T("Party") },
         { 0x8A, _T("Nightsnap") },
         { 0x8B, _T("Stitch") },
         { 0x8C, _T("Movie") },
         { 0x8D, _T("Custom") },
         { 0x8E, _T("Interval") },
         { 0x8F, _T("DitigalMacro") },
         { 0x90, _T("Long Shutter") },
         { 0x91, _T("Underwater") },
         { 0x92, _T("Kids and Pets") },
         { 0x93, _T("Fast Shutter") },
         { 0x94, _T("Slow Shutter") },
         { 0x95, _T("Custom1") },
         { 0x96, _T("Custom2") },
         { 0x97, _T("Neutral") },
         { 0x98, _T("Gray") },
         { 0x99, _T("Sepia") },
         { 0x9A, _T("Vivid") },
         { 0x9B, _T("Sports") },
         { 0x9C, _T("Macro") },
         { 0x9D, _T("Supermacro") },
         { 0x9E, _T("Pan Focus") },
         { 0x9F, _T("B/W") },
         { 0xA0, _T("Flash Inhibit") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_IMAGE_MODE,
      {
         { 0x00, _T("Auto") },
         { 0x01, _T("Manual") },
         { 0x02, _T("Distant View") },
         { 0x03, _T("High-speed Shutter") },
         { 0x04, _T("Low-speed Shutter") },
         { 0x05, _T("Night View") },
         { 0x06, _T("Grayscale") },
         { 0x07, _T("Sepia") },
         { 0x08, _T("Portrait") },
         { 0x09, _T("Sports") },
         { 0x0A, _T("Macro") },
         { 0x0B, _T("Monochrome") },
         { 0x0C, _T("Pan Focus") },
         { 0x0D, _T("Neutral") },
         { 0x0E, _T("Soft") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_DRIVE_MODE,
      {
         { 0x00, _T("Single-frame Shooting") },
         { 0x01, _T("Continuous Shooting") },
         { 0x02, _T("Timer (Single) Shooting") },
         { 0x04, _T("Continuous Low-speed Shooting") },
         { 0x05, _T("Continuous High-speed Shooting") },
         { 0xFF, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_EZOOM,
      {
         { 0x00, _T("Off") },
         { 0x01, _T("x2") },
         { 0x02, _T("x4") },
         { 0x03, _T("Smooth") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_ML_WEI_MODE,
      {
         { 0x00, _T("Center-weighted Metering") },
         { 0x01, _T("Spot Metering") },
         { 0x02, _T("Average Metering") },
         { 0x03, _T("Evaluative Metering") },
         { 0x04, _T("Partial Metering") },
         { 0x05, _T("Center-weighted Average Metering") },
         { 0x06, _T("Spot Metering Interlocked with AF Frame") },
         { 0x07, _T("Multi-Spot Metering") },
         { 0xFF, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_AF_DISTANCE,
      {
         { 0x00, _T("Manual") },
         { 0x01, _T("Auto") },
         { 0x02, _T("Unknown") },
         { 0x03, _T("Zone Focus (Close-up)") },
         { 0x04, _T("Zone Focus (Very close)") },
         { 0x05, _T("Zone Focus (Close)") },
         { 0x06, _T("Zone Focus (Medium)") },
         { 0x07, _T("Zone Focus (Far)") },
         { 0x08, _T("Zone Focus (Reserved 1)") },
         { 0x09, _T("Zone Focus (Reserved 2)") },
         { 0x0a, _T("Zone Focus (Reserved 3)") },
         { 0x0b, _T("Zone Focus (Reserved 4)") },
         { 0xff, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_FOCUS_POINT_SETTING,
      {
         { 0x0000, _T("Invalid") },
         { 0x1000, _T("Focusing point on center only, Manual") },
         { 0x1001, _T("Focusing point on center only, Auto") },
         { 0x3000, _T("Multiple focusing points, Manual") },
         { 0x3001, _T("Multiple focusing points, Auto") },
         { 0x3002, _T("Multiple focusing points (Right)") },
         { 0x3003, _T("Multiple focusing points (Center)") },
         { 0x3004, _T("Multiple focusing points (Left)") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_WB_SETTING,
      {
         { 0, _T("Auto") },
         { 1, _T("Daylight") },
         { 2, _T("Cloudy") },
         { 3, _T("Tungsten") },
         { 4, _T("Fluorescent") },
         { 5, _T("Flash") }, // gathered from G9
         { 6, _T("Preset") },
         { 7, _T("Fluorescent H") },
         { 9, _T("Color Temperature") },
         { 10, _T("Custom White Balance PC-1") },
         { 11, _T("Custom White Balance PC-2") },
         { 12, _T("Custom White Balance PC-3") },
         //{ 13, _T("Missing number") },
         { 14, _T("Fluorescent H") },
         { 15, _T("Custom 1") }, // gathered from G9
         { 16, _T("Custom 2") }, // gathered from G9
         { 0xFF, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_SLOW_SHUTTER_SETTING,
      {
         { 0x00, _T("Off") },
         { 0x01, _T("Night View") },
         { 0x02, _T("On") },
         { 0x03, _T("Not available.") },
         { 0xFF, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_AF_MODE,
      {
         { 0x00, _T("Single Shot") },
         { 0x01, _T("AI Servo") },
         { 0x02, _T("AI Focus") },
         { 0x03, _T("Manual") },
         { 0x04, _T("Continuous") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_IMAGE_STABILIZATION,
      {
         { 0, _T("Off") },
         { 1, _T("On") },
         { 0xFF, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_CONTRAST,
      {
         { 0xfffffffe, _T("Low 2") }, // -2
         { 0xffffffff, _T("Low") }, // -1
         { 0x00000000, _T("Standard") },
         { 0x00000001, _T("High") },
         { 0x00000002, _T("High 2") },
         { 0, nullptr }
      }
   },

#if 0


   {
      xxx,
      {
         { 0x0000, _T("") },
         { 0, nullptr }
      }
   },


prPTP_DEV_PROP_COLOR_GAIN
prPTP_DEV_PROP_SHARPNESS
prPTP_DEV_PROP_SENSITIVITY
prPTP_DEV_PROP_PARAMETER_SET
//prPTP_DEV_PROP_FOCAL_LENGTH
//prPTP_DEV_PROP_FOCAL_LENGTH_TELE
//prPTP_DEV_PROP_FOCAL_LENGTH_WIDE
//prPTP_DEV_PROP_FOCAL_LENGTH_DENOMI
#endif

   {
      prPTP_DEV_PROP_CAPTURE_TRANSFER_MODE,
      {
         // TODO G9 lists 1, 2, 3, 8..15
         { 0x0000, _T("Not defined") },
         { 0x0002, _T("Transfer Entire Image to PC") },
         { 0x0004, _T("Save Thumbnail Image to Device") },
         { 0x0008, _T("Save Entire Image to Device") },
         { 0, nullptr }
      }
   },

#if 0
prPTP_DEV_PROP_ZOOM_POS
//prPTP_DEV_PROP_SUPPORTED_SIZE
//prPTP_DEV_PROP_SUPPORTED_THUMB_SIZE
//prPTP_DEV_PROP_CAMERA_TIME
#endif
   {
      prPTP_DEV_PROP_CAMERA_OUTPUT,
      {
         { 0x0000, _T("Not defined") },
         { 0x0001, _T("LCD") },
         { 0x0002, _T("Video OUT") },
         { 0x0003, _T("Off") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_DISP_AV,
      {
         { 0x0000, _T("1/3 Level") },
         { 0x0001, _T("1/10th Av value") },
         { 0, nullptr }
      }
   },
#if 0
//
//prPTP_DEV_PROP_EZOOM_SIZE
prPTP_DEV_PROP_ML_SPOT_POS
#endif
   {
      prPTP_DEV_PROP_DISP_AV_MAX,
      {
         { 0x0000, _T("1/3 Level") },
         { 0x0001, _T("1/10th Av value") },
         { 0, nullptr }
      }
   },
#if 0

//prPTP_DEV_PROP_EZOOM_START_POS
//prPTP_DEV_PROP_FOCAL_LENGTH_OF_TELE
//prPTP_DEV_PROP_EZOOM_SIZE_OF_TELE
prPTP_DEV_PROP_PHOTO_EFFECT
#endif
   {
      prPTP_DEV_PROP_AF_LIGHT,
      {
         { 0x0001, _T("Off") },
         { 0x0000, _T("On") },
         { 0, nullptr }
      }
   },

#if 0
//prPTP_DEV_PROP_FLASH_QUANTITY
#endif
   {
      prPTP_DEV_PROP_ROTATION_ANGLE,
      {
         { 0x0000, _T("0 Degree") },
         { 0x0001, _T("90 Degrees") },
         { 0x0002, _T("180 Degrees") },
         { 0x0003, _T("270 Degrees") },
         { 0xffff, _T("None") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_ROTATION_SENCE,
      {
         { 0x0000, _T("Enable") },
         { 0x0001, _T("Disable") },
         { 0xffff, _T("None") },
         { 0, nullptr }
      }
   },

#if 0
prPTP_DEV_PROP_IMEGE_FILE_SIZE
#endif
};

Variant PropertyAccess::GetImageFormatProperty() const
{
   std::vector<prUInt8> vecRawValues;

   DevicePropDesc compQuality(m_hCamera, prPTP_DEV_PROP_COMP_QUALITY, false);
   DevicePropDesc imageSize(m_hCamera, prPTP_DEV_PROP_IMAGE_SIZE, false);

   vecRawValues.push_back(compQuality.m_varCurrentValue.Get<prUInt8>());
   vecRawValues.push_back(imageSize.m_varCurrentValue.Get<prUInt8>());

   Variant value;
   value.SetArray(vecRawValues);
   value.SetType(Variant::typeUInt8);

   return value;
}

CString PropertyAccess::DisplayTextFromIdAndValue(prUInt16 propertyId, Variant value)
{
   switch(propertyId)
   {
   case prPTP_DEV_PROP_SELFTIMER:
      {
         unsigned int uiValue = value.Get<prUInt16>();
         switch(uiValue)
         {
         case 0: return _T("Not used");
         case 0xffff: return _T("Invalid");
         default:
            {
               CString cszText;
               cszText.Format(uiValue % 10 == 0 ? _T("%us") : _T("%u.%us"), uiValue / 10, uiValue % 10);
               return cszText;
            }
         }
      }
      break;

   case prPTP_DEV_PROP_AV:
   case prPTP_DEV_PROP_AV_OPEN:
   case prPTP_DEV_PROP_AV_MAX:
   case prPTP_DEV_PROP_AV_MAX_APEX:
   case prPTP_DEV_PROP_AV_OPEN_APEX:
      return FormatApexValue(value);

   case prPTP_DEV_PROP_TV:
      return FormatShutterSpeedValue(value);

   case prPTP_DEV_PROP_ISO:
      return FormatIsoValue(value);

   case prPTP_DEV_PROP_EXPOSURE_COMP:
   case prPTP_DEV_PROP_FLASH_COMP:
   case prPTP_DEV_PROP_AEB_EXPOSURE_COMP:
      return FormatCompensationValue(value, false);

   case prPTP_DEV_PROP_OWNER_NAME:
      {
         std::vector<unsigned char> vecOwnerName = value.GetArray<unsigned char>();
         vecOwnerName.push_back(0);

         CString cszText(vecOwnerName.data());
         return cszText;
      }
      break;

   case PSREC_PROP_IMAGE_FORMAT:
      return FormatImageFormatValue(value);

   case PSREC_PROP_AVAILABLE_SHOTS:
      {
         CString cszText;
         cszText.Format(_T("%u"), value.Get<unsigned int>());
         return cszText;
      }
      break;

   default:
      {
         CString cszText;
         if (FormatValueById(g_aPropIdDisplayInfo, sizeof(g_aPropIdDisplayInfo)/sizeof(*g_aPropIdDisplayInfo),
               propertyId, value, cszText))
            return cszText;
      }
      break;
   }

   return value.ToString();
}

CString PropertyAccess::FormatImageFormatValue(const Variant& value)
{
   ATLASSERT(value.IsArray() == true);
   ATLASSERT(value.Type() == Variant::typeUInt8);

   std::vector<prUInt8> vecValues = value.GetArray<prUInt8>();
   ATLASSERT(vecValues.size() == 2);

   CString cszText1;
   {
      prUInt8 compQuality = vecValues[0];
      Variant vCompQuality; vCompQuality.Set(compQuality); vCompQuality.SetType(Variant::typeUInt8);

      FormatValueById(g_aPropIdDisplayInfo, sizeof(g_aPropIdDisplayInfo) / sizeof(*g_aPropIdDisplayInfo),
         prPTP_DEV_PROP_COMP_QUALITY, vCompQuality, cszText1);
   }

   CString cszText2;
   {
      prUInt8 imageSize = static_cast<prUInt8>(vecValues[1]);
      Variant vImageSize; vImageSize.Set(imageSize); vImageSize.SetType(Variant::typeUInt8);

      FormatValueById(g_aPropIdDisplayInfo, sizeof(g_aPropIdDisplayInfo) / sizeof(*g_aPropIdDisplayInfo),
         prPTP_DEV_PROP_IMAGE_SIZE, vImageSize, cszText2);
   }

   return cszText1 + _T(" ") + cszText2;
}
