//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2026 Michael Fink
//
/// \file SingleThreadExecutor.cpp Single-thread executor
//
#include "stdafx.h"
#include "SingleThreadExecutor.hpp"
#include "SingleThreadExecutorImpl.hpp"
#include <asio.hpp>

SingleThreadExecutor::Impl::~Impl() noexcept
{
   try
   {
      m_defaultWork.reset();
      m_ioService.stop();

      if (m_thread.joinable())
         m_thread.join();
   }
   catch (...)
   {
   }
}

void SingleThreadExecutor::Impl::Run()
{
   Thread::SetName(m_threadName == nullptr ? _T("SingleThreadExecutor") : m_threadName);

   // run message queue
   while (!m_isFinished)
   {
      // process asio handlers, if any
      std::error_code ec;

      m_ioService.poll_one(ec);

      // skip current time slice
      Sleep(1);
   }
}

SingleThreadExecutor::SingleThreadExecutor(LPCTSTR threadName)
   :m_impl(std::make_unique<Impl>(threadName))
{
}

SingleThreadExecutor::~SingleThreadExecutor()
{
   m_impl->m_isFinished = true;
   m_impl.reset();
}

void SingleThreadExecutor::Schedule(std::function<void()> func)
{
   if (m_impl->m_isFinished)
      return;

   m_impl->m_ioService.post(func);
}
