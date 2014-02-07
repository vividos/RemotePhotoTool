//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CameraException.cpp Canon control - Camera exception
//

// includes
#include "stdafx.h"
#include "CameraException.hpp"

CString CameraException::FormatExceptionText(const CString& cszFunction, bool bIsSpecific, unsigned int uiComponentId, unsigned int uiErrorId)
{
   CString cszText;

   cszText.Format(_T("function %s() returned error %x (component %x, code %04x)"),
      cszFunction.GetString(),
      uiErrorId | uiComponentId | (bIsSpecific ? 0x80000000 : 0 ),
      uiComponentId >> 24,
      uiErrorId);

   return cszText;
}
