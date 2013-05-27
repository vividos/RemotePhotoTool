//
// ulib - a collection of useful classes
// Copyright (C) 2013 Michael Fink
//
/// \file LightweightMutex.hpp lightweight mutex class
//
#pragma once

// includes
#include "MutexLock.hpp"

/// a non-recursive lockable object
class LightweightMutex
{
public:
   /// lock type
   typedef MutexLock<LightweightMutex> LockType;

   /// ctor
   LightweightMutex() throw()
   {
      InitializeCriticalSection(&m_cs);
   }

   /// dtor
   ~LightweightMutex() throw()
   {
      DeleteCriticalSection(&m_cs);
   }

private:
   /// locks mutex
   void Lock() throw()
   {
      EnterCriticalSection(&m_cs);
   }

   /// unlocks mutex
   void Unlock() throw()
   {
      LeaveCriticalSection(&m_cs);
   }

   /// friend so that Lock class can call Lock() and Unlock()
   friend MutexLock<LightweightMutex>;

private:
   /// critical section object
   CRITICAL_SECTION m_cs;
};
