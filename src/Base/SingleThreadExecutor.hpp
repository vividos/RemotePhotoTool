//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2023 Michael Fink
//
/// \file SingleThreadExecutor.hpp Single-thread executor
//
#pragma once

#include <functional>
#include <memory>

/// Executor that uses a single background thread to execute functions
/// \details once C++23 has executors, this can be replaced with
/// std::static_thread_pool
class SingleThreadExecutor
{
public:
   /// sets up executor and starts background thread
   SingleThreadExecutor(LPCTSTR threadName = nullptr);
   /// waits for any executing methods to stop and quits background thread
   ~SingleThreadExecutor() noexcept;

   /// Schedules a function to run
   void Schedule(std::function<void()> func);

private:
   friend class PeriodicExecuteTimer;
   friend class OneShotExecuteTimer;

   struct Impl;

   /// implementation details
   std::shared_ptr<Impl> m_impl;
};
