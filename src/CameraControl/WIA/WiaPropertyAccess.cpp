//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file WiaPropertyAccess.cpp WIA - Property access
//
#include "stdafx.h"
#include "WiaPropertyAccess.hpp"
#include <propvarutil.h>

#pragma comment(lib, "propsys.lib")

using namespace WIA;

CString PropertyAccess::Get(PROPID wiaPropertyId) const
{
   CComQIPtr<IWiaPropertyStorage> wiaPropertyStorage{ m_item };

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

UINT PropertyAccess::GetUint(PROPID wiaPropertyId) const
{
   CComQIPtr<IWiaPropertyStorage> wiaPropertyStorage{ m_item };

   if (wiaPropertyStorage == nullptr)
      return 0;

   PROPSPEC propSpec = { 0 };
   PROPVARIANT propVariant = { 0 };

   propSpec.ulKind = PRSPEC_PROPID;
   propSpec.propid = wiaPropertyId;

   HRESULT hr = wiaPropertyStorage->ReadMultiple(1, &propSpec, &propVariant);

   UINT value = 0;
   if (SUCCEEDED(hr))
      value = propVariant.lVal;

   FreePropVariantArray(1, &propVariant);

   return value;
}

time_t PropertyAccess::GetSystemTime(PROPID wiaPropertyId) const
{
   CComQIPtr<IWiaPropertyStorage> wiaPropertyStorage{ m_item };

   if (wiaPropertyStorage == nullptr)
      return 0;

   PROPSPEC propSpec = { 0 };
   PROPVARIANT propVariant = { 0 };

   propSpec.ulKind = PRSPEC_PROPID;
   propSpec.propid = wiaPropertyId;

   HRESULT hr = wiaPropertyStorage->ReadMultiple(1, &propSpec, &propVariant);

   SYSTEMTIME st = {};
   if (SUCCEEDED(hr) &&
      IsPropVariantVector(propVariant) &&
      PropVariantGetElementCount(propVariant) == 8)
   {
      PropVariantGetUInt16Elem(propVariant, 0, &st.wYear);
      PropVariantGetUInt16Elem(propVariant, 1, &st.wMonth);
      PropVariantGetUInt16Elem(propVariant, 2, &st.wDayOfWeek);
      PropVariantGetUInt16Elem(propVariant, 3, &st.wDay);
      PropVariantGetUInt16Elem(propVariant, 4, &st.wHour);
      PropVariantGetUInt16Elem(propVariant, 5, &st.wMinute);
      PropVariantGetUInt16Elem(propVariant, 6, &st.wSecond);
      PropVariantGetUInt16Elem(propVariant, 7, &st.wMilliseconds);
   }

   FreePropVariantArray(1, &propVariant);

   std::tm tm;
   tm.tm_sec = st.wSecond;
   tm.tm_min = st.wMinute;
   tm.tm_hour = st.wHour;
   tm.tm_mday = st.wDay;
   tm.tm_mon = st.wMonth - 1;
   tm.tm_year = st.wYear - 1900;
   tm.tm_isdst = -1;

   return std::mktime(&tm);
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
