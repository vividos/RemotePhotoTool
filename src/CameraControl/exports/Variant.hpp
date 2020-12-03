//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2019 Michael Fink
//
/// \file Variant.hpp Canon control - Variant data
//
#pragma once

// includes
#include <vector>
#include <any>

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
   VariantType Type() const { return m_enType; }

   /// returns if array values are stored
   bool IsArray() const { return m_bIsArray; }

   // single value get/set

   /// sets value
   template <typename T>
   void Set(const T& val)
   {
      m_variant = val;
      m_bIsArray = false;
   }

   /// sets value; specialized for unsigned long type
   template <>
   void Set(const unsigned long& val)
   {
      static_assert(sizeof(unsigned long) == sizeof(unsigned int),
         "this only works when unsigned long and unsigned int have the same size");
      Set<unsigned int>(val);
   }

   /// returns value
   template <typename T>
   T Get() const
   {
      ATLASSERT(m_bIsArray == false);
      return std::any_cast<T>(m_variant);
   }

   /// returns string value
   template <>
   CString Get() const
   {
      ATLASSERT(m_bIsArray == false);
      return std::any_cast<CString>(m_variant);
   }

   /// returns unsigned long value
   template <>
   unsigned long Get() const
   {
      return Get<unsigned int>();
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
      return std::any_cast<std::vector<T>>(m_variant);
   }

   /// converts variant to string representation
   CString ToString() const;

   /// sets type of value
   void SetType(VariantType enType) { m_enType = enType; }

   /// formats variant type as string
   static LPCTSTR TypeAsString(VariantType vt);

   /// sets raw variant value
   void SetRaw(std::any& variant, VariantType enType, bool bIsArray)
   {
      m_variant = variant;
      m_enType = enType;
      m_bIsArray = bIsArray;
   }

   /// equality operator
   bool operator==(const Variant& rhs) const
   {
      if (m_enType == typeUInt32)
         return IsValueEqual<unsigned int>(rhs);
      else
      if (m_enType == typeUInt16)
         return IsValueEqual<unsigned short>(rhs);
      else
      if (m_enType == typeUInt8)
         return IsValueEqual<unsigned char>(rhs);
      else
      {
         ATLASSERT(false); // implement more comparison types when needed
         return false;
      }
   }

   /// compare function
   template <typename T>
   bool IsValueEqual(const Variant& rhs) const
   {
      if (m_enType != rhs.m_enType)
         return false;

      if (!m_variant.has_value() && !rhs.m_variant.has_value())
         return true; // empty variants are equal

      if ((!m_variant.has_value() && rhs.m_variant.has_value()) ||
          (m_variant.has_value() && !rhs.m_variant.has_value()))
         return false; // one of them is non-empty

      try
      {
         T left = std::any_cast<T>(m_variant);
         T right = std::any_cast<T>(rhs.m_variant);

         return left == right;
      }
      catch (const std::bad_any_cast& ex)
      {
         ATLTRACE(_T("caught std::bad_any_cast: %hs\n"), ex.what());
      }

      return false;
   }

private:
   /// variant value
   std::any m_variant;

   /// variant type
   VariantType m_enType;

   /// indicates if array is stored
   bool m_bIsArray;
};
