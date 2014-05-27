//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PsrecPropertyAccess.hpp PS-ReC - device/image property access
//
#pragma once

// includes
#include "PsrecCommon.hpp"
#include "PsrecVarDataParser.hpp"
#include "Variant.hpp"

namespace PSREC
{
/// dummy property value for getting propImageFormat value
const prUInt16 PSREC_PROP_IMAGE_FORMAT = 0x1000;

/// device value parser
struct DeviceValueParser: public VarDataParser
{
   /// reads next value from buffer
   Variant ReadValue(prUInt16 uiDataType)
   {
      Variant v;

      switch (uiDataType)
      {
      case 0x0001: v.Set<prInt8>(ReadUint8());     v.SetType(Variant::typeInt8); break; // prInt8
      case 0x0002: v.Set<prUInt8>(ReadUint8());    v.SetType(Variant::typeUInt8); break; // prUInt8
      case 0x0003: v.Set<prInt16>(ReadUint16());   v.SetType(Variant::typeInt16); break; // prInt16
      case 0x0004: v.Set<prUInt16>(ReadUint16());  v.SetType(Variant::typeUInt16); break; // prUInt16
      case 0x0005: v.Set<prInt32>(ReadUint32());   v.SetType(Variant::typeInt32); break; // prInt32
      case 0x0006: v.Set<unsigned int>(ReadUint32());  v.SetType(Variant::typeUInt32); break; // prUInt32
      //case 0x0007: v.Set<prInt64>(); break; // prInt64
      //case 0x0008: v.Set<prUInt64>(); break; // prUInt64
      //case 0x0009: uiSize = 16; // signed 128 bit integer
      //case 0x000A: uiSize = 16; // unsigned 128 bit integer
      //case 0x4001: v.Set<>(); break; // prInt8 array
      case 0x4002: v.SetArray<prUInt8>(ReadUint8Array()); v.SetType(Variant::typeUInt8); break; // prUInt8 array
      //case 0x4003: v.Set<>(); break; // prInt16 array
      case 0x4004: v.SetArray<prUInt16>(ReadUint16Array()); v.SetType(Variant::typeUInt16); break; // prUInt16 array
      //case 0x4005: v.Set<>(); break; // prInt32 array
      case 0x4006: v.SetArray<unsigned int>(ReadUint32Array()); v.SetType(Variant::typeUInt32); break; // prUInt32 array
      //case 0x4007: v.Set<>(); break; // prInt64 array
      //case 0x4008: v.Set<>(); break; // prUInt64 array
      //case 0x4009: uiSize = 16; bArray = true; break; // array of signed 128 bit integer
      //case 0x400A: uiSize = 16; bArray = true; break; // array of unsigned 128 bit integer
      case 0xFFFF:
         v.Set<CString>(ReadString());
         v.SetType(Variant::typeString);
         break;

      default:
         ATLASSERT(false);
         break;
      }

      return v;
   }
};


/// access to device property description
struct DevicePropDesc: public DeviceValueParser
{
public:
   /// ctor; reads property description from camera
   DevicePropDesc(prHandle hCamera, prUInt16 propId, bool bParseValues = false)
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

   /// parses property description
   void Parse()
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

            // TODO add all values in between
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

   /// returns if property can be written to
   bool IsSetAllowed() const throw()
   {
      // 0x00 means read-only
      // 0x01 means read-write
      return m_uiGetSet != 0x00;
   }

   /// creates value data from variant
   void CreateValueData(Variant v, std::vector<BYTE>& vecValueData)
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

   prUInt16 m_uiDevicePropCode;  ///< device property code
   prUInt16 m_uiDataType;        ///< data type
   prUInt8 m_uiGetSet;           ///< get/set possible?
   Variant m_varDefaultValue;    ///< default value
   Variant m_varCurrentValue;    ///< current value
   prUInt8 m_uiFormat;           ///< format

   std::vector<Variant> m_vecAllValues;   ///< all values

private:
   /// indicates if available values should be parsed
   bool m_bParseValues;
};

/// combined image or device property
class PropertyAccess
{
public:
   /// ctor
   PropertyAccess(prHandle hCamera) throw()
      :m_hCamera(hCamera)
   {
   }

   /// returns current value of property
   Variant Get(prUInt16 propId) const
   {
      // special case: image format
      if (propId == PSREC_PROP_IMAGE_FORMAT)
         return GetImageFormatProperty();

      // Note: we could use PR_GetDevicePropValue here, but we have to call
      // PR_GetDevicePropDesc anyway, to find out data type; so we just use
      // the current value field then.
      DevicePropDesc desc(m_hCamera, propId, false);
      return desc.m_varCurrentValue;
   }

   /// sets property
   void Set(prUInt16 propId, Variant v)
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

   /// enumerates all values for given property
   void Enum(prUInt16 propId, std::vector<Variant>& vecValues)
   {
      DevicePropDesc desc(m_hCamera, propId, true);
      vecValues = desc.m_vecAllValues;
   }

   /// returns value for image format property
   Variant GetImageFormatProperty() const;

   /// converts from property id to property name
   static LPCTSTR NameFromId(prUInt16 propertyId) throw()
   {
      LPCTSTR pszName = _T("???");
      switch (propertyId)
      {
      case prPTP_DEV_PROP_BUZZER: pszName = _T("Set on/off the device buzzer"); break;
      case prPTP_DEV_PROP_BATTERY_KIND: pszName = _T("Type of the battery"); break;
      case prPTP_DEV_PROP_BATTERY_STATUS: pszName = _T("Battery Status"); break;
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
      case prPTP_DEV_PROP_AF_DISTANCE: pszName = _T("Search range in the AF mode"); break;
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
      case prPTP_DEV_PROP_AV_MAX: pszName = _T("maximum aperture value"); break;
      case prPTP_DEV_PROP_FOCAL_LENGTH: pszName = _T("Value corresponding to the current focal distance multiplied by FocalLengthDenominator"); break;
      case prPTP_DEV_PROP_FOCAL_LENGTH_TELE: pszName = _T("Value corresponding to the telescopic focal distance multiplied by FocalLengthDenominator"); break;
      case prPTP_DEV_PROP_FOCAL_LENGTH_WIDE: pszName = _T("Value corresponding to the wide-angle focus distance multiplied by FocalLengthDenominator"); break;
      case prPTP_DEV_PROP_FOCAL_LENGTH_DENOMI: pszName = _T("Focus information multiplier value"); break;
      case prPTP_DEV_PROP_CAPTURE_TRANSFER_MODE: pszName = _T("Image transfer mode to be applied at capture"); break;
      case prPTP_DEV_PROP_ZOOM_POS: pszName = _T("Current zoom position"); break;
      case prPTP_DEV_PROP_SUPPORTED_SIZE: pszName = _T("Support size"); break;
      case prPTP_DEV_PROP_SUPPORTED_THUMB_SIZE: pszName = _T("Thumbnail size supported by the device"); break;
      case prPTP_DEV_PROP_FIRMWARE_VERSION: pszName = _T("Version of the camera device firmware"); break;
      case prPTP_DEV_PROP_CAMERA_MODEL_NAME: pszName = _T("Camera model"); break;
      case prPTP_DEV_PROP_OWNER_NAME: pszName = _T("Owner name"); break;
      case prPTP_DEV_PROP_CAMERA_TIME: pszName = _T("Current time information in the device"); break;
      case prPTP_DEV_PROP_CAMERA_OUTPUT: pszName = _T("Destination of image signal output in the Viewfinder mode"); break;
      case prPTP_DEV_PROP_DISP_AV: pszName = _T("How to display the Av value"); break;
      case prPTP_DEV_PROP_AV_OPEN_APEX: pszName = _T("Open aperture value"); break;
      case prPTP_DEV_PROP_EZOOM_SIZE: pszName = _T("Horizontal size of image to be cut out from CCD image using electronic zoom"); break;
      case prPTP_DEV_PROP_ML_SPOT_POS: pszName = _T("Spot metering positon"); break;
      case prPTP_DEV_PROP_DISP_AV_MAX: pszName = _T("How to display the maximum Av value"); break;
      case prPTP_DEV_PROP_AV_MAX_APEX: pszName = _T("Maximum aperture value"); break;
      case prPTP_DEV_PROP_EZOOM_START_POS: pszName = _T("Zoom position at which the electornic zoom range starts"); break;
      case prPTP_DEV_PROP_FOCAL_LENGTH_OF_TELE: pszName = _T("Focal distance at the optical telescopic end"); break;
      case prPTP_DEV_PROP_EZOOM_SIZE_OF_TELE: pszName = _T("Horizontal size of image to be cut out from CCD image at the telescopic end of the electronic zoom range"); break;
      case prPTP_DEV_PROP_PHOTO_EFFECT: pszName = _T("Photo effect"); break;
      case prPTP_DEV_PROP_AF_LIGHT: pszName = _T("ON/OFF of AF assist light"); break;
      case prPTP_DEV_PROP_FLASH_QUANTITY: pszName = _T("Number of flash levels that can be set in the manual mode"); break;
      case prPTP_DEV_PROP_ROTATION_ANGLE: pszName = _T("Angle of rotation detected by the gravity sensor"); break;
      case prPTP_DEV_PROP_ROTATION_SENCE: pszName = _T("Whether the gravity sensor is enabled or disabled"); break;
      case prPTP_DEV_PROP_IMEGE_FILE_SIZE: pszName = _T("Image file size supported by the camera"); break;
      case prPTP_DEV_PROP_CAMERA_MODEL_ID: pszName = _T("Camera model ID"); break;
      case PSREC_PROP_IMAGE_FORMAT: pszName = _T("Image format"); break;
      }

      return pszName;
   }

   /// formats display text from id and value
   static CString DisplayTextFromIdAndValue(prUInt16 propertyId, Variant value);

   /// formats image format value
   static CString FormatImageFormatValue(const Variant& value);

private:
   /// camera handle
   prHandle m_hCamera;
};

} // namespace PSREC
