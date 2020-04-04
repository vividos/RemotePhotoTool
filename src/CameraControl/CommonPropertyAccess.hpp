//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CommonPropertyAccess.hpp Common property access functions
//
#pragma once

// includes
#include "Variant.hpp"

/// struct with possible value and text, for a single property value
struct PropIdPossibleValues
{
   /// possible value for property
   unsigned int uiValue;

   /// description of property value
   LPCTSTR pszText;
};

/// struct with all possible property values for a single property
struct PropIdDisplayInfo
{
   /// propety id
   unsigned int propertyId;

   /// possible values for property id
   PropIdPossibleValues possibleValues[200];
};

/// finds text for property id and given value
bool FormatValueById(PropIdDisplayInfo* aDispInfo, size_t uiDispInfoSize, unsigned int propertyId, Variant value, CString& cszValue);

/// returns unsigned integer value from variant
unsigned int GetUnsignedIntValue(const Variant& value);

/// formats aperture value
CString FormatApexValue(Variant value);

/// formats shutter speed value
CString FormatShutterSpeedValue(Variant value);

/// formats iso value
CString FormatIsoValue(Variant value);

/// formats compensation value (exposure, flash, etc.)
CString FormatCompensationValue(Variant value, bool bIsEdsdk);

/// formats 32 bit date/time format
CString FormatDateTime32bit(const Variant& value);

/// formats date/time as seconds since epoch (1970-01-01 00:00:00) in UTC
CString FormatDateTimeEpochUTC(const Variant& value);
