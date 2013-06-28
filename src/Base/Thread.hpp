//
// ulib - a collection of useful classes
// Copyright (C) 2006,2007,2008,2012,2013 Michael Fink
//
/// \file Thread.hpp Thread class
//
#pragma once

// includes
#include "Event.hpp"

#ifndef MS_VC_EXCEPTION
#define MS_VC_EXCEPTION 0x406D1388
#endif

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType;     ///< Must be 0x1000.
   LPCSTR szName;    ///< Pointer to name (in user addr space).
   DWORD dwThreadID; ///< Thread ID (-1=caller thread).
   DWORD dwFlags;    ///< Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)


/// thread helper class
class Thread
{
public:
   /// sets thread name for current or specified thread
   /// \note from http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
   static void SetName(LPCTSTR pszThreadName, DWORD dwThreadId = DWORD(-1))
   {
#ifdef WIN32
      USES_CONVERSION;

      THREADNAME_INFO info;
      info.dwType = 0x1000;
      info.szName = T2CA(pszThreadName);
      info.dwThreadID = dwThreadId;
      info.dwFlags = 0;

      __try
      {
         RaiseException(MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info);
      }
      __except(EXCEPTION_EXECUTE_HANDLER)
      {
      }
#endif // WIN32
   }

   /// returns current thread id
   static DWORD CurrentId()
   {
      return ::GetCurrentThreadId();
   }
};
