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
#include "Thread.hpp"
#include "LightweightMutex.hpp"

/// background worker thread
class BackgroundWorkerThread
{
public:
   /// ctor
   BackgroundWorkerThread()
      :m_ioService(1),
       m_upDefaultWork(new boost::asio::io_service::work(m_ioService)),
       m_thread(boost::bind(&BackgroundWorkerThread::Run, this))
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
   /// thread function
   void Run()
   {
      Thread::SetName(_T("BackgroundWorkerThread"));
      m_ioService.run();
   }

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
       m_fnCallback(fnCallback)
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
      InitTimer();
   }

   /// stop timer
   void Stop()
   {
      LightweightMutex::LockType lock(m_mtxTimerCallback);

      m_timer.cancel();

      m_fnCallback.clear();
   }

private:
   /// timer handler
   void OnTimer(const boost::system::error_code& error)
   {
      LightweightMutex::LockType lock(m_mtxTimerCallback);

      if (error)
         return; // timer was canceled

      {
         try
         {
            if (!m_fnCallback.empty())
               m_fnCallback();
         }
         catch(...)
         {
         }
      }

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

   /// mutext to protect callback and timer
   LightweightMutex m_mtxTimerCallback;

   /// callback to call
   boost::function<void()> m_fnCallback;
};
