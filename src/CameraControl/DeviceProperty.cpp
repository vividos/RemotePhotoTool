//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file DeviceProperty.cpp Canon control - Device property
//

// includes
#include "stdafx.h"
#include "DeviceProperty.hpp"
#include "CDSDK/CdsdkDevicePropertyAccess.hpp"
#include "EDSDK/EdsdkPropertyAccess.hpp"
#include "PSREC/PsrecPropertyAccess.hpp"
#include "gPhoto2/GPhoto2PropertyAccess.hpp"
#include "gPhoto2/GPhoto2SourceDeviceImpl.hpp"

CString DeviceProperty::Name() const
{
   try
   {
      CString cszName = _T("???");

      switch(m_enSDKVariant)
      {
      case variantCdsdk: cszName = CDSDK::DevicePropertyAccess::NameFromId(m_uiPropertyId); break;
      case variantEdsdk: cszName = EDSDK::PropertyAccess::NameFromId(static_cast<EdsPropertyID>(m_uiPropertyId)); break;
      case variantPsrec: cszName = PSREC::PropertyAccess::NameFromId(static_cast<prUInt16>(m_uiPropertyId & 0xFFFF)); break;
      case variantGphoto2:
         return std::static_pointer_cast<GPhoto2::PropertyAccess>(m_spImpl)->NameFromId(m_uiPropertyId);
      default:
         ATLASSERT(false);
         LOG_TRACE(_T("invalid SDK variant in DeviceProperty::Name()\n"));
         break;
      }

      return cszName;
   }
   catch (...)
   {
         LOG_TRACE(_T("unknown exception in DeviceProperty::Name()\n"));
      return CString();
   }
}

CString DeviceProperty::AsString() const
{
   return ValueAsString(m_value);
}

CString DeviceProperty::ValueAsString(Variant value) const
{
   try
   {
      switch(m_enSDKVariant)
      {
      case variantCdsdk: return CDSDK::DevicePropertyAccess::DisplayTextFromIdAndValue(m_uiPropertyId, value);
      case variantEdsdk: return EDSDK::PropertyAccess::DisplayTextFromIdAndValue(static_cast<EdsPropertyID>(m_uiPropertyId), value);
      case variantPsrec: return PSREC::PropertyAccess::DisplayTextFromIdAndValue(static_cast<prUInt16>(m_uiPropertyId & 0xFFFF), value);
      case variantGphoto2:
         return std::static_pointer_cast<GPhoto2::PropertyAccess>(m_spImpl)->DisplayTextFromIdAndValue(m_uiPropertyId, value);
      default:
         ATLASSERT(false);
         LOG_TRACE(_T("invalid SDK variant in DeviceProperty::ValueAsString()\n"));
         return CString(_T("???"));
      }
   }
   catch (const CameraException& ex)
   {
      LOG_TRACE(_T("camera exception in DeviceProperty::ValueAsString(): %s\n"), ex.Message().GetString());
      return CString();
   }
   catch (const std::bad_any_cast&)
   {
      LOG_TRACE(_T("bad_any_cast exception in DeviceProperty::ValueAsString()\n"));
      return CString();
   }
   catch (...)
   {
      LOG_TRACE(_T("unknown exception in DeviceProperty::ValueAsString()\n"));
      return CString();
   }
}
