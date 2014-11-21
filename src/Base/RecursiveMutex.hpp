//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file RecursiveMutex.hpp recursive mutex class
//
#pragma once

// includes
#include "MutexLock.hpp"
#include "SystemException.hpp"

/// a recursive lockable synchronisation object
class RecursiveMutex
{
public:
   /// lock type
   typedef MutexLock<RecursiveMutex> LockType;

   /// try-lock type
   typedef MutexTryLock<RecursiveMutex> TryLockType;

   /// ctor
   RecursiveMutex()
   {
      HANDLE hMutex = ::CreateMutex(NULL, FALSE, NULL);
      if (hMutex == NULL)
         throw SystemException(_T("failed to create mutex"), GetLastError(), __FILE__, __LINE__);

      m_spMutex.reset(hMutex, ::CloseHandle);
   }

private:
   /// locks object
   void Lock()
   {
      DWORD dwRet = ::WaitForSingleObject(m_spMutex.get(), INFINITE);
      if (dwRet == WAIT_OBJECT_0)
         return;

      if (dwRet == WAIT_ABANDONED)
         throw SystemException(_T("try locking abandoned mutex"), dwRet, __FILE__, __LINE__);

      // unknown failure
      throw SystemException(_T("failed to lock recursive mutex"), GetLastError(), __FILE__, __LINE__);
   }

   /// tries locking object, with timeout (in milliseconds)
   bool TryLock(DWORD dwTimeoutInMs)
   {
      DWORD dwRet = ::WaitForSingleObject(m_spMutex.get(), dwTimeoutInMs);
      if (dwRet == WAIT_OBJECT_0)
         return true;

      if (dwRet == WAIT_TIMEOUT)
         return false;

      if (dwRet == WAIT_ABANDONED)
         throw SystemException(_T("try locking abandoned mutex"), dwRet, __FILE__, __LINE__);

      // unknown failure
      throw SystemException(_T("failed to lock recursive mutex"), GetLastError(), __FILE__, __LINE__);
   }

   /// locks object
   void Unlock()
   {
      BOOL bRet = ::ReleaseMutex(m_spMutex.get());
      if (bRet == FALSE)
         ATLTRACE(_T("RecursiveMutex::Unlock() error: didn't own mutex!\n"));
   }

   // so that Lock class can call Lock(), Try() and Unlock()
   friend MutexLock<RecursiveMutex>;
   friend MutexTryLock<RecursiveMutex>;
   friend MutexUnLocker<RecursiveMutex>;

private:
   /// mutex handle
   std::shared_ptr<void> m_spMutex;
};
