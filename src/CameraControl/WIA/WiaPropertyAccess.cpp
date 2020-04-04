//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file WiaPropertyAccess.cpp WIA - Property access
//

// includes
#include "stdafx.h"
#include "WiaPropertyAccess.hpp"
#include <propvarutil.h>

#pragma comment(lib, "propsys.lib")

using namespace WIA;

CString PropertyAccess::Get(PROPID wiaPropertyId) const
{
   CComQIPtr<IWiaPropertyStorage> wiaPropertyStorage(m_item);

   if (wiaPropertyStorage == nullptr)
      return CString();

   PROPSPEC propSpec = { 0 };
   PROPVARIANT propVariant = { 0 };

   propSpec.ulKind = PRSPEC_PROPID;
   propSpec.propid = wiaPropertyId;

   HRESULT hr = wiaPropertyStorage->ReadMultiple(1, &propSpec, &propVariant);

   CString text;
   if (SUCCEEDED(hr))
      text = StringFromPropVariant(propVariant);

   FreePropVariantArray(1, &propVariant);

   return text;
}

CString PropertyAccess::StringFromPropVariant(const PROPVARIANT& propVariant, ULONG elementIndex)
{
   CString text;

   PWSTR pszValue;
   HRESULT hr = PropVariantGetStringElem(propVariant, elementIndex, &pszValue);

   if (SUCCEEDED(hr))
   {
      text = pszValue;
      CoTaskMemFree(pszValue);
   }

   return text;
}
