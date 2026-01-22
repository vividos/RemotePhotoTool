//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2026 Michael Fink
//
/// \file SingleThreadExecutorImpl.hpp Single-thread executor implementation
//
#pragma once

#include "SingleThreadExecutor.hpp"
#include <asio.hpp>
#include <ulib/thread/Thread.hpp>

/// SingleThreadExecutor implementation
struct SingleThreadExecutor::Impl : public std::enable_shared_from_this<SingleThreadExecutor::Impl>
{
   /// ctor
   Impl(LPCTSTR threadName)
      :m_threadName(threadName),
      m_ioService(1),
      m_defaultWork(new asio::io_service::work(m_ioService)),
      m_thread(std::bind(&Impl::Run, this)),
      m_isFinished(false)
   {
   }

   /// dtor
   ~Impl() noexcept;

   /// runs worker thread
   void Run();

   /// thread name, or nullptr if default should be used
   LPCTSTR m_threadName;

   /// io service
   asio::io_service m_ioService;

   /// default work
   std::unique_ptr<asio::io_service::work> m_defaultWork;

   /// background thread
   std::thread m_thread;

   /// indicates that thread should stop
   std::atomic<bool> m_isFinished;
};
