//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file TimeLapseScheduler.hpp TimeLapse scheduler
//
#pragma once

#include <set>
#include <thread>
#include <ulib/config/BoostAsio.hpp>
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
   bool IsFinished() const { return m_isFinished; }

private:
   /// thread function
   void Run();

private:
   /// background thread
   std::unique_ptr<std::thread> m_upThread;

   /// io service
   boost::asio::io_service m_ioService;

   /// default work
   std::unique_ptr<boost::asio::io_service::work> m_upDefaultWork;

   /// indicates that thread should stop
   std::atomic<bool> m_isFinished;

   /// all currently running timers
   std::set<std::shared_ptr<boost::asio::system_timer>> m_setAllTimer;
};
