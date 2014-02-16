//
// ulib - a collection of useful classes
// Copyright (C) 2008,2013 Michael Fink
//
/// \file Event.hpp event class
//
#pragma once

// includes
#include "SystemException.hpp"
#include <memory>

/// event class
class Event
{
public:
   /// ctor
   Event(bool bManualReset, bool bInitialState)
   {
      HANDLE hEvent = ::CreateEvent(NULL, bManualReset ? TRUE : FALSE, bInitialState ? TRUE : FALSE, NULL);
      if (hEvent == NULL)
         throw SystemException(_T("failed to create event"), GetLastError(), __FILE__, __LINE__);

      m_spEvent.reset(hEvent, ::CloseHandle);
   }

   /// sets event
   void Set()
   {
      BOOL bRet = ::SetEvent(m_spEvent.get());
      if (bRet == FALSE)
         throw SystemException(_T("failed to set event"), GetLastError(), __FILE__, __LINE__);
   }

   /// resets event
   void Reset()
   {
      BOOL bRet = ::ResetEvent(m_spEvent.get());
      if (bRet == FALSE)
         throw SystemException(_T("failed to set event"), GetLastError(), __FILE__, __LINE__);
   }

   /// waits given time (or infinitely) for event to get set
   bool Wait(DWORD dwTimeout = INFINITE)
   {
      DWORD dwRet = ::WaitForSingleObject(m_spEvent.get(), dwTimeout);
      if (dwRet == WAIT_OBJECT_0)
         return true;
      if (dwRet == WAIT_TIMEOUT)
         return false;

      throw SystemException(_T("failed to wait for event"), dwRet, __FILE__, __LINE__);
   }

   /// returns internal event handle
   HANDLE Handle() const throw() { return m_spEvent != NULL ? m_spEvent.get() : INVALID_HANDLE_VALUE; }

private:
   /// event handle
   std::shared_ptr<void> m_spEvent;
};
