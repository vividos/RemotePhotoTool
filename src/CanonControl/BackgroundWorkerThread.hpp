//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file BackgroundWorkerThread.hpp Canon control - Background worker thread
//

// includes
#include <boost/bind.hpp>
#include <thread>
#include "Asio.hpp"
#include "Event.hpp"

/// background worker thread
class BackgroundWorkerThread
{
public:
   /// ctor
   BackgroundWorkerThread()
      :m_ioService(1),
       m_upDefaultWork(new boost::asio::io_service::work(m_ioService)),
       m_thread(boost::bind(&boost::asio::io_service::run, &m_ioService))
   {
   }
   /// dtor
   ~BackgroundWorkerThread()
   {
      m_upDefaultWork.reset();
      m_ioService.stop();
      m_thread.join();
   }

   /// returns asio io service
   boost::asio::io_service& GetIoService() { return m_ioService; }

private:
   /// io service
   boost::asio::io_service m_ioService;

   /// default work
   std::unique_ptr<boost::asio::io_service::work> m_upDefaultWork;

   /// background thread
   std::thread m_thread;
};

/// background timer
class BackgroundTimer: public std::enable_shared_from_this<BackgroundTimer>
{
public:
   /// ctor
   BackgroundTimer(boost::asio::io_service& ioService, unsigned int uiMilliseconds,
      boost::function<void()> fnCallback)
      :m_timer(ioService),
       m_uiMilliseconds(uiMilliseconds),
       m_fnCallback(fnCallback),
       m_evtStopped(true, true) // manual-reset event, initially set
   {
      ATLASSERT(fnCallback.empty() == false);
   }
   /// dtor
   ~BackgroundTimer() throw()
   {
      try
      {
         Stop();
      }
      catch(...)
      {
      }
   }

   /// start timer
   void Start()
   {
      m_evtStopped.Reset();
      InitTimer();
   }

   /// stop timer
   void Stop()
   {
      if (m_evtStopped.Wait(0))
         return; // already stopped

      m_timer.cancel();

      m_evtStopped.Wait();
   }

private:
   /// timer handler
   void OnTimer(const boost::system::error_code& error)
   {
      if (error)
      {
         // timer was canceled
         m_evtStopped.Set();
         return;
      }

      m_fnCallback();

      InitTimer();
   }

   /// inits timer
   void InitTimer()
   {
      m_timer.expires_from_now(boost::posix_time::milliseconds(m_uiMilliseconds));
      m_timer.async_wait(boost::bind(&BackgroundTimer::OnTimer, shared_from_this(), _1));
   }

private:
   /// deadline timer
   boost::asio::deadline_timer m_timer;

   /// timer interval
   unsigned int m_uiMilliseconds;

   /// callback to call
   boost::function<void()> m_fnCallback;

   /// event to signal that timer has stopped
   Event m_evtStopped;
};
