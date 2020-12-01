//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file PeriodicExecuteTimer.cpp Periodic execution timer
//
#include "stdafx.h"
#include "PeriodicExecuteTimer.hpp"
#include "SingleThreadExecutor.hpp"
#include "SingleThreadExecutorImpl.hpp"
#include <ulib/config/BoostAsio.hpp>
#include <ulib/thread/LightweightMutex.hpp>

/// PeriodicExecuteTimer implementation
struct PeriodicExecuteTimer::Impl :
   public std::enable_shared_from_this<PeriodicExecuteTimer::Impl>
{
   /// contains the executor's implementation in order to only destroy the
   /// executor when the periodic timer is also destroyed
   std::shared_ptr<SingleThreadExecutor::Impl> m_executorImpl;

   /// deadline timer
   boost::asio::deadline_timer m_timer;

   /// timer interval
   unsigned int m_timerPeriodInMilliseconds;

   /// mutext to protect callback and timer
   LightweightMutex m_mtxTimerCallback;

   /// timer function
   std::function<void()> m_timerFunc;

   /// ctor
   Impl(std::shared_ptr<SingleThreadExecutor::Impl> executorImpl,
      unsigned int timerPeriodInMilliseconds,
      std::function<void()> timerFunc);

   /// dtor
   ~Impl() noexcept;

   /// (re-)initializes timer
   void InitTimer();

   /// timer handler
   void OnTimer(const boost::system::error_code& error);

   /// stops timer
   void Stop();
};

PeriodicExecuteTimer::Impl::Impl(std::shared_ptr<SingleThreadExecutor::Impl> executorImpl,
   unsigned int timerPeriodInMilliseconds, std::function<void()> timerFunc)
   :m_executorImpl(executorImpl),
   m_timer(executorImpl->m_ioService),
   m_timerPeriodInMilliseconds(timerPeriodInMilliseconds),
   m_timerFunc(timerFunc)
{
}

PeriodicExecuteTimer::Impl::~Impl() noexcept
{
   try
   {
      Stop();
   }
   catch (...)
   {
   }
}

void PeriodicExecuteTimer::Impl::InitTimer()
{
   m_timer.expires_from_now(boost::posix_time::milliseconds(m_timerPeriodInMilliseconds));
   m_timer.async_wait(std::bind(&Impl::OnTimer, shared_from_this(), std::placeholders::_1));
}

void PeriodicExecuteTimer::Impl::OnTimer(const boost::system::error_code& error)
{
   if (error)
      return; // timer was cancelled

   {
      LightweightMutex::LockType lock{ m_mtxTimerCallback };

      try
      {
         if (m_timerFunc != nullptr)
         {
            m_timerFunc();
         }
      }
      catch (...)
      {
      }
   }

   InitTimer();
}

void PeriodicExecuteTimer::Impl::Stop()
{
   {
      LightweightMutex::LockType lock{ m_mtxTimerCallback };
      m_timerFunc = nullptr;
   }

   m_timer.cancel();
}

PeriodicExecuteTimer::PeriodicExecuteTimer(
   SingleThreadExecutor& executor, unsigned int timerPeriodInMilliseconds, std::function<void()> timerFunc)
   :m_impl(std::make_shared<Impl>(executor.m_impl, timerPeriodInMilliseconds, timerFunc))
{
   m_impl->InitTimer();
}

PeriodicExecuteTimer::~PeriodicExecuteTimer() noexcept
{
   m_impl->Stop();
}
