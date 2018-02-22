//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file RegEnumKey.hpp Registry key enumeration class
//
#pragma once

#include <boost/noncopyable.hpp>

/// class to enumerate reg keys
class CRegEnumKey : public boost::noncopyable
{
public:
   /// ctor; takes reg key object
   CRegEnumKey(CRegKey& key)
      :m_key(key),
      m_currentKeyIndex(0)
   {
   }

   /// gets next registry key name
   bool Next(CString& name)
   {
      int bufferLength = 256;
      LONG result;

      do
      {
         result = ::RegEnumKey(m_key, m_currentKeyIndex, name.GetBuffer(bufferLength), bufferLength);
         name.ReleaseBuffer();

         if (result == ERROR_MORE_DATA)
            bufferLength *= 2;

      } while (result == ERROR_MORE_DATA);

      ATLASSERT(result == ERROR_SUCCESS || result == ERROR_NO_MORE_ITEMS);

      m_currentKeyIndex++;
      return result != ERROR_NO_MORE_ITEMS;
   }

   /// opens next sub key
   bool NextKey(CRegKey& subKey, REGSAM samDesired = KEY_ALL_ACCESS)
   {
      CString keyName;
      bool result = Next(keyName);
      if (result)
         subKey.Open(m_key, keyName, samDesired);

      return result;
   }

private:
   CRegKey & m_key;      ///< registry key to enumerate
   DWORD m_currentKeyIndex;  ///< index of current key
};
