//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file ImageProperty.cpp Canon control - Image property
//

// includes
#include "stdafx.h"
#include "ImageProperty.hpp"
#include "CDSDK\CdsdkImagePropertyAccess.hpp"
#include "EDSDK\EdsdkPropertyAccess.hpp"
#include "PSREC\PsrecPropertyAccess.hpp"
#include "gPhoto2\GPhoto2PropertyAccess.hpp"

CString ImageProperty::Name() const throw()
{
   try
   {
      CString cszName = _T("???");

      switch(m_enSDKVariant)
      {
      case variantCdsdk: cszName = CDSDK::ImagePropertyAccess::NameFromId(m_uiImageProperty); break;
      case variantEdsdk: cszName = EDSDK::PropertyAccess::NameFromId(m_uiImageProperty); break;
      case variantPsrec: cszName = PSREC::PropertyAccess::NameFromId(static_cast<prUInt16>(m_uiImageProperty & 0xFFFF)); break;
      case variantGphoto2: cszName = GPhoto2::PropertyAccess::NameFromId(m_uiImageProperty); break;
      default:
         ATLASSERT(false);
         LOG_TRACE(_T("invalid SDK variant in DeviceProperty::Name()\n"));
         break;
      }

      return cszName;
   }
   catch (...)
   {
      LOG_TRACE(_T("unknown exception in ImageProperty::Name()\n"));
      return CString();
   }
}

CString ImageProperty::AsString() const throw()
{
   return ValueAsString(m_value);
}

CString ImageProperty::ValueAsString(Variant value) const throw()
{
   try
   {
      switch(m_enSDKVariant)
      {
      case variantCdsdk: return CDSDK::ImagePropertyAccess::DisplayTextFromIdAndValue(m_uiImageProperty, value);
      case variantEdsdk: return EDSDK::PropertyAccess::DisplayTextFromIdAndValue(m_uiImageProperty, value);
      case variantPsrec: return PSREC::PropertyAccess::DisplayTextFromIdAndValue(static_cast<prUInt16>(m_uiImageProperty & 0xFFFF), value);
      case variantGphoto2: return GPhoto2::PropertyAccess::DisplayTextFromIdAndValue(m_uiImageProperty, value);
      default:
         ATLASSERT(false);
         LOG_TRACE(_T("invalid SDK variant in ImageProperty::ValueAsString()\n"));
         return CString(_T("???"));
      }
   }
   catch (const CameraException& ex)
   {
      LOG_TRACE(_T("camera exception in ImageProperty::ValueAsString(): %s\n"), ex.Message().GetString());
      return CString();
   }
   catch (const boost::bad_any_cast&)
   {
      LOG_TRACE(_T("bad_any_cast exception in ImageProperty::ValueAsString()\n"));
      return CString();
   }
   catch (...)
   {
      LOG_TRACE(_T("unknown exception in ImageProperty::ValueAsString()\n"));
      return CString();
   }
}
