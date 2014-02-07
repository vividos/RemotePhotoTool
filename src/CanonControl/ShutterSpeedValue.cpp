//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ShutterSpeedValue.cpp Canon control - Shutter speed value class
//

// includes
#include "stdafx.h"
#include "ShutterSpeedValue.hpp"
#include "RemoteReleaseControl.hpp"

void ShutterSpeedValue::Subtract2EV()
{
   // adding 8 subtracts 2 EV; 0x48 (1/4) => 0x50 (1/8)
   unsigned int uiValue = RawValue() + 8;

   SetAndCheckNewRaw(uiValue);
}

void ShutterSpeedValue::Add2EV()
{
   // subtracting 8 adds 2 EV; 0x48 (1/4) => 0x40 (0"5)
   unsigned int uiValue = RawValue() - 8;

   SetAndCheckNewRaw(uiValue);
}

unsigned int ShutterSpeedValue::RawValue() const
{
   unsigned int uiRawValue = 0;
   if (m_value.Value().Type() == Variant::typeUInt16)
      uiRawValue = m_value.Value().Get<unsigned short>();
   else
   if (m_value.Value().Type() == Variant::typeUInt32)
      uiRawValue = m_value.Value().Get<unsigned int>();
   else
      ATLASSERT(false);

   return uiRawValue;
}

void ShutterSpeedValue::SetAndCheckNewRaw(unsigned int uiValue)
{
   // set...
   Variant newVal;
   newVal.SetType(m_value.Value().Type());

   if (m_value.Value().Type() == Variant::typeUInt16)
      newVal.Set<unsigned short>(uiValue & 0xffff);
   else
   if (m_value.Value().Type() == Variant::typeUInt32)
      newVal.Set<unsigned int>(uiValue);
   else
      ATLASSERT(false);

   ImageProperty newProp = m_value;
   newProp.SetValue(newVal);

   // ... and check if value is available
   std::vector<ImageProperty> vecValues;
   m_spRemoteReleaseControl->EnumImagePropertyValues(m_value.Id(), vecValues);

   std::vector<ImageProperty>::iterator iterValues = std::find_if(vecValues.begin(), vecValues.end(),
      [uiValue](const ImageProperty& prop)
      {
         unsigned int uiRawValue = 0;
         if (prop.Value().Type() == Variant::typeUInt16)
            uiRawValue = prop.Value().Get<unsigned short>();
         else
         if (prop.Value().Type() == Variant::typeUInt32)
            uiRawValue = prop.Value().Get<unsigned int>();
         else
            ATLASSERT(false);

         return uiRawValue == uiValue;
      });

   if (iterValues == vecValues.end())
      return; // not found; don't set it

   // valid value; set it
   m_value = newProp;
}
