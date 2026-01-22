//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2026 Michael Fink
//
/// \file OneShotExecuteTimer.cpp One-shot execution timer
//
#include "stdafx.h"
#include "OneShotExecuteTimer.hpp"
#include "SingleThreadExecutor.hpp"
#include "SingleThreadExecutorImpl.hpp"
#include <asio.hpp>
#include <ulib/thread/LightweightMutex.hpp>

/// OneShotExecuteTimer implementation
struct OneShotExecuteTimer::Impl :
   public std::enable_shared_from_this<OneShotExecuteTimer::Impl>
{
   /// contains the executor's implementation in order to only destroy the
   /// executor when the one-shot timer is also destroyed
   std::shared_ptr<SingleThreadExecutor::Impl> m_executorImpl;

   /// system timer
   asio::system_timer m_timer;

   /// mutext to protect callback and timer
   LightweightMutex m_mtxTimerCallback;

   /// timer function
   std::function<void()> m_timerFunc;

   /// ctor
   Impl(std::shared_ptr<SingleThreadExecutor::Impl> executorImpl,
      std::function<void()> timerFunc);

   /// dtor
   ~Impl() noexcept;

   /// initializes timer
   void InitTimer(time_t executeTime);

   /// timer handler
   void OnTimer(const std::error_code& error);

   /// stops timer
   void Stop();
};

OneShotExecuteTimer::Impl::Impl(std::shared_ptr<SingleThreadExecutor::Impl> executorImpl,
   std::function<void()> timerFunc)
   :m_executorImpl(executorImpl),
   m_timer(executorImpl->m_ioService),
   m_timerFunc(timerFunc)
{
}

OneShotExecuteTimer::Impl::~Impl() noexcept
{
   try
   {
      Stop();
   }
   catch (...)
   {
   }
}

void OneShotExecuteTimer::Impl::InitTimer(time_t executeTime)
{
   auto nextTime = std::chrono::system_clock::from_time_t(executeTime);

   m_timer.expires_at(nextTime);
   m_timer.async_wait(std::bind(&Impl::OnTimer, shared_from_this(), std::placeholders::_1));
}

void OneShotExecuteTimer::Impl::OnTimer(const std::error_code& error)
{
   if (error)
      return; // timer was cancelled

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

void OneShotExecuteTimer::Impl::Stop()
{
   {
      LightweightMutex::LockType lock{ m_mtxTimerCallback };
      m_timerFunc = nullptr;
   }

   m_timer.cancel();
}

OneShotExecuteTimer::OneShotExecuteTimer(
   SingleThreadExecutor& executor, time_t executeTime, std::function<void()> timerFunc)
   :m_impl(std::make_shared<Impl>(executor.m_impl, timerFunc))
{
   m_impl->InitTimer(executeTime);
}

OneShotExecuteTimer::~OneShotExecuteTimer() noexcept
{
   m_impl->Stop();
}
