//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file TimeLapseScheduler.cpp TimeLapse scheduler
//
#include "stdafx.h"
#include "TimeLapseScheduler.hpp"
#include "OneShotExecuteTimer.hpp"

TimeLapseScheduler::TimeLapseScheduler()
   :m_executor(std::make_unique<SingleThreadExecutor>(_T("TimeLapseScheduler Thread")))
{
}

TimeLapseScheduler::~TimeLapseScheduler()
{
   try
   {
      CancelAll();
   }
   catch (...)
   {
   }
}

void TimeLapseScheduler::Schedule(const std::function<void()>& func)
{
   if (m_executor != nullptr)
      m_executor->Schedule(func);
}

void TimeLapseScheduler::Schedule(const ATL::COleDateTime& dateTime, const std::function<void()>& func)
{
   if (m_executor == nullptr)
      return;

   SYSTEMTIME systemTime = { 0 };
   dateTime.GetAsSystemTime(systemTime);

   CTime ctime(systemTime);

   auto timer = std::make_shared<OneShotExecuteTimer>(*m_executor, ctime.GetTime(), func);

   m_setAllTimer.insert(timer);
}

void TimeLapseScheduler::CancelAll()
{
   m_setAllTimer.clear();
   m_executor.reset();
}
