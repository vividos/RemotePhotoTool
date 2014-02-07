//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file BackgroundWindowThread.hpp Canon control - Background window thread
//
#pragma once

// includes
#include "Event.hpp"
#include <thread>
#include "Thread.hpp"
#include "EdsdkCommon.hpp"

/// this is the background window we're starting
/// \note this class is only usable for EDSDK at the moment
template <typename TInstance>
class BackgroundWindowThread
{
public:
   /// ctor
   BackgroundWindowThread()
      :m_evtStarted(false, false), // auto-reset event
       m_evtStop(true, false) // manual-reset event
   {
   }
   /// dtor
   ~BackgroundWindowThread()
   {
      if (m_upThread != nullptr)
      {
         m_evtStop.Set();
         m_upThread->join();
      }
   }

   /// starts background thread
   void Start()
   {
      m_upThread.reset(new std::thread(std::bind(&BackgroundWindowThread::Run, this)));
      m_evtStarted.Wait();
   }

private:
   /// thread method
   int Run()
   {
      Thread::SetName(_T("BackgroundWindowThread"));
      // note: we don't even have to create a window, it's enough to run the message loop

      // start instance
      m_upInst.reset(new TInstance);

      m_evtStarted.Set();

      // run EDSDK and window message processing while waiting for stop event
      EDSDK::MsgWaitForEvent(m_evtStop);

      // end instance
      m_upInst.reset();

      return 0;
   }

private:
   Event m_evtStarted;  ///< indicates if thread has started
   Event m_evtStop;     ///< indicates that thread should stop
   std::unique_ptr<std::thread> m_upThread;  ///< background thread
   std::unique_ptr<TInstance> m_upInst;      ///< instance
};
