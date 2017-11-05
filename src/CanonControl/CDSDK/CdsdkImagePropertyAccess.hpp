//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CdsdkImagePropertyAccess.hpp CDSDK - Image property access
//
#pragma once

// includes
#include "CdsdkCommon.hpp"
#include "Variant.hpp"
#include "ImageProperty.hpp"

namespace CDSDK
{

/// image property access
class ImagePropertyAccess
{
public:
   /// ctor
   ImagePropertyAccess(cdHSource& hSource)
      :m_hSource(hSource)
   {
   }

   /// enumerates all available image property ids
   std::vector<unsigned int> EnumImageProperties() const;

   // image property access

   /// returns image property value
   Variant Get(unsigned int uiPropId) const;

   /// sets image property value
   void Set(unsigned int uiPropId, Variant val);

   /// enumerates image property values
   void Enum(unsigned int uiPropId, std::vector<Variant>& vecValues);

   /// returns if a given value is read only
   bool IsReadOnly(unsigned int uiPropId);

   // release settings access

   /// enumerates all available release settings
   void EnumAvailReleaseSettings(std::vector<unsigned int>& vecPropIds) const;

   /// retrieves release settings value
   Variant GetReleaseSetting(cdRelCamSettingID propId) const;

   /// sets release settings value
   void SetReleaseSetting(cdRelCamSettingID propId, Variant value);

   /// enumerates release settings values
   void EnumReleaseSettingValues(cdRelCamSettingID propId, std::vector<Variant>& vecValues) const;

   /// returns if release setting is read-only
   bool IsReadOnlyReleaseSetting(cdRelCamSettingID propId) const;

   // raw data

   /// stores raw data to variant value
   void SetRawCdsdk(Variant& value, unsigned int propId, const std::vector<unsigned char>& vecData) const;

   /// retrieves raw data from variant value
   void GetRawCdsdk(const Variant& value, unsigned int propId, std::vector<unsigned char>& vecData) const;

   // special camera model functions

   /// returns camera model; for the AddCameraModel*() methods
   CString CameraModel() const;

   /// adds shooting mode values for specific cameras
   void AddCameraModelShootingMode(std::vector<Variant>& vecValues);

   /// adds Av values for specific cameras
   void AddCameraModelAvValues(std::vector<Variant>& vecValues);

   /// adds Tv values for specific cameras
   void AddCameraModelTvValues(std::vector<Variant>& vecValues);

   // mappings

   /// maps property type to property id
   static unsigned int MapToPropertyID(T_enImagePropertyType enProperty);

   // formatting

   /// converts image property to name
   static LPCTSTR NameFromId(unsigned int propId);

   /// converts property value to string
   static CString DisplayTextFromIdAndValue(unsigned int propId, Variant value);

   /// formats focus point value
   static LPCTSTR FormatFocusPoint(const Variant& value);

   /// formats parameter set value
   static CString FormatParameterSet(const Variant& value);

   /// formats image format value
   static CString FormatImageFormatValue(const Variant& value);

private:
   /// camera source
   cdHSource m_hSource;
};

} // namespace CDSDK
