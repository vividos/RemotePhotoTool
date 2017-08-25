//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file BackgroundTimer.hpp Canon control - Background timer
//

// includes
#pragma once

// includes
#include <ulib/config/BoostAsio.hpp>
#include <ulib/thread/LightweightMutex.hpp>

/// background timer
class BackgroundTimer: public std::enable_shared_from_this<BackgroundTimer>
{
public:
   /// ctor
   BackgroundTimer(boost::asio::io_service& ioService, unsigned int uiMilliseconds,
      std::function<void()> fnCallback)
      :m_timer(ioService),
       m_uiMilliseconds(uiMilliseconds),
       m_fnCallback(fnCallback)
   {
      ATLASSERT(fnCallback != nullptr);
   }
   /// dtor
   ~BackgroundTimer() throw()
   {
      try
      {
         Stop();
      }
      catch (...)
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

      m_fnCallback = nullptr;
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
            if (m_fnCallback != nullptr)
            {
               m_fnCallback();
            }
         }
         catch (...)
         {
         }
      }

      InitTimer();
   }

   /// inits timer
   void InitTimer()
   {
      m_timer.expires_from_now(boost::posix_time::milliseconds(m_uiMilliseconds));
      m_timer.async_wait(std::bind(&BackgroundTimer::OnTimer, shared_from_this(), std::placeholders::_1));
   }

private:
   /// deadline timer
   boost::asio::deadline_timer m_timer;

   /// timer interval
   unsigned int m_uiMilliseconds;

   /// mutext to protect callback and timer
   LightweightMutex m_mtxTimerCallback;

   /// callback to call
   std::function<void()> m_fnCallback;
};
