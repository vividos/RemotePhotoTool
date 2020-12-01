//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file PeriodicExecuteTimer.hpp Periodic execution timer
//
#pragma once

#include <memory>
#include <functional>

class SingleThreadExecutor;

/// Periodic timer that executes a timer function periodically
class PeriodicExecuteTimer
{
public:
   /// ctor; starts timer
   PeriodicExecuteTimer(
      SingleThreadExecutor& executor,
      unsigned int timerPeriodInMilliseconds,
      std::function<void()> timerFunc);

   /// dtor; stops timer
   ~PeriodicExecuteTimer() noexcept;

private:
   struct Impl;

   /// implementation
   std::shared_ptr<Impl> m_impl;
};
