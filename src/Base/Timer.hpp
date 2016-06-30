//
// ulib - a collection of useful classes
// Copyright (C) 2006,2007,2008,2012,2013,2016 Michael Fink
//
/// \file Timer.hpp Timer class
//
#pragma once

#pragma warning(push)
#pragma warning(disable: 28159) // Consider using 'GetTickCount64' instead of 'GetTickCount'. Reason: GetTickCount overflows roughly every 49 days.  Code that does not take that into account can loop indefinitely.  GetTickCount64 operates on 64 bit values and does not have that problem

/// \brief Timer class
/// \details Has a precision of about 15 ms.
class Timer
{
public:
   /// ctor
   Timer() throw()
      :m_bStarted(false),
       m_dwTickStart(0),
       m_dwTotalElapsed(0)
   {
   }

   /// starts timer; timer must be stopped or reset
   void Start() throw()
   {
      ATLASSERT(m_bStarted == false);
      m_dwTickStart = GetTickCount();
      m_bStarted = true;
   }

   /// stops timer; timer must be started
   void Stop() throw()
   {
      ATLASSERT(m_bStarted == true);

      DWORD dwTickEnd = GetTickCount();
      m_dwTotalElapsed += (dwTickEnd-m_dwTickStart);
      m_bStarted = false;
   }

   /// resets timer; timer must be stopped
   void Reset() throw()
   {
      ATLASSERT(m_bStarted == false); // must be stopped when calling Reset()
      m_dwTotalElapsed = 0;
   }

   /// restarts timer by resetting and starting again
   void Restart() throw()
   {
      // can either be stopped or started when restarting
      if (m_bStarted)
         Stop();
      Reset();
      Start();
   }

   /// returns elapsed time since Start() was called, in seconds
   double Elapsed() const throw()
   {
      if (!m_bStarted)
         return 0.0; // no time elapsed, since timer isn't started

      DWORD dwTickNow = GetTickCount();
      return 0.001 * (dwTickNow-m_dwTickStart);
   }

   /// returns total elapsed time in seconds
   double TotalElapsed() const throw()
   {
      DWORD dwTickNow = GetTickCount();
      return 0.001 * (m_dwTotalElapsed + (m_bStarted ? (dwTickNow-m_dwTickStart) : 0));
   }

   /// returns if timer is running
   bool IsStarted() const throw() { return m_bStarted; }

private:
   /// indicates if timer is started
   bool m_bStarted;

   /// tick count of start
   DWORD m_dwTickStart;

   /// number of ticks already elapsed
   DWORD m_dwTotalElapsed;
};

#pragma warning(pop)
