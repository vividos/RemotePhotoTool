//
// ulib - a collection of useful classes
// Copyright (C) 2013 Michael Fink
//
/// \file MutexLock.hpp generic mutex lock class
//
#pragma once

/// lock class for synchronisation objects
template <typename T>
class MutexLock
{
public:
   /// ctor; takes a lockable object
   MutexLock(T& mutex)
      :m_mutex(mutex)
   {
      mutex.Lock();
   }

   /// dtor; unlocks object
   ~MutexLock()
   {
      m_mutex.Unlock();
   }

private:
   /// locked object
   T& m_mutex;
};

/// class for temporarily unlocking synchronisation objects
template <typename T>
class MutexUnLocker
{
public:
   /// ctor; takes a lockable object and unlocks it
   MutexUnLocker(T& mutex)
      :m_mutex(mutex)
   {
      mutex.Unlock();
   }

   /// dtor; locks object
   ~MutexUnLocker()
   {
      m_mutex.Lock();
   }

private:
   /// unlocked object
   T& m_mutex;
};
