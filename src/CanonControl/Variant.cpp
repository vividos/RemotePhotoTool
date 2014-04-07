//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file Variant.cpp Canon control - Variant data
//

// includes
#include "StdAfx.h"
#include "Variant.hpp"

Variant::Variant() throw()
:m_enType(typeInvalid),
 m_bIsArray(false)
{
}

CString Variant::ToString() const
{
   CString cszValue;

   if (m_bIsArray)
   {
      switch (m_enType)
      {
      case typeUInt8:
         {
            CString cszTemp;
            std::vector<unsigned char> vecData = GetArray<unsigned char>();
            for (size_t i=0, iMax = vecData.size(); i<iMax; i++)
            {
               cszTemp.Format(_T("%s%02x"), i == 0 ? _T("") : _T(" "), vecData[i]);
               cszValue += cszTemp;
            }
         }
         break;

      case typeUInt16:
         {
            CString cszTemp;
            std::vector<unsigned short> vecData = GetArray<unsigned short>();
            for (size_t i = 0, iMax = vecData.size(); i<iMax; i++)
            {
               cszTemp.Format(_T("%s%04x"), i == 0 ? _T("") : _T(" "), vecData[i]);
               cszValue += cszTemp;
            }
         }
         break;

      case typeUInt32:
         {
            CString cszTemp;
            std::vector<unsigned int> vecData = GetArray<unsigned int>();
            for (size_t i=0, iMax = vecData.size(); i<iMax; i++)
            {
               cszTemp.Format(_T("%s%08x"), i == 0 ? _T("") : _T(" "), vecData[i]);
               cszValue += cszTemp;
            }
         }
         break;

      case typeInt32:
         {
            CString cszTemp;
            std::vector<int> vecData = GetArray<int>();
            for (size_t i=0, iMax = vecData.size(); i<iMax; i++)
            {
               cszTemp.Format(_T("%s%i"), i == 0 ? _T("") : _T(" "), vecData[i]);
               cszValue += cszTemp;
            }
         }
         break;
      default:
         ATLASSERT(false);
         cszValue = _T("??? (array)");
         break;
      }
   }
   else
   switch (m_enType)
   {
   case typeBool:
      cszValue = Get<bool>() ? _T("yes") : _T("no");
      break;

   case typeString:
      cszValue = Get<CString>();
      break;

   case typeInt8:
      cszValue.Format(_T("%i"), Get<char>());
      break;

   case typeUInt8:
      cszValue.Format(_T("%02x"), Get<unsigned char>());
      break;

   case typeInt16:
      cszValue.Format(_T("%i"), Get<short>());
      break;

   case typeUInt16:
      cszValue.Format(_T("%04x"), Get<unsigned short>());
      break;

   case typeInt32:
      cszValue.Format(_T("%i"), Get<int>());
      break;

   case typeUInt32:
      cszValue.Format(_T("%08x"), Get<unsigned int>());
      break;

   case typeDouble:
      cszValue.Format(_T("%f"), Get<double>());
      break;

   case typeInvalid:
      cszValue = _T("invalid type");
      break;

   case typeInt64:
   case typeUInt64:
   case typeFloat:
   case typeByteBlock:
   case typeRational:
   case typePoint:
   case typeRect:
   case typeTime:
      // TODO implement

   default:
      ATLASSERT(false);
      cszValue = _T("???");
      break;
   }
   return cszValue;
}

CString Variant::TypeAsString(VariantType vt) throw()
{
   switch(vt)
   {
   case typeBool:      return _T("Bool");
   case typeString:    return _T("String");
   case typeInt8:      return _T("Int8");
   case typeUInt8:     return _T("UInt8");
   case typeInt16:     return _T("Int16");
   case typeUInt16:    return _T("UInt16");
   case typeInt32:     return _T("Int32");
   case typeUInt32:    return _T("UInt32");
   case typeInt64:     return _T("Int64");
   case typeUInt64:    return _T("UInt64");
   case typeFloat:     return _T("Float");
   case typeDouble:    return _T("Double");
   case typeByteBlock: return _T("ByteBlock");
   case typeRational:  return _T("Rational");
   case typePoint:     return _T("Point");
   case typeRect:      return _T("Rect");
   case typeTime:      return _T("Time");
   case typeInvalid:   return _T("Invalid");
   default:
      ATLASSERT(false);
      return _T("???");
   }
}
