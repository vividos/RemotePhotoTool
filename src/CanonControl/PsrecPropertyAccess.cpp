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

/// this is not really a property supported by PSREC; see entries in g_aPropIdDisplayInfo below
const prUInt16 prPTP_DEV_PROP_SUPPORTED_SIZE_META_IMAGEFORMAT = 0xF000;

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
         { 0x03, _T("Not available") },
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

   {
      prPTP_DEV_PROP_COLOR_GAIN,
      {
         { 0xfffffffe, _T("Low 2") }, // -2
         { 0xffffffff, _T("Low") }, // -1
         { 0x00000000, _T("Standard") },
         { 0x00000001, _T("High") },
         { 0x00000002, _T("High 2") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_SHARPNESS,
      {
         { 0xfffffffe, _T("Low 2") }, // -2
         { 0xffffffff, _T("Low") }, // -1
         { 0x00000000, _T("Standard") },
         { 0x00000001, _T("High") },
         { 0x00000002, _T("High 2") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_SENSITIVITY,
      {
         { 0, _T("Standard") },
         { 1, _T("Upper 1") },
         { 2, _T("Upper 2") },
         { 0xFF, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_PARAMETER_SET,
      {
         { 0x0008, _T("Standard Development Parameters") },
         { 0x0010, _T("Development Parameters 1") },
         { 0x0020, _T("Development Parameters 2") },
         { 0x0040, _T("Development Parameters 3") },
         { 0xffff, _T("Invalid") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_CAPTURE_TRANSFER_MODE,
      {
         // G9 lists 1, 2, 3, 8..15
         { 0x0000, _T("Not defined") },
         { 0x0001, _T("Transfer Thumbnail Image to PC") },
         { 0x0002, _T("Transfer Full Image to PC") },
         { 0x0003, _T("Transfer Thumbnail + Full Image to PC") },
         { 0x0004, _T("Save Thumbnail Image to Device") },
         { 0x0008, _T("Save Full Image to Device") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_CAMERA_OUTPUT,
      {
         { 0x0000, _T("Not defined") },
         { 0x0001, _T("LCD") },
         { 0x0002, _T("Video Out") },
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

   {
      prPTP_DEV_PROP_ML_SPOT_POS,
      {
         { 0, _T("Center") }, // MlSpotPosCenter
         { 1, _T("AutoFocus") }, // MlSpotPosAfLink
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_DISP_AV_MAX,
      {
         { 0x0000, _T("1/3 Level") },
         { 0x0001, _T("1/10th Av value") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_PHOTO_EFFECT,
      {
         { 0x0000, _T("Off") },
         { 0x0001, _T("Vivid") },
         { 0x0002, _T("Neutral") },
         { 0x0003, _T("Soft") },
         { 0x0004, _T("Sepia") },
         { 0x0005, _T("Monochrome") },
         { 0, nullptr }
      }
   },

   {
      prPTP_DEV_PROP_AF_LIGHT,
      {
         { 0x0001, _T("Off") },
         { 0x0000, _T("On") },
         { 0, nullptr }
      }
   },

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

   {
      // this is not really a property supported by PSREC, but the image formats for
      // a subfield of the data returned by the prPTP_DEV_PROP_SUPPORTED_SIZE property.
      // See PSReCSDK110eUserGuide.doc for details.
      prPTP_DEV_PROP_SUPPORTED_SIZE_META_IMAGEFORMAT,
      {
         { 0x00000000, _T("Not defined") },
         { 0x00000001, _T("JPEG SuperFine Large") },
         { 0x00000002, _T("JPEG SuperFine Middle1") },
         { 0x00000003, _T("JPEG SuperFine Middle2") },
         { 0x00000004, _T("JPEG SuperFine Middle") },
         { 0x00000005, _T("JPEG SuperFine Small") },
         { 0x00000006, _T("JPEG Fine Large") },
         { 0x00000007, _T("JPEG Fine Middle1") },
         { 0x00000008, _T("JPEG Fine Middle2") },
         { 0x00000009, _T("JPEG Fine Middle") },
         { 0x0000000A, _T("JPEG Fine Small") },
         { 0x0000000B, _T("JPEG Normal Large") },
         { 0x0000000C, _T("JPEG Normal Middle1") },
         { 0x0000000D, _T("JPEG Normal Middle2") },
         { 0x0000000E, _T("JPEG Normal Middle") },
         { 0x0000000F, _T("JPEG Normal Small") },
         { 0x00000010, _T("CRW Large") },
         { 0x00000011, _T("JPEG SuperFine Middle3") },
         { 0x00000012, _T("JPEG Fine Middle3") },
         { 0x00000013, _T("JPEG Normal Middle3") },
         { 0x00000014, _T("CR2 Large") },
         { 0, nullptr }
      }
   }
};

Variant DeviceValueParser::ReadValue(prUInt16 uiDataType)
{
   Variant v;

   switch (uiDataType)
   {
   case 0x0001: v.Set<prInt8>(ReadUint8());     v.SetType(Variant::typeInt8); break;
   case 0x0002: v.Set<prUInt8>(ReadUint8());    v.SetType(Variant::typeUInt8); break;
   case 0x0003: v.Set<prInt16>(ReadUint16());   v.SetType(Variant::typeInt16); break;
   case 0x0004: v.Set<prUInt16>(ReadUint16());  v.SetType(Variant::typeUInt16); break;
   case 0x0005: v.Set<prInt32>(ReadUint32());   v.SetType(Variant::typeInt32); break;
   case 0x0006: v.Set<unsigned int>(ReadUint32());  v.SetType(Variant::typeUInt32); break; // prUInt32
   case 0x4002: v.SetArray<prUInt8>(ReadUint8Array()); v.SetType(Variant::typeUInt8); break; // prUInt8 array
   case 0x4004: v.SetArray<prUInt16>(ReadUint16Array()); v.SetType(Variant::typeUInt16); break; // prUInt16 array
   case 0x4006: v.SetArray<unsigned int>(ReadUint32Array()); v.SetType(Variant::typeUInt32); break; // prUInt32 array
   case 0xFFFF: // string
      v.Set<CString>(ReadString());
      v.SetType(Variant::typeString);
      break;

   default:
      ATLASSERT(false);
      break;
   }

   return v;
}

DevicePropDesc::DevicePropDesc(prHandle hCamera, prUInt16 propId, bool bParseValues)
:m_uiDevicePropCode(0),
m_uiDataType(0),
m_uiGetSet(0),
m_uiFormat(0),
m_bParseValues(bParseValues)
{
   // determine size
   prUInt32 uiSize = 0;
   prResponse err = PR_GetDevicePropDesc(hCamera, propId, &uiSize, nullptr);

   if ((err & prERROR_ERRORID_MASK) != prINSUFFICIENT_BUFFER)
   {
      LOG_TRACE(_T("PR_GetDevicePropDesc(handle = %08x, propid = %04x, 0, nullptr) returned %08x, buffer size %u\n"),
         hCamera, propId, err, uiSize);
      CheckError(_T("PR_GetDevicePropDesc"), err, __FILE__, __LINE__);
   }

   if (uiSize == 0)
      return;

   std::vector<BYTE>& vecBuffer = GetBuffer();
   vecBuffer.resize(uiSize);

   prVoid* pDevicePropDesc = reinterpret_cast<prVoid*>(&vecBuffer[0]);

   // may return prINVALID_FN_CALL, prINVALID_HANDLE, prINVALID_PARAMETER, prMEM_ALLOC_FAILED, prINSUFFICIENT_BUFFER or @ERR
   err = PR_GetDevicePropDesc(hCamera, propId, &uiSize, pDevicePropDesc);
   LOG_TRACE(_T("PR_GetDevicePropDesc(handle = %08x, propId = %04x, size = %u, buffer) returned %08x\n"),
      hCamera, propId, uiSize, err);
   CheckError(_T("PR_GetDevicePropDesc"), err, __FILE__, __LINE__);

   Parse();
}

void DevicePropDesc::Parse()
{
   m_uiDevicePropCode = ReadUint16();
   m_uiDataType = ReadUint16();
   m_uiGetSet = ReadUint8();

   // factory default
   m_varDefaultValue = ReadValue(m_uiDataType);

   // current value
   m_varCurrentValue = ReadValue(m_uiDataType);

   if (!m_bParseValues)
      return; // stop parsing

   // format flag
   m_uiFormat = ReadUint8();

   // settable values
   if (m_uiFormat == 0)
      return;

   // read form data
   if (m_uiFormat == 1)
   {
      // range form
      Variant varMinValue = ReadValue(m_uiDataType);
      Variant varMaxValue = ReadValue(m_uiDataType);
      Variant varStepSize = ReadValue(m_uiDataType);

      if (varMinValue.Type() == Variant::typeUInt16)
      {
         prUInt16 uiMin = varMinValue.Get<prUInt16>();
         prUInt16 uiMax = varMaxValue.Get<prUInt16>();
         prUInt16 uiStep = varStepSize.Get<prUInt16>();

         for (prUInt16 ui = uiMin; ui<uiMax; ui += uiStep)
         {
            Variant val;
            val.Set<prUInt16>(ui);
            val.SetType(Variant::typeUInt16);

            m_vecAllValues.push_back(val);
         }
      }
      else
      {
         ATLASSERT(false); // not implemented

         m_vecAllValues.push_back(varMinValue);
         m_vecAllValues.push_back(varMaxValue);
      }
   }
   else
   if (m_uiFormat == 2)
   {
      // enum form
      prUInt16 uiCount = ReadUint16();
      for (prUInt16 ui = 0; ui<uiCount; ui++)
      {
         Variant varValue = ReadValue(m_uiDataType);
         m_vecAllValues.push_back(varValue);
      }
   }
   else
      throw Exception(_T("unknown FORM format in device property desc data"), __FILE__, __LINE__);
}

void DevicePropDesc::CreateValueData(Variant v, std::vector<BYTE>& vecValueData)
{
   switch (v.Type())
   {
   case Variant::typeInt8:
      vecValueData.push_back(static_cast<BYTE>(v.Get<prInt8>()));
      break;

   case Variant::typeUInt8:
      vecValueData.push_back(v.Get<prUInt8>());
      break;

   case Variant::typeInt16:
   {
      prUInt16 val = static_cast<prUInt16>(v.Get<prInt16>());
      vecValueData.push_back(static_cast<BYTE>(val & 0xff));
      vecValueData.push_back(static_cast<BYTE>((val >> 8) & 0xff));
   }
      break;

   case Variant::typeUInt16:
   {
      prUInt16 val = v.Get<prUInt16>();
      vecValueData.push_back(static_cast<BYTE>(val & 0xff));
      vecValueData.push_back(static_cast<BYTE>((val >> 8) & 0xff));
   }
      break;

   case Variant::typeInt32:
   {
      prUInt32 val = static_cast<prUInt32>(v.Get<prInt32>());
      vecValueData.push_back(static_cast<BYTE>(val & 0xff));
      vecValueData.push_back(static_cast<BYTE>((val >> 8) & 0xff));
      vecValueData.push_back(static_cast<BYTE>((val >> 16) & 0xff));
      vecValueData.push_back(static_cast<BYTE>((val >> 24) & 0xff));
   }
      break;

   case Variant::typeUInt32:
   {
      prUInt32 val = v.Get<prUInt32>();
      vecValueData.push_back(static_cast<BYTE>(val & 0xff));
      vecValueData.push_back(static_cast<BYTE>((val >> 8) & 0xff));
      vecValueData.push_back(static_cast<BYTE>((val >> 16) & 0xff));
      vecValueData.push_back(static_cast<BYTE>((val >> 24) & 0xff));
   }
      break;

   default:
      ATLASSERT(false);
      break;
   }
}

Variant PropertyAccess::Get(prUInt16 propId) const
{
   // special case: image format
   if (propId == PSREC_PROP_IMAGE_FORMAT)
      return GetImageFormatProperty();

   // must not be "available shots"; already handled in GetImageProperty()
   ATLASSERT(propId != PSREC_PROP_AVAILABLE_SHOTS);

   // Note: we could use PR_GetDevicePropValue here, but we have to call
   // PR_GetDevicePropDesc anyway, to find out data type; so we just use
   // the current value field then.
   DevicePropDesc desc(m_hCamera, propId, false);
   return desc.m_varCurrentValue;
}
void PropertyAccess::Set(prUInt16 propId, Variant v)
{
   // first, get type and size of property
   DevicePropDesc desc(m_hCamera, propId, false);

   if (!desc.IsSetAllowed())
      throw Exception(_T("PSREC::PropertyAccess::Set: property is read-only"), __FILE__, __LINE__);

   std::vector<BYTE> vecValueData;
   desc.CreateValueData(v, vecValueData);

   prUInt32 uiSize = static_cast<prUInt32>(vecValueData.size());
   prVoid* pDeviceProperty = &vecValueData[0];

   // may return prINVALID_FN_CALL, prINVALID_HANDLE, prINVALID_PARAMETER, prMEM_ALLOC_FAILED or @ERR
   prResponse err = PR_SetDevicePropValue(m_hCamera, propId, uiSize, pDeviceProperty);

   LOG_TRACE(_T("PR_SetDevicePropValue(handle = %08x, propid = %04x, size = %u, buffer) returned %08x\n"),
      m_hCamera, propId, uiSize, err);
   CheckError(_T("PR_SetDevicePropValue"), err, __FILE__, __LINE__);
}

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

LPCTSTR PropertyAccess::NameFromId(prUInt16 propertyId) throw()
{
   LPCTSTR pszName = _T("???");
   switch (propertyId)
   {
   case prPTP_DEV_PROP_BUZZER: pszName = _T("Device buzzer on/off"); break;
   case prPTP_DEV_PROP_BATTERY_KIND: pszName = _T("Battery type"); break;
   case prPTP_DEV_PROP_BATTERY_STATUS: pszName = _T("Battery status"); break;
   case prPTP_DEV_PROP_COMP_QUALITY: pszName = _T("Image quality"); break;
   case prPTP_DEV_PROP_FULLVIEW_FILE_FORMAT: pszName = _T("Image type"); break;
   case prPTP_DEV_PROP_IMAGE_SIZE: pszName = _T("Image size"); break;
   case prPTP_DEV_PROP_SELFTIMER: pszName = _T("Self-timer"); break;
   case prPTP_DEV_PROP_STROBE_SETTING: pszName = _T("Strobe setting"); break;
   case prPTP_DEV_PROP_BEEP: pszName = _T("Buzzer setting"); break;
   case prPTP_DEV_PROP_EXPOSURE_MODE: pszName = _T("Exposure mode setting"); break;
   case prPTP_DEV_PROP_IMAGE_MODE: pszName = _T("Image mode setting"); break;
   case prPTP_DEV_PROP_DRIVE_MODE: pszName = _T("Drive mode"); break;
   case prPTP_DEV_PROP_EZOOM: pszName = _T("Electonic zoom setting"); break;
   case prPTP_DEV_PROP_ML_WEI_MODE: pszName = _T("Metering method"); break;
   case prPTP_DEV_PROP_AF_DISTANCE: pszName = _T("Search range in AF mode"); break;
   case prPTP_DEV_PROP_FOCUS_POINT_SETTING: pszName = _T("Selection mode for focusing point"); break;
   case prPTP_DEV_PROP_WB_SETTING: pszName = _T("White balance setting"); break;
   case prPTP_DEV_PROP_SLOW_SHUTTER_SETTING: pszName = _T("Slow Shutter setting"); break;
   case prPTP_DEV_PROP_AF_MODE: pszName = _T("Auto focus mode setting"); break;
   case prPTP_DEV_PROP_IMAGE_STABILIZATION: pszName = _T("Image stabilization processing"); break;
   case prPTP_DEV_PROP_CONTRAST: pszName = _T("Contrast"); break;
   case prPTP_DEV_PROP_COLOR_GAIN: pszName = _T("Color Compensation"); break;
   case prPTP_DEV_PROP_SHARPNESS: pszName = _T("Sharpness"); break;
   case prPTP_DEV_PROP_SENSITIVITY: pszName = _T("Sensitivity"); break;
   case prPTP_DEV_PROP_PARAMETER_SET: pszName = _T("Development parameter setting"); break;
   case prPTP_DEV_PROP_ISO: pszName = _T("ISO value"); break;
   case prPTP_DEV_PROP_AV: pszName = _T("Av value"); break;
   case prPTP_DEV_PROP_TV: pszName = _T("Tv value"); break;
   case prPTP_DEV_PROP_EXPOSURE_COMP: pszName = _T("Exposure compensation value"); break;
   case prPTP_DEV_PROP_FLASH_COMP: pszName = _T("Flash exposure compensation value"); break;
   case prPTP_DEV_PROP_AEB_EXPOSURE_COMP: pszName = _T("AEB exposure compensation value"); break;
   case prPTP_DEV_PROP_AV_OPEN: pszName = _T("Open aperture value"); break;
   case prPTP_DEV_PROP_AV_MAX: pszName = _T("Maximum aperture value"); break;
   case prPTP_DEV_PROP_FOCAL_LENGTH: pszName = _T("Current focal distance multiplied by FocalLengthDenominator"); break;
   case prPTP_DEV_PROP_FOCAL_LENGTH_TELE: pszName = _T("Telescopic focal length multiplied by FocalLengthDenominator"); break;
   case prPTP_DEV_PROP_FOCAL_LENGTH_WIDE: pszName = _T("Wide-angle focus length multiplied by FocalLengthDenominator"); break;
   case prPTP_DEV_PROP_FOCAL_LENGTH_DENOMI: pszName = _T("Focal length multiplier value"); break;
   case prPTP_DEV_PROP_CAPTURE_TRANSFER_MODE: pszName = _T("Image transfer mode at capture"); break;
   case prPTP_DEV_PROP_ZOOM_POS: pszName = _T("Current zoom position"); break;
   case prPTP_DEV_PROP_SUPPORTED_SIZE: pszName = _T("Image size supported by the camera"); break;
   case prPTP_DEV_PROP_SUPPORTED_THUMB_SIZE: pszName = _T("Thumbnail size supported by the camera"); break;
   case prPTP_DEV_PROP_FIRMWARE_VERSION: pszName = _T("Version of the camera device firmware"); break;
   case prPTP_DEV_PROP_CAMERA_MODEL_NAME: pszName = _T("Camera model"); break;
   case prPTP_DEV_PROP_OWNER_NAME: pszName = _T("Owner name"); break;
   case prPTP_DEV_PROP_CAMERA_TIME: pszName = _T("Camera time"); break;
   case prPTP_DEV_PROP_CAMERA_OUTPUT: pszName = _T("Destination of image signal output in the Viewfinder mode"); break;
   case prPTP_DEV_PROP_DISP_AV: pszName = _T("How to display the Av value"); break;
   case prPTP_DEV_PROP_AV_OPEN_APEX: pszName = _T("Open aperture value"); break;
   case prPTP_DEV_PROP_EZOOM_SIZE: pszName = _T("Horizontal size of image to be cut out from CCD image using electronic zoom"); break;
   case prPTP_DEV_PROP_ML_SPOT_POS: pszName = _T("Spot metering position"); break;
   case prPTP_DEV_PROP_DISP_AV_MAX: pszName = _T("How to display the maximum Av value"); break;
   case prPTP_DEV_PROP_AV_MAX_APEX: pszName = _T("Maximum Av value"); break;
   case prPTP_DEV_PROP_EZOOM_START_POS: pszName = _T("Zoom position at which the electronic zoom range starts"); break;
   case prPTP_DEV_PROP_FOCAL_LENGTH_OF_TELE: pszName = _T("Focal distance at the optical telescopic end"); break;
   case prPTP_DEV_PROP_EZOOM_SIZE_OF_TELE: pszName = _T("Horizontal size of image to be cut out from CCD image at the telescopic end of the electronic zoom range"); break;
   case prPTP_DEV_PROP_PHOTO_EFFECT: pszName = _T("Photo effect"); break;
   case prPTP_DEV_PROP_AF_LIGHT: pszName = _T("AF assist light"); break;
   case prPTP_DEV_PROP_FLASH_QUANTITY: pszName = _T("Number of flash levels that can be set in the manual mode"); break;
   case prPTP_DEV_PROP_ROTATION_ANGLE: pszName = _T("Rotation angle"); break;
   case prPTP_DEV_PROP_ROTATION_SENCE: pszName = _T("Gravity sensor on/off"); break;
   case prPTP_DEV_PROP_IMEGE_FILE_SIZE: pszName = _T("Image file size supported by the camera"); break;
   case prPTP_DEV_PROP_CAMERA_MODEL_ID: pszName = _T("Camera model ID"); break;
   case PSREC_PROP_IMAGE_FORMAT: pszName = _T("Image format"); break;
   case PSREC_PROP_AVAILABLE_SHOTS: pszName = _T("Available shots"); break;
   case PSREC_PROP_OUTPUT_DATA_SIZE: pszName = _T("Output data size"); break;
   case PSREC_PROP_INPUT_DATA_SIZE: pszName = _T("Input data size"); break;
   case PSREC_PROP_REMOTE_API_VERSION: pszName = _T("Remote API version"); break;
   case PSREC_PROP_EVENT_EMULATE_MODE: pszName = _T("Event emulate mode"); break;
   case PSREC_PROP_DPOF_VERSION: pszName = _T("DPOF version"); break;
   case PSREC_PROP_SUPPORTED_SLIDESHOW_TYPE: pszName = _T("Supported slideshow type"); break;
   case PSREC_PROP_MTP_DEVICE_NAME: pszName = _T("MTP Device name"); break;
   case PSREC_PROP_MTP_SESSION_INITIATOR: pszName = _T("MTP Session Initiator"); break;
   case PSREC_PROP_MTP_DEVICE_TYPE: pszName = _T("MTP Device type"); break;
   }

   return pszName;
}

CString PropertyAccess::DisplayTextFromIdAndValue(prUInt16 propertyId, Variant value)
{
   switch (propertyId)
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

   case prPTP_DEV_PROP_SUPPORTED_SIZE:
      return FormatPropSupportedSize(value);

   case prPTP_DEV_PROP_SUPPORTED_THUMB_SIZE:
      {
         std::vector<unsigned int> vecThumbSizeInfo = value.GetArray<unsigned int>();
         if (vecThumbSizeInfo.size() < 4)
            return _T("invalid data");

         CString cszText;
         cszText.Format(_T("Left:%u Right:%u Top:%u Bottom:%u"),
            vecThumbSizeInfo[1],
            vecThumbSizeInfo[2],
            vecThumbSizeInfo[3],
            vecThumbSizeInfo[0]);

         return cszText;
      }

   case prPTP_DEV_PROP_CAMERA_TIME:
      return FormatDateTimeEpochUTC(value);

   case prPTP_DEV_PROP_FOCAL_LENGTH:
   case prPTP_DEV_PROP_FOCAL_LENGTH_TELE:
   case prPTP_DEV_PROP_FOCAL_LENGTH_WIDE:
   case prPTP_DEV_PROP_FOCAL_LENGTH_DENOMI:
   case prPTP_DEV_PROP_FOCAL_LENGTH_OF_TELE:
      {
         unsigned int uiValue = GetUnsignedIntValue(value);
         CString cszText;
         cszText.Format(_T("%u mm"), uiValue);
         return cszText;
      }

   case prPTP_DEV_PROP_ZOOM_POS:
   case prPTP_DEV_PROP_EZOOM_SIZE:
   case prPTP_DEV_PROP_EZOOM_SIZE_OF_TELE:
   case prPTP_DEV_PROP_EZOOM_START_POS:
      {
         unsigned int uiValue = GetUnsignedIntValue(value);
         CString cszText;
         cszText.Format(_T("%u"), uiValue);
         return cszText;
      }

   case prPTP_DEV_PROP_FLASH_QUANTITY:
      {
         unsigned int uiValue = GetUnsignedIntValue(value);

         CString cszText;
         if (uiValue == 0xffff)
            cszText = _T("N/A");
         else
            cszText.Format(_T("%u"), uiValue);

         return cszText;
      }

   case prPTP_DEV_PROP_IMEGE_FILE_SIZE:
      {
         std::vector<unsigned int> vecFileSizes = value.GetArray<unsigned int>();

         CString cszText;
         for (size_t i = 0, iMax = vecFileSizes.size(); i < iMax; i++)
            cszText.AppendFormat(_T("%u kb, "), vecFileSizes[i] / 1024);
         cszText.TrimRight(_T(", "));
         return cszText;
      }

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
   if (value.Type() == Variant::typeInvalid)
      return _T("invalid");

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

CString PropertyAccess::FormatPropSupportedSize(const Variant& value)
{
   std::vector<unsigned int> vecSizeInfo = value.GetArray<unsigned int>();
   if (vecSizeInfo.size() < 6)
      return _T("invalid data");

   CString cszFormatImage;

   Variant imageFormat;
   imageFormat.Set(vecSizeInfo[2]);
   imageFormat.SetType(Variant::typeUInt32);

   FormatValueById(g_aPropIdDisplayInfo, sizeof(g_aPropIdDisplayInfo) / sizeof(*g_aPropIdDisplayInfo),
      prPTP_DEV_PROP_SUPPORTED_SIZE_META_IMAGEFORMAT, imageFormat, cszFormatImage);

   CString cszFormatThumb;
   imageFormat.Set(vecSizeInfo[5]);

   FormatValueById(g_aPropIdDisplayInfo, sizeof(g_aPropIdDisplayInfo) / sizeof(*g_aPropIdDisplayInfo),
      prPTP_DEV_PROP_SUPPORTED_SIZE_META_IMAGEFORMAT, imageFormat, cszFormatThumb);

   CString cszText;
   cszText.Format(_T("Image:%ux%u (%s), Thumb:%ux%u (%s)"),
      vecSizeInfo[0],
      vecSizeInfo[1],
      cszFormatImage.GetString(),
      vecSizeInfo[3],
      vecSizeInfo[4],
      cszFormatThumb.GetString());

   return cszText;
}
