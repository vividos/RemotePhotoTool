//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file EdsdkPropertyAccess.cpp EDSDK - Property access
//

// includes
#include "stdafx.h"
#include "EdsdkPropertyAccess.hpp"
#include "CommonPropertyAccess.hpp"

using namespace EDSDK;

/// all property descriptions for EDSDK
static PropIdDisplayInfo g_aPropIdDisplayInfo[] =
{
   // camera properties
   {
      kEdsPropID_BatteryQuality,
      {
         { 3, _T("Full") },
         { 2, _T("High") },
         { 1, _T("Half") },
         { 0, _T("Low") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_SaveTo,
      {
         { 1, _T("Camera") },
         { 2, _T("Host") },
         { 3, _T("Camera and Host") },
         { 0, nullptr }
      }
   },

   // image properties
   {
      kEdsPropID_Orientation,
      {
         { 1, _T("0 Degrees") },
         { 3, _T("180 Degrees") },
         { 6, _T("90 Degrees") },
         { 8, _T("270 Degrees") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_WhiteBalance,
      {
         { 0, _T("Auto") },
         { 1, _T("Daylight") },
         { 2, _T("Cloudy") },
         { 3, _T("Tungsten") },
         { 4, _T("Fluorescent") },
         { 5, _T("Flash") },
         { 6, _T("Manual") },
         { 8, _T("Shade") },
         { 9, _T("Color temp.") },
         { 10, _T("PC-1") },
         { 11, _T("PC-2") },
         { 12, _T("PC-3") },
         { 15, _T("Manual 2") },
         { 16, _T("Manual 3") },
         { 18, _T("Manual 4") },
         { 19, _T("Manual 5") },
         { 20, _T("PC-4") },
         { 21, _T("PC-5") },
         { 23, _T("White")},
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_ColorSpace,
      {
         { 1, _T("sRGB") },
         { 2, _T("Adobe RGB") },
         { 0xFFFFFFFF, _T("Unknown") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_PictureStyle,
      {
         { 0x0081, _T("Standard") },
         { 0x0082, _T("Portrait") },
         { 0x0083, _T("Landscape") },
         { 0x0084, _T("Neutral") },
         { 0x0085, _T("Faithful") },
         { 0x0086, _T("Monochrome") },
         { 0x0087, _T("Auto") },
         { 0x0088, _T("Fine detail") },
         { 0x0021, _T("User Style 1") }, /* kEdsPictureStyle_User1 */
         { 0x0022, _T("User Style 2") }, /* kEdsPictureStyle_User2 */
         { 0x0023, _T("User Style 3") }, /* kEdsPictureStyle_User3 */
         { 0x0041, _T("Computer Setting 1") },
         { 0x0042, _T("Computer Setting 2") },
         { 0x0043, _T("Computer Setting 3") },
         { 0, nullptr }
      }
   },

   // capture properties
   {
      kEdsPropID_AEMode,
      {
         { 0, _T("P") },
         { 1, _T("Tv") },
         { 2, _T("Av") },
         { 3, _T("M") },
         { 4, _T("Bulb") },
         { 5, _T("A-DEP") },
         { 6, _T("DEP") },
         { 7, _T("Custom 1") },
         { 9, _T("Auto") },
         { 10, _T("Night Scene Portrait") },
         { 11, _T("Sports") },
         { 12, _T("Portrait") },
         { 13, _T("Landscape") },
         { 14, _T("Close-Up") },
         { 15, _T("Flash Off") },
         { 16, _T("Custom 2") },
         { 17, _T("Custom 3") },
         { 19, _T("Creative Auto") },
         { 19, _T("Movie") },
         { 21, _T("Photo in movie") },
         { 22, _T("Scene intelligent auto") },
         { 23, _T("Night scenes") },
         { 24, _T("Backlit scenes") },
         { 25, _T("SCN") },
         { 26, _T("Children") },
         { 27, _T("Food") },
         { 28, _T("Candlelight portraits") },
         { (unsigned int)kEdsAEMode_CreativeFilter, _T("Creative filter") },
         { (unsigned int)kEdsAEMode_RoughMonoChrome, _T("Rough monochrome") },
         { (unsigned int)kEdsAEMode_SoftFocus, _T("Soft focus") },
         { (unsigned int)kEdsAEMode_ToyCamera, _T("Toy camera") },
         { (unsigned int)kEdsAEMode_Fisheye, _T("Fisheye") },
         { (unsigned int)kEdsAEMode_WaterColor, _T("Waltercolor") },
         { (unsigned int)kEdsAEMode_Miniature, _T("Miniature") },
         { (unsigned int)kEdsAEMode_Hdr_Standard, _T("HDR Standard") },
         { (unsigned int)kEdsAEMode_Hdr_Vivid, _T("HDR Vivid") },
         { (unsigned int)kEdsAEMode_Hdr_Bold, _T("HDR Bold") },
         { (unsigned int)kEdsAEMode_Hdr_Embossed, _T("HDR Embossed") },
         { (unsigned int)kEdsAEMode_Movie_Fantasy, _T("Movie Fantasy") },
         { (unsigned int)kEdsAEMode_Movie_Old, _T("Movie Old") },
         { (unsigned int)kEdsAEMode_Movie_Memory, _T("Movie Memory") },
         { (unsigned int)kEdsAEMode_Movie_DirectMono, _T("Movie Direct mono") },
         { (unsigned int)kEdsAEMode_Movie_Mini, _T("Movie mini") },
         { (unsigned int)kEdsAEMode_PanningAssist, _T("Panning assist") },
         { (unsigned int)kEdsAEMode_GroupPhoto, _T("Group photo") },
         { (unsigned int)kEdsAEMode_Unknown, _T("Unknown") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_DriveMode,
      {
         { 0x00000000, _T("Single-Frame Shooting") },
         { 0x00000001, _T("Continuous Shooting") },
         { 0x00000002, _T("Video") },
         { 0x00000003, _T("Not used") },
         { 0x00000004, _T("High-Speed Continuous Shooting") },
         { 0x00000005, _T("Low-Speed Continuous Shooting") },
         { 0x00000006, _T("Silent single shooting") },
         { 0x00000007, _T("10 Second Self-Timer plus continuous shots") },
         { 0x00000010, _T("10 Second Self-Timer") },
         { 0x00000011, _T("2 Second Self-Timer") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_MeteringMode,
      {
         { 1, _T("Spot metering") },
         { 3, _T("Evaluative metering") },
         { 4, _T("Partial metering") },
         { 5, _T("Center-weighted averaging metering") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_AFMode,
      {
         { 0, _T("One-Shot AF") },
         { 1, _T("AI Servo AF") },
         { 2, _T("AI Focus AF") },
         { 3, _T("Manual Focus") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_Bracket,
      {
         { 0x00, _T("Off") }, // not in manual, but also means off
         { 0x01, _T("AE bracket") },
         { 0x02, _T("ISO bracket") },
         { 0x04, _T("WB bracket") },
         { 0x08, _T("FE bracket") },
         { 0xFFFFFFFF, _T("Bracket off") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_NoiseReduction,
      {
         { 0, _T("Off") },
         { 1, _T("On 1") },
         { 2, _T("On 2") },
         { 3, _T("On") },
         { 4, _T("Auto") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_FlashOn,
      {
         { 0, _T("No flash") },
         { 1, _T("Flash") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_RedEye,
      {
         { 0, _T("Off") },
         { 1, _T("On") },
         { 0xFFFFFFFF, _T("Invalid value") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_LensStatus,
      {
         { 0, _T("Lens not attached") },
         { 1, _T("Lens attached") },
         { 0, nullptr }
      }
   },

   // viewfinder properties
   {
      kEdsPropID_Evf_OutputDevice,
      {
         { 0, _T("Off") },
         { 1, _T("Camera") },
         { 2, _T("PC") },
         { 3, _T("Camera and PC") },
         { 4, _T("Mobile") },
         { 8, _T("Mobile 2") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_Evf_Mode,
      {
         { 0, _T("Disabled") },
         { 1, _T("Enabled") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_Evf_DepthOfFieldPreview,
      {
         { 0, _T("Off") },
         { 1, _T("On") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_Evf_Zoom,
      {
         { 1, _T("Fit") },
         { 5, _T("x5") },
         { 10, _T("x10") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_Evf_HistogramStatus,
      {
         { 0, _T("Hide") },
         { 1, _T("Display") },
         { 2, _T("Grayout") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_Evf_AFMode,
      {
         { 0, _T("Quick mode") },
         { 1, _T("Live mode") },
         { 2, _T("Live face mode") },
         { 3, _T("Live multi") },
         { 0, nullptr }
      }
   },

   {
      kEdsPropID_Record,
      {
         { 0, _T("End movie shooting") },
         { 4, _T("Begin movie shooting") },
         { 0, nullptr }
      }
   },
};

// PropertyAccess functions

Variant PropertyAccess::Get(EdsPropertyID propId, int iParam) const
{
   if (!m_h.IsValid())
      return Variant();

   if (propId == kEdsPropID_Meta_MaxZoomPos)
   {
      Variant v;
      v.Set<unsigned int>(kEdsEvfZoom_x10);
      v.SetType(Variant::typeUInt32);

      return v;
   }

   MutexTryLock<RecursiveMutex> tryLock(const_cast<Handle&>(m_h).GetRef()->SdkFunctionMutex());

   while (!tryLock.Try(10))
      EDSDK::Ref::OnIdle();

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

void PropertyAccess::Set(EdsPropertyID propId, Variant value, int iParam)
{
   if (propId == kEdsPropID_Meta_MaxZoomPos)
      return;

   if (!m_h.IsValid())
      return;

   MutexTryLock<RecursiveMutex> tryLock(const_cast<Handle&>(m_h).GetRef()->SdkFunctionMutex());

   while (!tryLock.Try(10))
      EDSDK::Ref::OnIdle();

   // first, get type and size of property
   EdsDataType dataType = kEdsDataType_Unknown;
   EdsUInt32 size = 0;
   GetTypeAndSize(propId, iParam, dataType, size);

   // then generate data bytes from variant
   std::vector<BYTE> vecData;
   GetRawEdsdk(value, dataType, vecData);

   LOG_TRACE(_T("Property [%s] set to value [%s]\n"),
      PropertyAccess::NameFromId(propId),
      PropertyAccess::DisplayTextFromIdAndValue(propId, value).GetString());

   // now set the property
   EdsError err = EdsSetPropertyData(m_h, propId, iParam, size, &vecData[0]);
   if (err != EDS_ERR_OK) // only log errors here
      LOG_TRACE(_T("EdsSetPropertyData(ref = %08x, id = %04x, param = %u, size = %u, &data) returned %08x\n"),
      m_h.Get(), propId, iParam, size, err);
   EDSDK::CheckError(_T("EdsSetPropertyData"), err, __FILE__, __LINE__);
}

void PropertyAccess::Enum(EdsPropertyID propId, std::vector<Variant>& vecValues, bool& bReadOnly)
{
   // special cases
   if (propId == kEdsPropID_Meta_MaxZoomPos)
      return;

   if (!m_h.IsValid())
      return;

   if (propId == kEdsPropID_Evf_Zoom)
   {
      unsigned int auiZoomValues[] = {
         kEdsEvfZoom_Fit,
         kEdsEvfZoom_x5,
         kEdsEvfZoom_x10,
      };

      std::for_each(std::begin(auiZoomValues), std::end(auiZoomValues), [&](unsigned int& uiValue)
      {
         Variant v;
         v.Set(uiValue);
         v.SetType(Variant::typeUInt32);
         vecValues.push_back(v);
      });

      return;
   }

   MutexTryLock<RecursiveMutex> tryLock(const_cast<Handle&>(m_h).GetRef()->SdkFunctionMutex());

   while (!tryLock.Try(10))
      EDSDK::Ref::OnIdle();

   // note: since all properties that can be retrieved are of type UInt8, assume that
   EdsPropertyDesc propDesc = { 0 };
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
      for (EdsInt32 i = 0; i<propDesc.numElements; i++)
      {
         Variant v;
         v.Set(puiData[i]);
         v.SetType(Variant::typeUInt32);
         vecValues.push_back(v);
      }
   }
}

bool PropertyAccess::IsReadOnly(EdsPropertyID propId) const
{
   if (propId == kEdsPropID_Meta_MaxZoomPos) return true;
   if (propId == kEdsPropID_Evf_Zoom) return false;

   if ((propId & 0xFFFF) == kEdsPropID_CFn)
   {
      // support for custom functions: EdsGetPropertyDesc can't be used, so
      // return that they are always read-write
      return false;
   }

   if (!m_h.IsValid())
      return true;

   MutexTryLock<RecursiveMutex> tryLock(const_cast<Handle&>(m_h).GetRef()->SdkFunctionMutex());

   while (!tryLock.Try(10))
      EDSDK::Ref::OnIdle();

   EdsPropertyDesc propDesc = { 0 };
   EdsError err = EdsGetPropertyDesc(m_h, propId, &propDesc);
   //LOG_TRACE(_T("EdsGetPropertyDesc(ref = %08x, id = %04x, &numProp = %u) returned %08x\n"),
   //   m_h.Get(), propId, propDesc.numElements, err);
   EDSDK::CheckError(_T("EdsGetPropertyDesc"), err, __FILE__, __LINE__);

   // note: this reinterpret cast assumes that EdsInt32 and EdsUInt32 have the same size
   static_assert(sizeof(propDesc.propDesc[0]) == sizeof(EdsUInt32), "propDesc must have type and size EdsUInt32");

   return (propDesc.access == 0);
}

void PropertyAccess::GetTypeAndSize(EdsPropertyID propId, int iParam, EdsDataType& dataType, EdsUInt32& size) const
{
   if (!m_h.IsValid())
      return;

   MutexTryLock<RecursiveMutex> tryLock(const_cast<Handle&>(m_h).GetRef()->SdkFunctionMutex());

   while (!tryLock.Try(10))
      EDSDK::Ref::OnIdle();

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

bool PropertyAccess::IsPropertyAvail(unsigned int uiPropId, int iParam) const
{
   if (uiPropId >= kEdsPropID_Meta_MaxZoomPos)
      return true;

   if (!m_h.IsValid())
      return false;

   MutexTryLock<RecursiveMutex> tryLock(const_cast<Handle&>(m_h).GetRef()->SdkFunctionMutex());

   try
   {
      while (!tryLock.Try(10))
         EDSDK::Ref::OnIdle();
   }
   catch (...)
   {
      return false;
   }

   // check if property exists by retrieving type and size
   EdsDataType dataType = kEdsDataType_Unknown;
   EdsUInt32 size = 0;
   EdsError err = EdsGetPropertySize(m_h.Get(), uiPropId, iParam, &dataType, &size);
   return (err == EDS_ERR_OK && dataType != kEdsDataType_Unknown);
}

// ImageFormat functions

/// bitset for image format 32-bit value
union ImageFormatBitSet
{
   /// 32-bit value
   unsigned int uiValue;

   /// bit set
   // NOSONAR
   struct
   {
      // secondary image values
      EdsCompressQuality compressQuality2nd : 4; // 0-3 // NOSONAR
      EdsImageType imageType2nd : 4; // 4-7
      EdsImageSize imageSize2nd : 4; // 8-11
      unsigned int reserved1 : 4;

      // main image values
      EdsCompressQuality compressQuality : 4; // 16-19
      EdsImageType imageType : 4; // 20-23
      EdsImageSize imageSize : 4; // 24-27
      unsigned int reserved2 : 4; // 28-31
   } bits;
};

static_assert(sizeof(ImageFormatBitSet) == sizeof(unsigned int), "ImageFormatBitSet must have 32 bit");

/// converts image size to string
static LPCTSTR ImageSizeToString(EdsImageSize enSize)
{
   switch (enSize)
   {
   case kEdsImageSize_Large:   return _T("L"); // when quality is RAW, this value means full RAW
   case kEdsImageSize_Middle:  return _T("M");
   case kEdsImageSize_Small:   return _T("S"); // when quality is RAW, this value means sRAW
   case kEdsImageSize_Middle1: return _T("M1");
   case kEdsImageSize_Middle2: return _T("M2");
   case kEdsImageSize_Small1:  return _T("S1");
   case kEdsImageSize_Small2:  return _T("S2");
   case kEdsImageSize_Small3:  return _T("S3");
   case kEdsImageSize_Unknown: return _T("unknown");
   default:
      ATLASSERT(false);
      return _T("???");
   }
}

/// converts compress quality to string
static LPCTSTR CompressQualityToString(EdsCompressQuality enQuality)
{
   switch (enQuality)
   {
   case kEdsCompressQuality_Normal:     return _T("Normal");
   case kEdsCompressQuality_Fine:       return _T("Fine");
   case kEdsCompressQuality_Lossless:   return _T("RAW");
   case kEdsCompressQuality_SuperFine:  return _T("Superfine");
   case kEdsCompressQuality_Unknown:    return _T("unknown");
   default:
      ATLASSERT(false);
      return _T("???");
   }
}

CString PropertyAccess::FormatImageFormatValue(unsigned int uiValue)
{
   ImageFormatBitSet bitset;
   bitset.uiValue = uiValue;

   // main image
   CString cszText;

   // special cases for RAW and sRAW
   if (bitset.bits.compressQuality == kEdsCompressQuality_Lossless && bitset.bits.imageSize == kEdsImageSize_Large)
      cszText = _T("RAW");
   else
   if (bitset.bits.compressQuality == kEdsCompressQuality_Lossless && bitset.bits.imageSize == kEdsImageSize_Small)
      cszText = _T("sRAW");
   else
   {
      cszText.Format(_T("%s %s"),
         ImageSizeToString(bitset.bits.imageSize),
         CompressQualityToString(bitset.bits.compressQuality));
   }

   // secondary image
   if (bitset.bits.compressQuality2nd != kEdsCompressQuality_Unknown && bitset.bits.imageSize2nd != kEdsImageSize_Unknown)
   {
      cszText.AppendFormat(_T(" + %s %s"),
         ImageSizeToString(bitset.bits.imageSize2nd),
         CompressQualityToString(bitset.bits.compressQuality2nd));
   }

   return cszText;
}

EdsPropertyID PropertyAccess::MapToPropertyID(T_enImagePropertyType enProperty)
{
   switch (enProperty)
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
   case propCurrentZoomPos:         return kEdsPropID_Evf_Zoom;
   case propMaxZoomPos:             return kEdsPropID_Meta_MaxZoomPos;
   case propAvailableShots:         return kEdsPropID_AvailableShots;
   case propSaveTo:                 return kEdsPropID_SaveTo;
   case propBatteryLevel:           return kEdsPropID_BatteryQuality;
   case propImageFormat:            return kEdsPropID_ImageQuality;
   default:
      ATLASSERT(false);
      return kEdsPropID_Unknown;
   }
}

void PropertyAccess::EnumDeviceIds(std::vector<unsigned int>& vecDeviceIds)
{
   // since we have no function to enumerate all available properties, we just have to check
   // for every available device property here
   EdsPropertyID aAllPropIds[] =
   {
      kEdsPropID_ProductName,
      // kEdsPropID_BodyID, // not in SDK 2.12 anymore, replaced by kEdsPropID_BodyIDEx
      kEdsPropID_OwnerName,
      kEdsPropID_MakerName,
      kEdsPropID_DateTime,
      kEdsPropID_FirmwareVersion,
      kEdsPropID_BatteryLevel,
      kEdsPropID_CFn,
      kEdsPropID_SaveTo,
      kEdsPropID_CurrentStorage,
      kEdsPropID_CurrentFolder,
      kEdsPropID_MyMenu,
      kEdsPropID_BatteryQuality,
      kEdsPropID_BodyIDEx,
      kEdsPropID_HDDirectoryStructure,
      kEdsPropID_Unknown1,
   };

   for (unsigned int i=0; i<sizeof(aAllPropIds)/sizeof(*aAllPropIds); i++)
   {
      unsigned int uiPropId = aAllPropIds[i];

      if (IsPropertyAvail(uiPropId, 0))
         vecDeviceIds.push_back(uiPropId);
   }
}

void PropertyAccess::EnumImageIds(std::vector<unsigned int>& vecImageIds)
{
   EdsPropertyID aAllPropIds[] =
   {
      // capture properties
      kEdsPropID_AEMode,
      kEdsPropID_DriveMode,
      kEdsPropID_ISOSpeed,
      kEdsPropID_MeteringMode,
      kEdsPropID_AFMode,
      kEdsPropID_Av,
      kEdsPropID_Tv,
      kEdsPropID_ExposureCompensation,
      kEdsPropID_FlashCompensation,
      kEdsPropID_FocalLength,
      kEdsPropID_AvailableShots,
      kEdsPropID_Bracket,
      kEdsPropID_WhiteBalanceBracket,
      kEdsPropID_LensName,
      kEdsPropID_AEBracket,
      kEdsPropID_FEBracket,
      kEdsPropID_ISOBracket,
      kEdsPropID_NoiseReduction,
      kEdsPropID_FlashOn,
      kEdsPropID_RedEye,
      kEdsPropID_FlashMode,
      kEdsPropID_LensStatus,
      kEdsPropID_Artist	,
      kEdsPropID_Copyright	,
      kEdsPropID_DepthOfField	,
      kEdsPropID_EFCompensation,
      kEdsPropID_AEModeSelect,

      // evf properties
      kEdsPropID_Evf_OutputDevice,
      kEdsPropID_Evf_Mode,
      kEdsPropID_Evf_WhiteBalance,
      kEdsPropID_Evf_ColorTemperature,
      kEdsPropID_Evf_DepthOfFieldPreview,
      kEdsPropID_Evf_Zoom,
      kEdsPropID_Evf_ZoomPosition,
      kEdsPropID_Evf_FocusAid,
      kEdsPropID_Evf_Histogram,
      kEdsPropID_Evf_ImagePosition,
      kEdsPropID_Evf_HistogramStatus,
      kEdsPropID_Evf_AFMode,
      kEdsPropID_Record,
      kEdsPropID_Evf_HistogramY,
      kEdsPropID_Evf_HistogramR,
      kEdsPropID_Evf_HistogramG,
      kEdsPropID_Evf_HistogramB,
      kEdsPropID_Evf_CoordinateSystem,
      kEdsPropID_Evf_ZoomRect,
      kEdsPropID_Evf_ImageClipRect,

      // image properties
      kEdsPropID_ImageQuality,
      kEdsPropID_JpegQuality,
      kEdsPropID_Orientation,
      kEdsPropID_ICCProfile,
      kEdsPropID_FocusInfo,
      kEdsPropID_DigitalExposure,
      kEdsPropID_WhiteBalance,
      kEdsPropID_ColorTemperature,
      kEdsPropID_WhiteBalanceShift,
      kEdsPropID_Contrast,
      kEdsPropID_ColorSaturation,
      kEdsPropID_ColorTone,
      kEdsPropID_Sharpness,
      kEdsPropID_ColorSpace,
      kEdsPropID_ToneCurve,
      kEdsPropID_PhotoEffect,
      kEdsPropID_FilterEffect,
      kEdsPropID_ToningEffect,
      kEdsPropID_ParameterSet,
      kEdsPropID_ColorMatrix,
      kEdsPropID_PictureStyle,
      kEdsPropID_PictureStyleDesc,
      kEdsPropID_PictureStyleCaption,

      // image GPS properties
      kEdsPropID_GPSVersionID,
      kEdsPropID_GPSLatitudeRef,
      kEdsPropID_GPSLatitude,
      kEdsPropID_GPSLongitudeRef,
      kEdsPropID_GPSLongitude,
      kEdsPropID_GPSAltitudeRef,
      kEdsPropID_GPSAltitude,
      kEdsPropID_GPSTimeStamp,
      kEdsPropID_GPSSatellites,
      kEdsPropID_GPSStatus,
      kEdsPropID_GPSMapDatum,
      kEdsPropID_GPSDateStamp,

      // develop properties
      kEdsPropID_Linear,
      kEdsPropID_ClickWBPoint,
      kEdsPropID_WBCoeffs,
   };

   for (unsigned int i=0; i<sizeof(aAllPropIds)/sizeof(*aAllPropIds); i++)
   {
      unsigned int uiPropId = aAllPropIds[i];

      if (IsPropertyAvail(uiPropId, 0))
         vecImageIds.push_back(uiPropId);
   }
}

void PropertyAccess::SetRawEdsdk(Variant& v, unsigned int datatype, std::vector<unsigned char> vecData)
{
   boost::any variant;
   Variant::VariantType enType = Variant::typeInvalid;
   bool bIsArray = false;

   switch (datatype)
   {
   case kEdsDataType_Bool:
      variant = vecData[0] != 0;
      enType = Variant::typeBool;
      break;

   case kEdsDataType_String:
      vecData.push_back(0);
      variant = CString(reinterpret_cast<const char*>(&vecData[0]));
      enType = Variant::typeString;
      break;

   case kEdsDataType_Int8:
      variant = static_cast<char>(vecData[0]);
      enType = Variant::typeInt8;
      break;

   case kEdsDataType_UInt8:
      variant = static_cast<unsigned char>(vecData[0]);
      enType = Variant::typeUInt8;
      break;

   case kEdsDataType_Int16:
      variant = static_cast<short>(
         static_cast<unsigned short>(vecData[0]) |
         (static_cast<unsigned short>(vecData[1]) << 8));
      enType = Variant::typeInt16;
      break;

   case kEdsDataType_UInt16:
      variant =
         static_cast<unsigned short>(vecData[0]) |
         (static_cast<unsigned short>(vecData[1]) << 8);
      enType = Variant::typeUInt16;
      break;

   case kEdsDataType_Int32:
      {
         unsigned int uiValue =
            static_cast<unsigned int>(vecData[0]) |
            (static_cast<unsigned int>(vecData[1]) << 8) |
            (static_cast<unsigned int>(vecData[2]) << 16) |
            (static_cast<unsigned int>(vecData[3]) << 24);
         variant = static_cast<int>(uiValue);
      }
      enType = Variant::typeInt32;
      break;

   case kEdsDataType_UInt32:
      {
         // assuming little endian here...
         unsigned int uiValue =
            static_cast<unsigned int>(vecData[0]) |
            (static_cast<unsigned int>(vecData[1]) << 8) |
            (static_cast<unsigned int>(vecData[2]) << 16) |
            (static_cast<unsigned int>(vecData[3]) << 24);
         variant = uiValue;
         enType = Variant::typeUInt32;
      }
      break;

   case kEdsDataType_Int32_Array:
      {
         std::vector<int> vecValues;

         ATLASSERT(vecData.size() % 4 == 0);
         size_t iMax = vecData.size() / 4;

         for (size_t i=0; i<iMax; i++)
         {
            unsigned int uiValue =
               static_cast<unsigned int>(vecData[i*4 + 0]) |
               (static_cast<unsigned int>(vecData[i*4 + 1]) << 8) |
               (static_cast<unsigned int>(vecData[i*4 + 2]) << 16) |
               (static_cast<unsigned int>(vecData[i*4 + 3]) << 24);

            vecValues.push_back(static_cast<int>(uiValue));
         }

         variant = vecValues;

         bIsArray = true;
         enType = Variant::typeInt32;
      }
      break;

   case kEdsDataType_Int64:
   case kEdsDataType_UInt64:
   case kEdsDataType_Float:
   case kEdsDataType_Double:
   case kEdsDataType_ByteBlock:
   case kEdsDataType_Rational:
   case kEdsDataType_Point:
   case kEdsDataType_Rect:
   case kEdsDataType_Time:

   case kEdsDataType_Bool_Array:
   case kEdsDataType_Int8_Array:
   case kEdsDataType_Int16_Array:
   //case kEdsDataType_Int32_Array:
   case kEdsDataType_UInt8_Array:
   case kEdsDataType_UInt16_Array:
   case kEdsDataType_UInt32_Array:
   case kEdsDataType_Rational_Array:

   case kEdsDataType_FocusInfo:
   case kEdsDataType_PictureStyleDesc:
      // use all types as vector array
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

void PropertyAccess::GetRawEdsdk(const Variant& v, unsigned int datatype, std::vector<unsigned char>& vecData)
{
   switch (datatype)
   {
   case kEdsDataType_UInt32:
      {
         unsigned int uiValue = v.Get<unsigned int>();
         vecData.push_back(static_cast<unsigned char>(uiValue & 0xff));
         vecData.push_back(static_cast<unsigned char>((uiValue >> 8) & 0xff));
         vecData.push_back(static_cast<unsigned char>((uiValue >> 16) & 0xff));
         vecData.push_back(static_cast<unsigned char>((uiValue >> 24) & 0xff));
      }
      break;

   case kEdsDataType_Int32:
      {
         unsigned int uiValue = static_cast<unsigned int>(v.Get<int>());
         vecData.push_back(static_cast<unsigned char>(uiValue & 0xff));
         vecData.push_back(static_cast<unsigned char>((uiValue >> 8) & 0xff));
         vecData.push_back(static_cast<unsigned char>((uiValue >> 16) & 0xff));
         vecData.push_back(static_cast<unsigned char>((uiValue >> 24) & 0xff));
      }
      break;

   default:
      ATLASSERT(false);
      break;
   }
}


LPCTSTR PropertyAccess::NameFromId(EdsPropertyID propertyId)
{
   LPCTSTR pszName = _T("???");

   switch (propertyId)
   {
   case kEdsPropID_ProductName: pszName = _T("Product name"); break;
   case kEdsPropID_BodyID: pszName = _T("Body ID"); break;// not in 2.12 anymore
   case kEdsPropID_OwnerName: pszName = _T("Owner name"); break;
   case kEdsPropID_MakerName: pszName = _T("Maker name"); break;
   case kEdsPropID_DateTime: pszName = _T("Date / Time"); break;
   case kEdsPropID_FirmwareVersion: pszName = _T("Firmware version"); break;
   case kEdsPropID_BatteryLevel: pszName = _T("Battery level"); break;
   case kEdsPropID_CFn: pszName = _T("Custom function"); break;
   case kEdsPropID_SaveTo: pszName = _T("Save to"); break;
   case kEdsPropID_CurrentStorage: pszName = _T("Current storage"); break;
   case kEdsPropID_CurrentFolder: pszName = _T("Current folder"); break;
   case kEdsPropID_MyMenu: pszName = _T("My Menu"); break;
   case kEdsPropID_BatteryQuality: pszName = _T("Battery quality"); break;
   case kEdsPropID_BodyIDEx: pszName = _T("Body ID Ex"); break;
   case kEdsPropID_HDDirectoryStructure: pszName = _T("HD directory structure"); break;
   case kEdsPropID_Unknown1: pszName = _T("Unknown1"); break;
   case kEdsPropID_ShutterCounter: pszName = _T("Shutter counter"); break;

      // capture properties
   case kEdsPropID_AEMode: pszName = _T("Shooting mode"); break;
   case kEdsPropID_DriveMode: pszName = _T("Drive mode"); break;
   case kEdsPropID_ISOSpeed: pszName = _T("ISO speed"); break;
   case kEdsPropID_MeteringMode: pszName = _T("Metering mode"); break;
   case kEdsPropID_AFMode: pszName = _T("AF mode"); break;
   case kEdsPropID_Av: pszName = _T("Av"); break;
   case kEdsPropID_Tv: pszName = _T("Tv"); break;
   case kEdsPropID_ExposureCompensation: pszName = _T("Exposure compensation"); break;
   case kEdsPropID_FlashCompensation: pszName = _T("Flash compensation"); break;
   case kEdsPropID_FocalLength: pszName = _T("Focal length"); break;
   case kEdsPropID_AvailableShots: pszName = _T("Available shots"); break;
   case kEdsPropID_Bracket: pszName = _T("Bracket"); break;
   case kEdsPropID_WhiteBalanceBracket: pszName = _T("White balance bracket"); break;
   case kEdsPropID_LensName: pszName = _T("Lens name"); break;
   case kEdsPropID_AEBracket: pszName = _T("AEB (aperture bracketing)"); break;
   case kEdsPropID_FEBracket: pszName = _T("FEB (focus bracketing)"); break;
   case kEdsPropID_ISOBracket: pszName = _T("ISO bracketing"); break;
   case kEdsPropID_NoiseReduction: pszName = _T("Noise reduction"); break;
   case kEdsPropID_FlashOn: pszName = _T("Flash on"); break;
   case kEdsPropID_RedEye: pszName = _T("Red eye"); break;
   case kEdsPropID_FlashMode: pszName = _T("Flash mode"); break;
   case kEdsPropID_LensStatus: pszName = _T("Lens status"); break;
   case kEdsPropID_Artist: pszName = _T("Artist"); break;
   case kEdsPropID_Copyright: pszName = _T("Copyright"); break;
   case kEdsPropID_DepthOfField: pszName = _T("Depth of field"); break;
   case kEdsPropID_EFCompensation: pszName = _T("EF Compensation"); break;
   case kEdsPropID_AEModeSelect: pszName = _T("AE Mode select"); break; // new in 2.11

      // evf properties
   case kEdsPropID_Evf_OutputDevice: pszName = _T("Evf output device"); break;
   case kEdsPropID_Evf_Mode: pszName = _T("Evf mode"); break;
   case kEdsPropID_Evf_WhiteBalance: pszName = _T("Evf white balance"); break;
   case kEdsPropID_Evf_ColorTemperature: pszName = _T("Evf color temperature"); break;
   case kEdsPropID_Evf_DepthOfFieldPreview: pszName = _T("Evf depth of field preview"); break;
   case kEdsPropID_Evf_Zoom: pszName = _T("Evf zoom"); break;
   case kEdsPropID_Evf_ZoomPosition: pszName = _T("Evf zoom position"); break;
   case kEdsPropID_Evf_FocusAid: pszName = _T("Evf focus aid"); break;
   case kEdsPropID_Evf_Histogram: pszName = _T("Evf histogram"); break;
   case kEdsPropID_Evf_ImagePosition: pszName = _T("Evf image positions"); break;
   case kEdsPropID_Evf_HistogramStatus: pszName = _T("Evf histogram status"); break;
   case kEdsPropID_Evf_AFMode: pszName = _T("Evf AF mode"); break;
   case kEdsPropID_Record: pszName = _T("Evf Record"); break;
   case kEdsPropID_Evf_HistogramY: pszName = _T("Evf histogram Y"); break;
   case kEdsPropID_Evf_HistogramR: pszName = _T("Evf histogram R"); break;
   case kEdsPropID_Evf_HistogramG: pszName = _T("Evf histogram G"); break;
   case kEdsPropID_Evf_HistogramB: pszName = _T("Evf histogram B"); break;
   case kEdsPropID_Evf_CoordinateSystem: pszName = _T("Evf coordinate system"); break;
   case kEdsPropID_Evf_ZoomRect: pszName = _T("Evf zoom rect"); break;
   case kEdsPropID_Evf_ImageClipRect: pszName = _T("Evf image clip rect"); break;

      // image properties
   case kEdsPropID_ImageQuality: pszName = _T("Image quality"); break;
   case kEdsPropID_JpegQuality: pszName = _T("JPEG quality"); break;
   case kEdsPropID_Orientation: pszName = _T("Orientation"); break;
   case kEdsPropID_ICCProfile: pszName = _T("ICC profile"); break;
   case kEdsPropID_FocusInfo: pszName = _T("Focus info"); break;
   case kEdsPropID_DigitalExposure: pszName = _T("Digital exposure"); break;
   case kEdsPropID_WhiteBalance: pszName = _T("White balance"); break;
   case kEdsPropID_ColorTemperature: pszName = _T("Color temperature"); break;
   case kEdsPropID_WhiteBalanceShift: pszName = _T("White balance shift"); break;
   case kEdsPropID_Contrast: pszName = _T("Contrast"); break;
   case kEdsPropID_ColorSaturation: pszName = _T("Color saturation"); break;
   case kEdsPropID_ColorTone: pszName = _T("Color tone"); break;
   case kEdsPropID_Sharpness: pszName = _T("Sharpness"); break;
   case kEdsPropID_ColorSpace: pszName = _T("Color space"); break;
   case kEdsPropID_ToneCurve: pszName = _T("Tone curve"); break;
   case kEdsPropID_PhotoEffect: pszName = _T("Photo effect"); break;
   case kEdsPropID_FilterEffect: pszName = _T("Filter effect"); break;
   case kEdsPropID_ToningEffect: pszName = _T("Tining effect"); break;
   case kEdsPropID_ParameterSet: pszName = _T("Parameter set"); break;
   case kEdsPropID_ColorMatrix: pszName = _T("Color matrix"); break;
   case kEdsPropID_PictureStyle: pszName = _T("Picture style"); break;
   case kEdsPropID_PictureStyleDesc: pszName = _T("Picture style description"); break;
   case kEdsPropID_PictureStyleCaption: pszName = _T("Picture style caption"); break;

      // image GPS properties
   case kEdsPropID_GPSVersionID: pszName = _T("GPS version ID"); break;
   case kEdsPropID_GPSLatitudeRef: pszName = _T("GPS latitude ref"); break;
   case kEdsPropID_GPSLatitude: pszName = _T("GPS Latitude"); break;
   case kEdsPropID_GPSLongitudeRef: pszName = _T("GPS longitude ref"); break;
   case kEdsPropID_GPSLongitude: pszName = _T("GPS longitude"); break;
   case kEdsPropID_GPSAltitudeRef: pszName = _T("GPS altitude ref"); break;
   case kEdsPropID_GPSAltitude: pszName = _T("GPS altitude"); break;
   case kEdsPropID_GPSTimeStamp: pszName = _T("GPS time stamp"); break;
   case kEdsPropID_GPSSatellites: pszName = _T("GPS satellites"); break;
   case kEdsPropID_GPSStatus: pszName = _T("GPS status"); break;
   case kEdsPropID_GPSMapDatum: pszName = _T("GPS map datum"); break;
   case kEdsPropID_GPSDateStamp: pszName = _T("GPS date stamp"); break;

      // develop properties
   case kEdsPropID_Linear: pszName = _T("Linear"); break;
   case kEdsPropID_ClickWBPoint: pszName = _T("Click white balance point"); break;
   case kEdsPropID_WBCoeffs: pszName = _T("White balance coefficients"); break;

   default:
      if ((propertyId & 0xFFFF) == kEdsPropID_CFn)
      {
         switch (propertyId >> 16)
         {
            // EOS 400D
         case 0x0001: return _T("Custom function: SET button/Cross keys function");
         case 0x0002: return _T("Custom function: Long exposure noise reduction");
         case 0x0003: return _T("Custom function: Flash sync. speed in Av mode");
         case 0x0004: return _T("Custom function: Shutter/AE lock button");
         case 0x0005: return _T("Custom function: AF-assist beam");
         case 0x0006: return _T("Custom function: Exposure level increments");
         case 0x0007: return _T("Custom function: Mirror lockup");
         case 0x0008: return _T("Custom function: E-TTL II");
         case 0x0009: return _T("Custom function: Shutter curtain sync.");
         case 0x000a: return _T("Custom function: Magnified view");
         case 0x000b: return _T("Custom function: LCD display when power ON");

            // EOS 40D, all newer EOS cameras?
         case 0x0101: return _T("Custom function: Exposure level increments");
         case 0x0102: return _T("Custom function: ISO speed setting increments");
         case 0x0103: return _T("Custom function: ISO expansion");
         case 0x0104: return _T("Custom function: Bracketing auto cancel");
         case 0x0105: return _T("Custom function: Bracketing sequence");
         case 0x0108: return _T("Custom function: Safety shift");
         case 0x010f: return _T("Custom function: Flash sync.speed in Av mode");

         case 0x0201: return _T("Custom function: Long exposure noise reduction");
         case 0x0202: return _T("Custom function: High ISO speed noise reduction");
         case 0x0203: return _T("Custom function: Highlight tone priority");

         case 0x0505: return _T("Custom function: Lens drive when AF impossible");
         case 0x0506: return _T("Custom function: Lens AF stop button function");
         case 0x050f: return _T("Custom function: AF point selection method");
         case 0x0510: return _T("Custom function: Superimposed display");
         case 0x050e: return _T("Custom function: AF-assist beam firing");
         case 0x0511: return _T("Custom function: AF during live view shooting");
         case 0x060f: return _T("Custom function: Mirror Lockup");

         case 0x0701: return _T("Custom function: Shutter button/AF-ON");
         case 0x0702: return _T("Custom function: AF-ON/AE lock button switch");
         case 0x0704: return _T("Custom function: SET button while shooting");
         case 0x0706: return _T("Custom function: Dial direction during Tv/Av");
         case 0x080b: return _T("Custom function: Focusing screen");
         case 0x080f: return _T("Custom function: Add original decision data");
         case 0x0810: return _T("Custom function: Live view exposure simulation");
         case 0x0811: return _T("Custom function: Add image verification data"); // Canon EOS 550D

         default:
            return _T("Custom function: Unknown");
            break;
         }
      }

      ATLASSERT(false);
      break;
   }

   return pszName;
}

CString PropertyAccess::DisplayTextFromIdAndValue(EdsPropertyID propertyId, Variant value)
{
   CString cszText;

   // remap some property ids, since they have the same format
   switch(propertyId)
   {
   case kEdsPropID_AEModeSelect: propertyId = kEdsPropID_AEMode; break;
   case kEdsPropID_Evf_WhiteBalance: propertyId = kEdsPropID_WhiteBalance; break;
   }

   switch (propertyId)
   {
      // device properties
   case kEdsPropID_BatteryLevel:
      {
         int iValue = value.Get<int>();
         if (-1 == iValue)
            cszText = _T("AC");
         else
            cszText.Format(_T("%i%%"), iValue);
      }
      break;

   case kEdsPropID_DateTime:
      {
         std::vector<BYTE> vecData = value.GetArray<BYTE>();
         if (vecData.empty() || vecData.size() != sizeof(EdsTime))
         {
            cszText = _T("Invalid Date/Time");
            break;
         }

         EdsTime& t = *reinterpret_cast<EdsTime*>(vecData.data());

         cszText.Format(_T("%04u-%02u-%02u %02u:%02u:%02u.%03u"),
            t.year, t.month, t.day,
            t.hour, t.minute, t.second,
            t.milliseconds);
      }
      break;

   case kEdsPropID_ShutterCounter:
   case kEdsPropID_AvailableShots:
      cszText.Format(_T("%u"), value.Get<unsigned int>());
      break;

      // image properties
   case kEdsPropID_ImageQuality:
      cszText = FormatImageFormatValue(value.Get<unsigned int>());
      break;

   case kEdsPropID_ColorTemperature:
      if (value.Type() == Variant::typeInt32)
         cszText.Format(_T("%i K"), value.Get<int>());
      else
         cszText.Format(_T("%u K"), value.Get<unsigned int>());
      break;

   case kEdsPropID_WhiteBalanceShift:
      {
         std::vector<int> vecData = value.GetArray<int>();
         ATLASSERT(vecData.size() == 2);

         if (vecData.size() != 2)
            break;

         cszText.Format(_T("Amber-Blue: %i, Green-Magenta: %i"),
            vecData[0], vecData[1]);
      }
      break;

   case kEdsPropID_FocalLength: // Note: only available on EdsImageRef
      {
         std::vector<EdsRational> vecData = value.GetArray<EdsRational>();
         ATLASSERT(vecData.size() == 3);

         if (vecData.size() != 3)
            break;

         double dFocalLength = vecData[0].denominator == 0 ? 0.0 :
            vecData[0].numerator / vecData[0].denominator;

         cszText.Format(_T("%3.1f mm"), dFocalLength);
      }
      break;

      // capture properties

   case kEdsPropID_ISOSpeed:
      cszText = FormatIsoValue(value);
      break;

   case kEdsPropID_Av:
      cszText = FormatApexValue(value);
      break;

   case kEdsPropID_Tv:
      cszText = FormatShutterSpeedValue(value);
      break;

   case kEdsPropID_ExposureCompensation:
   case kEdsPropID_FlashCompensation:
      cszText = FormatCompensationValue(value, true);
      break;

   case kEdsPropID_FlashMode: // Note: only available on EdsImageRef
      {
         std::vector<unsigned int> vecData = value.GetArray<unsigned int>();
         ATLASSERT(vecData.size() == 2);

         if (vecData.size() != 2)
            break;

         cszText.Format(_T("%s, %s"),
            vecData[0] == 0 ? _T("None") :
            vecData[0] == 1 ? _T("Internal") :
            vecData[0] == 2 ? _T("External E-TTL") :
            vecData[0] == 3 ? _T("External A-TTL") :
            vecData[0] == 0xFFFFFFFF ? _T("Invalid") : _T("???"),
            vecData[1] == 0 ? _T("1st curtain") :
            vecData[1] == 1 ? _T("2nd curtain") :
            vecData[1] == 0xFFFFFFFF ? _T("Invalid") : _T("???"));
      }
      break;

      // viewfinder properties

   case kEdsPropID_Evf_ColorTemperature:
      cszText.Format(_T("%u K"), value.Get<unsigned int>());
      break;

   default:
      if (!FormatValueById(g_aPropIdDisplayInfo, sizeof(g_aPropIdDisplayInfo)/sizeof(*g_aPropIdDisplayInfo),
            propertyId, value, cszText))
         cszText = value.ToString();
      break;
   }

   return cszText;
}
