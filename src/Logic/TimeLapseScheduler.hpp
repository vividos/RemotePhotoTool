//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file TimeLapseScheduler.hpp TimeLapse scheduler
//
#pragma once

#include <set>
#include <thread>
#include "SingleThreadExecutor.hpp"
#include <ATLComTime.h>

/// scheduler for time lapse photo mode
class TimeLapseScheduler
{
public:
   /// ctor
   TimeLapseScheduler();
   /// dtor
   ~TimeLapseScheduler();

   /// schedule function for immediate execution
   void Schedule(const std::function<void()>& func);

   /// schedule function for delayed execution at given time
   void Schedule(const ATL::COleDateTime& dateTime, const std::function<void()>& func);

   /// cancels all scheduled handler functions
   void CancelAll();

   /// returns if scheduler was already finished
   bool IsFinished() const { return m_executor == nullptr; }

private:
   /// background thread executor
   std::unique_ptr<SingleThreadExecutor> m_executor;

   /// all currently running timers
   std::set<std::shared_ptr<OneShotExecuteTimer>> m_setAllTimer;
};
