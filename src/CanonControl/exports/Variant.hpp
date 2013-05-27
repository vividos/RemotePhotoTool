//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file Variant.hpp Canon control - Variant data
//
#pragma once

// includes
#include <vector>
#include <boost/any.hpp>

/// variant class to hold values with dynamic type
class Variant
{
public:
   /// ctor; creates invalid type
   Variant();

   /// possible variant types
   enum VariantType
   {
      typeBool            = 1,
      typeString          = 2,
      typeInt8            = 3,
      typeUInt8           = 6,
      typeInt16           = 4,
      typeUInt16          = 7,
      typeInt32           = 8,
      typeUInt32          = 9,
      typeInt64           = 10,
      typeUInt64          = 11,
      typeFloat           = 12,
      typeDouble          = 13,
      typeByteBlock       = 14,
      typeRational        = 20,
      typePoint           = 21,
      typeRect            = 22,
      typeTime            = 23,
      typeInvalid         = -1
   };

   /// returns current type
   VariantType Type() const throw() { return m_enType; }

   /// returns if array values are stored
   bool IsArray() const throw() { return m_bIsArray; }

   // single value get/set

   /// sets value
   template <typename T>
   void Set(const T& val)
   {
      m_variant = val;
      m_bIsArray = false;
   }

   /// returns value
   template <typename T>
   T Get() const
   {
      ATLASSERT(m_bIsArray == false);
      return boost::any_cast<T>(m_variant);
   }

   /// returns string value
   template <>
   CString Get() const
   {
      ATLASSERT(m_bIsArray == false);
      return boost::any_cast<CString>(m_variant);
   }

   // array get/set

   /// sets array data
   template <typename T>
   void SetArray(const std::vector<T>& vecVal)
   {
      m_variant = vecVal;
      m_bIsArray = true;
   }

   /// returns array data
   template <typename T>
   std::vector<T> GetArray() const
   {
      ATLASSERT(m_bIsArray == true);
      return boost::any_cast<std::vector<T>>(m_variant);
   }

   /// converts variant to string representation
   CString ToString() const;

   /// sets type of value
   void SetType(VariantType enType) throw() { m_enType = enType; }

   /// formats variant type as string
   static CString TypeAsString(VariantType vt);

   /// sets raw variant value
   void SetRaw(boost::any& variant, VariantType enType, bool bIsArray)
   {
      m_variant = variant;
      m_enType = enType;
      m_bIsArray = bIsArray;
   }

private:
   /// variant value
   boost::any m_variant;

   /// variant type
   VariantType m_enType;

   /// indicates if array is stored
   bool m_bIsArray;
};
