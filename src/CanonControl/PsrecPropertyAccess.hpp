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

/// dummy property value for getting available shots
const prUInt16 PSREC_PROP_AVAILABLE_SHOTS = 0x1001;

/// device value parser
struct DeviceValueParser: public VarDataParser
{
   /// reads next value from buffer
   Variant ReadValue(prUInt16 uiDataType);
};


/// access to device property description
struct DevicePropDesc: public DeviceValueParser
{
public:
   /// ctor; reads property description from camera
   DevicePropDesc(prHandle hCamera, prUInt16 propId, bool bParseValues = false);

   /// parses property description
   void Parse();

   /// returns if property can be written to
   bool IsSetAllowed() const throw()
   {
      // 0x00 means read-only
      // 0x01 means read-write
      return m_uiGetSet != 0x00;
   }

   /// creates value data from variant
   void CreateValueData(Variant v, std::vector<BYTE>& vecValueData);

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
   Variant Get(prUInt16 propId) const;

   /// sets property
   void Set(prUInt16 propId, Variant v);

   /// enumerates all values for given property
   void Enum(prUInt16 propId, std::vector<Variant>& vecValues)
   {
      DevicePropDesc desc(m_hCamera, propId, true);
      vecValues = desc.m_vecAllValues;
   }

   /// returns value for image format property
   Variant GetImageFormatProperty() const;

   /// converts from property id to property name
   static LPCTSTR NameFromId(prUInt16 propertyId) throw();

   /// formats display text from id and value
   static CString DisplayTextFromIdAndValue(prUInt16 propertyId, Variant value);

   /// formats image format value
   static CString FormatImageFormatValue(const Variant& value);

   /// formats property "supported size" property
   static CString FormatPropSupportedSize(const Variant& value);

private:
   /// camera handle
   prHandle m_hCamera;
};

} // namespace PSREC
