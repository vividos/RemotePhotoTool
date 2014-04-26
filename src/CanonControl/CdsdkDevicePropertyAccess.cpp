//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CdsdkDevicePropertyAccess.cpp CDSDK - Device property access
//

// includes
#include "stdafx.h"
#include "CdsdkDevicePropertyAccess.hpp"
#include "CommonPropertyAccess.hpp"

using namespace CDSDK;

/// all device property descriptions for CDSDK
static PropIdDisplayInfo g_aPropIdDisplayInfo[] =
{
   {
      cdDEVICE_PROP_BATTERY_STATUS,
      {
         { BATTERY_LEVEL_NORMAL, _T("Normal") },
         { BATTERY_LEVEL_WEAK, _T("Weak") },
         { BATTERY_LEVEL_SAFETY_LOW, _T("Safety low") },
         { BATTERY_LEVEL_LB, _T("LB") },
         { 0, nullptr }
      }
   },

   {
      cdDEVICE_PROP_SLIDE_SHOW_CAP,
      {
         { 0, _T("N/A") },
         { cdSLIDE_SHOW_CAP_CANON_FORMAT, _T("Canon format slide show file") },
         { cdSLIDE_SHOW_CAP_DPOF, _T("DPOF slide show file") },
         { 0, nullptr }
      }
   },

   {
      cdDEVICE_PROP_PARSE_FACULTIES,
      {
         { 0, _T("N/A") },
         { cdPARSE_SET_PROP_IN_CAMERA, _T("Supports modifying properties") },
         { 0, nullptr }
      }
   },
};

Variant DevicePropertyAccess::Get(cdDevicePropertyID propId) const
{
   // first, get size of property
   cdUInt32 size = GetSize(propId);

   // now get the data bytes
   std::vector<BYTE> vecData(size);

   // may return cdINVALID_PARAMETER, cdINVALID_HANDLE, cdINVALID_POINTER, cdINVALID_ID,
   // cdPROPERTIES_UNAVAILABLE
   cdError err = CDGetDevicePropertyData(m_hSource, propId, &size, &vecData[0], 0);
   if (err != cdOK) LOG_TRACE(_T("CDGetDevicePropertyData(%08x, propId = %08x \"%s\", &size = %u, buffer, 0) returned %08x\n"),
      m_hSource, propId, NameFromId(propId), size, err);
   CheckError(_T("CDGetDevicePropertyData"), err, __FILE__, __LINE__);

   // convert to variant
   Variant v;
   SetRawCdsdk(v, propId, vecData);
   return v;
}

void DevicePropertyAccess::Set(cdDevicePropertyID propId, Variant v)
{
   // generate data bytes from variant
   std::vector<BYTE> vecData;
   GetRawCdsdk(v, propId, vecData);
   cdUInt32 size = static_cast<cdUInt32>(vecData.size());

   // now set the property

   // may return cdINVALID_PARAMETER, cdINVALID_HANDLE, cdINVALID_POINTER, cdINVALID_ID,
   // cdPROPERTIES_UNAVAILABLE
   cdError err = CDSetDevicePropertyData(m_hSource, propId, &size, &vecData[0]);
   LOG_TRACE(_T("CDSetDevicePropertyData(%08x, propId = %u, &size = %u, buffer) returned %08x\n"),
      m_hSource, propId, size, err);
   CheckError(_T("CDSetDevicePropertyData"), err, __FILE__, __LINE__);
}

cdUInt32 DevicePropertyAccess::GetSize(cdDevicePropertyID propId) const
{
   cdUInt32 size = 0;

   // may return cdINVALID_PARAMETER, cdINVALID_HANDLE, cdINVALID_POINTER, cdINVALID_ID,
   // cdPROPERTIES_UNAVAILABLE
   cdError err = CDGetDevicePropertyData(m_hSource, propId, &size, nullptr, 0);
   if (err != cdOK) LOG_TRACE(_T("CDGetDevicePropertyData(%08x, propId = %08x \"%s\", &size = %u, nullptr, 0) returned %08x\n"),
      m_hSource, propId, NameFromId(propId), size, err);
   CheckError(_T("CDGetDevicePropertyData"), err, __FILE__, __LINE__);

   return size;
}

/// \note vecData is passed by value, since it is modified in this function anyway
void DevicePropertyAccess::SetRawCdsdk(Variant& v, unsigned int propId, std::vector<unsigned char> vecData)
{
   boost::any variant;
   Variant::VariantType enType = Variant::typeInvalid;
   bool bIsArray = false;

   switch (propId)
   {
      // uint32
   case cdDEVICE_PROP_MODEL_ID:
   case cdDEVICE_PROP_SLIDE_SHOW_CAP: // cdSlideShowCap
   case cdDEVICE_PROP_DPOF_CAP:
   case cdDEVICE_PROP_RELEASE_CONTROL_CAP:
   case cdDEVICE_PROP_RAW_DEVELOP_FACULTIES: // cdRawDevelopFaculty
   case cdDEVICE_PROP_PARSE_FACULTIES: // cdParseFaculty
   case cdDEVICE_PROP_TIME: // cdTime
   case cdDEVICE_PROP_BATTERY_STATUS:
      ATLASSERT(vecData.size() >= 4);
      {
         // little endian
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
      enType = Variant::typeBool;
      break;

      // uint8
   case cdDEVICE_PROP_DIRECT_TRANSFER_STATUS:
      ATLASSERT(vecData.size() >= 1);
      variant = static_cast<unsigned char>(vecData[0]);
      enType = Variant::typeUInt8;
      break;

      // String
   case cdDEVICE_PROP_MODEL_NAME:
   case cdDEVICE_PROP_OWNER_NAME:
   case cdDEVICE_PROP_FIRMWARE_VERSION:
      vecData.push_back(0);
      variant = CString(reinterpret_cast<const char*>(&vecData[0]));
      enType = Variant::typeString;
      break;

      // custom data, as array of uint8
   case cdDEVICE_PROP_THUMB_VALID_AREA: // this property isn't described in cdType.h, but assume it's an array
   case cdDEVICE_PROP_UNKNOWN1:
      variant = std::vector<unsigned char>(vecData);
      enType = Variant::typeUInt8;
      bIsArray = true;
      break;

   default:
      ATLASSERT(false);
      break;
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

LPCTSTR DevicePropertyAccess::NameFromId(unsigned int propId) throw()
{
   cdDevicePropertyID devicePropId = static_cast<cdDevicePropertyID>(propId);

   LPCTSTR pszName = _T("???");
   switch (devicePropId)
   {
   case cdDEVICE_PROP_INVALID:                  pszName = _T("Invalid property ID"); break;
   case cdDEVICE_PROP_MODEL_ID:                 pszName = _T("Camera model ID"); break;
   case cdDEVICE_PROP_MODEL_NAME:               pszName = _T("Camera model name"); break;
   case cdDEVICE_PROP_SLIDE_SHOW_CAP:           pszName = _T("Slide show capacity"); break;
   case cdDEVICE_PROP_UPLOAD_FILE_CAP:          pszName = _T("Picture upload support"); break;
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
   case cdDEVICE_PROP_UNKNOWN1:                 pszName = _T("?? unknown ??"); break;
   }

   return pszName;
}

CString DevicePropertyAccess::DisplayTextFromIdAndValue(unsigned int propId, Variant value)
{
   cdDevicePropertyID devicePropId = static_cast<cdDevicePropertyID>(propId);

   CString cszText;

   switch (devicePropId)
   {
   case cdDEVICE_PROP_TIME: // cdTime
      {
         cdTime time = value.Get<unsigned int>();

         FILETIME fileTime = { 0 };
         BOOL bRet = DosDateTimeToFileTime(HIWORD(time), LOWORD(time), &fileTime);
         ATLASSERT(TRUE == bRet); bRet;

         SYSTEMTIME systemTime = { 0 };
         bRet = FileTimeToSystemTime(&fileTime, &systemTime);
         ATLASSERT(TRUE == bRet);

         CString cszDate;
         GetDateFormat(LOCALE_USER_DEFAULT, 0, &systemTime, _T("yyyy'-'MM'-'dd"), cszDate.GetBuffer(32), 32);
         cszDate.ReleaseBuffer();

         CString cszTime;
         GetTimeFormat(LOCALE_USER_DEFAULT, 0, &systemTime, _T(" HH':'mm':'ss"), cszTime.GetBuffer(32), 32);
         cszTime.ReleaseBuffer();

         cszText = cszDate + cszTime;
      }
      break;

   default:
      if (!FormatValueById(g_aPropIdDisplayInfo, sizeof(g_aPropIdDisplayInfo) / sizeof(*g_aPropIdDisplayInfo),
         propId, value, cszText))
         cszText = value.ToString();
      break;
   }

   return cszText;
}
