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
   ImagePropertyAccess(cdHSource& hSource) throw()
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

   // mappings

   /// maps property type to property id
   static unsigned int MapToPropertyID(T_enImagePropertyType enProperty) throw();

   // formatting

   /// converts image property to name
   static CString NameFromId(unsigned int propId);

   /// converts property value to string
   static CString DisplayTextFromIdAndValue(unsigned int propId, Variant value);

   /// formats image format value
   static CString FormatImageFormatValue(const Variant& value);

private:
   /// camera source
   cdHSource m_hSource;
};

} // namespace CDSDK
