//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file OneShotExecuteTimer.hpp One-shot execution timer
//
#pragma once

#include <memory>
#include <functional>
#include <ctime>

class SingleThreadExecutor;

/// Timer that executes a timer function once at a specific time
class OneShotExecuteTimer
{
public:
   /// ctor; starts timer
   OneShotExecuteTimer(
      SingleThreadExecutor& executor,
      time_t executeTime,
      std::function<void()> timerFunc);

   /// dtor; stops timer
   ~OneShotExecuteTimer() noexcept;

private:
   struct Impl;

   /// implementation
   std::shared_ptr<Impl> m_impl;
};
