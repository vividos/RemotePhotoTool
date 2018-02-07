//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file TimeLapseScheduler.cpp TimeLapse scheduler
//

#include "stdafx.h"
#include "TimeLapseScheduler.hpp"
#include <ulib/thread/Thread.hpp>

TimeLapseScheduler::TimeLapseScheduler()
   :m_ioService(1),
   m_upDefaultWork(new boost::asio::io_service::work(m_ioService)),
   m_isFinished(false)
{
   m_upThread.reset(new std::thread(std::bind(&TimeLapseScheduler::Run, this)));
}

TimeLapseScheduler::~TimeLapseScheduler()
{
   try
   {
      m_isFinished = true;
      m_upDefaultWork.reset();
      CancelAll();

      m_upThread->join();
   }
   catch (...)
   {
   }
}

void TimeLapseScheduler::Schedule(const std::function<void()>& func)
{
   if (m_isFinished)
      return;

   m_ioService.post(func);
}

void TimeLapseScheduler::Schedule(const ATL::COleDateTime& dateTime, const std::function<void()>& func)
{
   if (m_isFinished)
      return;

   auto timer = std::make_shared<boost::asio::system_timer>(m_ioService);

   SYSTEMTIME systemTime = { 0 };
   dateTime.GetAsSystemTime(systemTime);

   CTime ctime(systemTime);

   auto nextTime = std::chrono::system_clock::from_time_t(ctime.GetTime());

   timer->expires_at(nextTime);
   timer->async_wait([func](const boost::system::error_code& ec)
   {
      if (!ec)
         func();
   });

   m_setAllTimer.insert(timer);
}

void TimeLapseScheduler::CancelAll()
{
   // cancel all timer
   for (auto& timer : m_setAllTimer)
   {
      timer->cancel();
   }

   m_setAllTimer.clear();

   m_ioService.stop();
}

void TimeLapseScheduler::Run()
{
   Thread::SetName(_T("TimeLapseScheduler Thread"));

   // run message queue
   while (!m_isFinished)
   {
      // process asio handlers, if any
      boost::system::error_code ec;

      m_ioService.poll_one(ec);

      // skip current time slice
      Sleep(1);
   }
}
