//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ImageFormat.cpp Canon control - Image format
//

// includes
#include "stdafx.h"
#include "ImageFormat.hpp"
#include "EDSDK\EdsdkPropertyAccess.hpp"
#include "CDSDK\CdsdkImagePropertyAccess.hpp"
#include "PSREC\PsrecPropertyAccess.hpp"

CString ImageFormat::ToString() const
{
   switch (m_value.m_enSDKVariant)
   {
   case T_enSDKVariant::variantEdsdk:
      return EDSDK::PropertyAccess::FormatImageFormatValue(m_value.Value().Get<unsigned int>());

   case T_enSDKVariant::variantCdsdk:
      return CDSDK::ImagePropertyAccess::FormatImageFormatValue(m_value.Value());

   case T_enSDKVariant::variantPsrec:
      return PSREC::PropertyAccess::FormatImageFormatValue(m_value.Value());

   default:
      ATLASSERT(false);
      break;
   }

   return _T("???");
}
