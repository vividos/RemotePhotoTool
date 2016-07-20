//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file DeviceProperty.hpp Canon control - Device property
//
#pragma once

// includes
#include "Variant.hpp"
#include "ImplFwd.hpp"

/// device property
class DeviceProperty
{
public:
   /// device property id
   unsigned int Id() const throw() { return m_uiPropertyId; }

   /// property name
   CString Name() const throw();

   /// property value as string
   CString AsString() const throw();

   /// value as variant
   Variant Value() const { return m_value; }

   /// returns if property is read-only
   bool IsReadOnly() const throw() { return m_bReadOnly; }

   /// returns valid values as list of variants
   const std::vector<Variant>& ValidValues() const { return m_vecValidValues; }

   /// converts a given value (e.g. from ValidValues()) to string
   CString ValueAsString(Variant value) const throw();

private:
   friend EDSDK::SourceDeviceImpl;
   friend CDSDK::SourceDeviceImpl;
   friend PSREC::SourceDeviceImpl;
   friend GPhoto2::PropertyAccess;
   friend RemotePhotoTool::CameraControl::DeviceProperty;

   /// ctor
   DeviceProperty(T_enSDKVariant enSDKVariant, unsigned int uiPropertyId, Variant value, bool bReadOnly,
      std::shared_ptr<void> spImpl = std::shared_ptr<void>())
      :m_enSDKVariant(enSDKVariant),
       m_uiPropertyId(uiPropertyId),
       m_value(value),
       m_bReadOnly(bReadOnly),
       m_spImpl(spImpl)
   {
   }

   /// returns valid values as list of variants; non-const version
   std::vector<Variant>& ValidValues() { return m_vecValidValues; }

   /// constructs a new device property based on this property, with another value
   DeviceProperty PropertyFromVariantValue(const Variant& value)
   {
      return DeviceProperty(m_enSDKVariant, m_uiPropertyId, value, m_bReadOnly, m_spImpl);
   }

private:
   /// SDK variant
   T_enSDKVariant m_enSDKVariant;

   /// device property id
   unsigned int m_uiPropertyId;

   /// value
   Variant m_value;

   /// read only property?
   bool m_bReadOnly;

   /// valid values
   std::vector<Variant> m_vecValidValues;

   /// pointer to implementation; may be null
   std::shared_ptr<void> m_spImpl;
};
