//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file DeviceProperty.cpp Canon control - Device property
//

// includes
#include "stdafx.h"
#include "DeviceProperty.hpp"
#include "CdsdkDevicePropertyAccess.hpp"
#include "EdsdkPropertyAccess.hpp"
#include "PsrecPropertyAccess.hpp"

CString DeviceProperty::Name() const throw()
{
   switch(m_enSDKVariant)
   {
   case variantCdsdk: return CDSDK::DevicePropertyAccess::NameFromId(m_uiPropertyId);
   case variantEdsdk: return EDSDK::PropertyAccess::NameFromId(static_cast<EdsPropertyID>(m_uiPropertyId));
   case variantPsrec: return PSREC::PropertyAccess::NameFromId(static_cast<prUInt16>(m_uiPropertyId & 0xFFFF));
   default:
      ATLASSERT(false);
      return CString(_T("???"));
   }
}

CString DeviceProperty::AsString() const throw()
{
   return ValueAsString(m_value);
}

CString DeviceProperty::ValueAsString(Variant value) const throw()
{
   try
   {
      switch(m_enSDKVariant)
      {
      case variantCdsdk: return CDSDK::DevicePropertyAccess::DisplayTextFromIdAndValue(m_uiPropertyId, value);
      case variantEdsdk: return EDSDK::PropertyAccess::DisplayTextFromIdAndValue(static_cast<EdsPropertyID>(m_uiPropertyId), value);
      case variantPsrec: return PSREC::PropertyAccess::DisplayTextFromIdAndValue(static_cast<prUInt16>(m_uiPropertyId & 0xFFFF), value);
      default:
         ATLASSERT(false);
         return CString(_T("???"));
      }
   }
   catch(const CameraException& ex)
   {
      return CString(_T("exception during value formatting: ") + ex.Message());
   }
}
