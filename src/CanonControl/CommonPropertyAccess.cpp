//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CommonPropertyAccess.cpp Common property access functions
//

// includes
#include "stdafx.h"
#include "CommonPropertyAccess.hpp"
#include <ctime>

unsigned int GetUnsignedIntValue(const Variant& value)
{
   unsigned int uiValue = 0;

   if (value.Type() == Variant::typeUInt8)
      uiValue = value.Get<unsigned char>();
   else
   if (value.Type() == Variant::typeUInt16)
      uiValue = value.Get<unsigned short>();
   else
   if (value.Type() == Variant::typeUInt32)
      uiValue = value.Get<unsigned int>();
   else
   if (value.Type() == Variant::typeInt32)
      uiValue = static_cast<unsigned int>(value.Get<int>());
   else
   if (value.Type() == Variant::typeInvalid)
      uiValue = 0;
   else
      ATLASSERT(false);

   return uiValue;
}

bool FormatValueById(PropIdDisplayInfo* aDispInfo, size_t uiDispInfoSize, unsigned int propertyId, Variant value, CString& cszValue)
{
   for (size_t i=0; i<uiDispInfoSize; i++)
   {
      if (aDispInfo[i].propertyId != propertyId)
         continue;

      const PropIdPossibleValues* possibleValues = aDispInfo[i].possibleValues;

      unsigned int uiValue = GetUnsignedIntValue(value);

      for (size_t j=0; possibleValues[j].pszText != nullptr; j++)
      {
         if (uiValue == possibleValues[j].uiValue)
         {
            cszValue = possibleValues[j].pszText;
            break;
         }
      }

      if (cszValue.IsEmpty())
         cszValue.Format(_T("??? (0x%08x)"), uiValue);

      return true;
   }

   return false;
}

CString FormatApexValue(Variant value)
{
   unsigned int uiAperture = GetUnsignedIntValue(value);

   // special values
   switch(uiAperture)
   {
   case 0x0000: return _T("No lens");
   case 0xffff: return _T("N/A");
   case 0x7fff: return _T("Open");
   case 0x7ffe: return _T("Av max");

      // values that would be calculated wrong by below algorithm
   case 0x25: return _T("f/3.5");
   case 0x33: return _T("f/6.3");
   case 0x43: return _T("f/13");
   }

   if (uiAperture >= 0x08 &&
      uiAperture <= 0x70)
   {
      bool bWithFract = uiAperture <= 0x3c; // 0x3c == f/9.5

      // note: adding 0x10 to value means double aperture
      // exp: 0x08 f/1.0 --> exp = 0
      // exp: 0x18 f/2.0 --> exp = 1
      unsigned int uiExp = (((uiAperture - 8) & 0xf0) / 0x10);

      // 2^0 = f/1, 2^1 = f/2, 2^6 = f/64
      double dAperture = std::pow(2.0, double(uiExp));

      // calc factor from fraction
      double dFactor = 1.0;
      unsigned int uiApertureFract = (uiAperture - 8) & 0x0f;
      switch(uiApertureFract)
      {
      case 0: break;
      case 3: dFactor = 36.0/32.0; break;
      case 4: dFactor = 38.0/32.0; break;
      case 5: dFactor = 40.0/32.0; break;
      case 8: dFactor = 45.0/32.0; break;
      case 11: dFactor = 51.0/32.0; break;
      case 12: dFactor = 54.0/32.0; break;
      case 13: dFactor = 57.0/32.0; break;
      default:
         ATLASSERT(false);
         break;
      }

      dAperture *= dFactor;

      // round after first digit
      dAperture += 0.05;

      unsigned int uiApertureInt = static_cast<unsigned int>(dAperture);

      CString cszAperture;
      cszAperture.Format(bWithFract ? _T("f/%u.%u") : _T("f/%u"),
         uiApertureInt,
         static_cast<unsigned int>((dAperture - uiApertureInt) * 10.0)
      );

      return cszAperture;
   }

   return _T("???");
}

CString FormatShutterSpeedValue(Variant value)
{
   unsigned int uiShutterSpeed = GetUnsignedIntValue(value);

   switch(uiShutterSpeed)
   {
   case 0x04:
   case 0x0c: return _T("Bulb");
   case 0x10: return _T("30\"");
   case 0x13: return _T("25\"");
   case 0x14: return _T("20\"");
   case 0x15: return _T("20\"");
   case 0x18: return _T("15\"");
   case 0x1b: return _T("13\"");
   case 0x1c: return _T("10\"");
   case 0x1d: return _T("10\"");
   case 0x20: return _T("8\"");
   case 0x23: return _T("6\"");
   case 0x24: return _T("6\"");
   case 0x25: return _T("5\"");
   case 0x28: return _T("4\"");
   case 0x2b: return _T("3\"2");
   case 0x2c: return _T("3\"");
   case 0x2d: return _T("2\"5");
   case 0x30: return _T("2\"");
   case 0x33: return _T("1\"6");
   case 0x34: return _T("1\"5");
   case 0x35: return _T("1\"3");
   case 0x38: return _T("1\"");
   case 0x3b: return _T("0\"8");
   case 0x3c: return _T("0\"7");
   case 0x3d: return _T("0\"6");
   case 0x40: return _T("0\"5");
   case 0x43: return _T("0\"4");
   case 0x44: return _T("0\"3");
   case 0x45: return _T("0\"3");
   case 0x48: return _T("1/4");
   case 0x4b: return _T("1/5");
   case 0x4c: return _T("1/6");
   case 0x4d: return _T("1/6");
   case 0x50: return _T("1/8");
   case 0x53: return _T("1/10");
   case 0x54: return _T("1/10");
   case 0x55: return _T("1/13");
   case 0x58: return _T("1/15");
   case 0x5b: return _T("1/20");
   case 0x5c: return _T("1/20");
   case 0x5d: return _T("1/25");
   case 0x60: return _T("1/30");
   case 0x63: return _T("1/40");
   case 0x64: return _T("1/45");
   case 0x65: return _T("1/50");
   case 0x68: return _T("1/60");
   case 0x6b: return _T("1/80");
   case 0x6c: return _T("1/90");
   case 0x6d: return _T("1/100");
   case 0x70: return _T("1/125");
   case 0x73: return _T("1/160");
   case 0x74: return _T("1/180");
   case 0x75: return _T("1/200");
   case 0x78: return _T("1/250");
   case 0x7b: return _T("1/320");
   case 0x7c: return _T("1/350");
   case 0x7d: return _T("1/400");
   case 0x80: return _T("1/500");
   case 0x83: return _T("1/640");
   case 0x84: return _T("1/750");
   case 0x85: return _T("1/800");
   case 0x88: return _T("1/1000");
   case 0x8b: return _T("1/1250");
   case 0x8c: return _T("1/1500");
   case 0x8d: return _T("1/1600");
   case 0x90: return _T("1/2000");
   case 0x93: return _T("1/2500");
   case 0x94: return _T("1/3000");
   case 0x95: return _T("1/3200");
   case 0x98: return _T("1/4000");
   case 0x9b: return _T("1/5000");
   case 0x9c: return _T("1/6000");
   case 0x9d: return _T("1/64000");
   case 0xa0: return _T("1/8000");
   case 0:
   case 0xffff: return _T("N/A");

   default:
      ATLASSERT(false);
      break;
   }

   return _T("???");
}

CString FormatIsoValue(Variant value)
{
   unsigned int uiIso = GetUnsignedIntValue(value);

   switch(uiIso)
   {
   case 0x00: return _T("Auto");
   case 0x28: return _T("6");
   case 0x2b: return _T("8");
   case 0x2d: return _T("10");
   case 0x30: return _T("12");
   case 0x33: return _T("16");
   case 0x35: return _T("20");
   case 0x38: return _T("25");
   case 0x3b: return _T("32");
   case 0x3d: return _T("40");
   case 0x40: return _T("50");
   case 0x43: return _T("64");
   case 0x45: return _T("80");
   case 0xff:
   case 0xffff:
   case 0xffffffff:
      return _T("Invalid");
   default:
      // 0x48 => ISO 100
      // 0x98 => ISO 102400
      if (uiIso >= 0x48 && uiIso <= 0x98)
      {
         ATLASSERT((uiIso & 7) == 0); // must be divisable by 8

         unsigned int uiIsoValue = 100;
         unsigned int uiFactor = ((uiIso - 0x48) / 8);
         uiIsoValue *= 1 << uiFactor;

         CString cszValue;
         cszValue.Format(_T("%u"), uiIsoValue);
         return cszValue;
      }
      ATLASSERT(false);
      break;
   }

   return _T("???");
}

CString FormatCompensationValue(Variant value, bool bIsEdsdk)
{
   // PSREC: +3 EV .. -3 EV <=> 0x00 .. 0x30
   // CDSDK: +3 EV .. -3 EV <=> 0x00 .. 0x30
   // EDSDK: +3 EV .. -3 EV <=> 0x18 .. 0x00, 0xff .. 0xe8
   // PSREC is the same as CDSDK
   // EDSDK = 0x18 - CDSDK

   unsigned int uiComp = GetUnsignedIntValue(value);

   if (!bIsEdsdk && uiComp == 0xff)
      return _T("N/A");

   int iValue = bIsEdsdk ? int(char(uiComp)) : 0x18 - int(uiComp);
   if (iValue == 0)
      return _T("0");

   int iFract = std::abs(iValue) & 7; // first 3 bits
   int iInt = std::abs(iValue) / 8;

   CString cszFract =
      iFract == 4 ? _T("1/2") :
      iFract == 3 ? _T("1/3") :
      iFract == 5 ? _T("2/3") : _T("???");

   TCHAR cSign = iValue < 0 ? _T('-') : _T('+');

   CString cszValue;

   if (iInt == 0)
      cszValue.Format(_T("%c%s"), cSign, cszFract.GetString());
   else
   {
      cszValue.Format(
         iFract == 0 ? _T("%c%u") : _T("%c%u (%s)"),
         cSign,
         iInt,
         cszFract);
   }

   return cszValue;
}

CString FormatDateTime32bit(const Variant& value)
{
   unsigned int uiTime = GetUnsignedIntValue(value);

   FILETIME fileTime = { 0 };
   BOOL bRet = DosDateTimeToFileTime(HIWORD(uiTime), LOWORD(uiTime), &fileTime);
   if (bRet != TRUE)
      return _T("N/A");

   SYSTEMTIME systemTime = { 0 };
   bRet = FileTimeToSystemTime(&fileTime, &systemTime);
   if (bRet != TRUE)
      return _T("N/A");

   CString cszDate;
   GetDateFormat(LOCALE_USER_DEFAULT, 0, &systemTime, _T("yyyy'-'MM'-'dd"), cszDate.GetBuffer(32), 32);
   cszDate.ReleaseBuffer();

   CString cszTime;
   GetTimeFormat(LOCALE_USER_DEFAULT, 0, &systemTime, _T(" HH':'mm':'ss"), cszTime.GetBuffer(32), 32);
   cszTime.ReleaseBuffer();

   return cszDate + cszTime;
}

CString FormatDateTimeEpochUTC(const Variant& value)
{
   __time64_t time = GetUnsignedIntValue(value);

   struct tm tmtime = { 0 };
   errno_t err = ::_localtime64_s(&tmtime, &time);
   if (err != 0)
      return _T("invalid date/time");

   CString cszText;
   _tcsftime(cszText.GetBuffer(32), 32, _T("%Y-%m-%d %H:%M:%S"), &tmtime);
   cszText.ReleaseBuffer();

   return cszText;
}
