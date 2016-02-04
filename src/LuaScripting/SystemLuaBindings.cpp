//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file SystemLuaBindings.cpp Lua bindings for system functionality
//

// includes
#include "stdafx.h"
#include "SystemLuaBindings.hpp"
#include "LuaScheduler.hpp"

/// poll time for manual-reset events
const unsigned int c_uiManualResetEventPollTimeInMs = 50;

SystemLuaBindings::SystemLuaBindings(LuaScheduler& scheduler, boost::asio::io_service::strand& strand)
:m_scheduler(scheduler),
m_strand(strand)
{
}

SystemLuaBindings::~SystemLuaBindings() throw()
{
   try
   {
      CleanupBindings();
   }
   catch (...)
   {
   }
}

void SystemLuaBindings::InitBindings()
{
   Lua::Table sys = GetState().AddTable(_T("Sys"));

   sys.AddFunction("isMainThread",
      std::bind(&SystemLuaBindings::SysIsMainThread, shared_from_this(),
         std::placeholders::_1));

   sys.AddFunction("createEvent",
      std::bind(&SystemLuaBindings::SysCreateEvent, shared_from_this(),
         std::placeholders::_1));
}

/// returns Lua state object
Lua::State& SystemLuaBindings::GetState() throw()
{
   return m_scheduler.GetState();
}

void SystemLuaBindings::CancelHandlers()
{
   std::for_each(m_vecAllEvents.begin(), m_vecAllEvents.end(), [](std::shared_ptr<ManualResetEvent>& spEvent)
   {
      spEvent->Cancel();
   });

   m_vecAllEvents.clear();
}

void SystemLuaBindings::CleanupBindings()
{
   GetState().AddValue(_T("Sys"), Lua::Value());

   GetState().CollectGarbage();
}

std::vector<Lua::Value> SystemLuaBindings::SysIsMainThread(Lua::State& state)
{
   bool bIsMainThread = state.GetState() == m_scheduler.GetThread().GetThreadState();

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(bIsMainThread));

   return vecRetValues;
}

std::vector<Lua::Value> SystemLuaBindings::SysCreateEvent(Lua::State& state)
{
   std::shared_ptr<ManualResetEvent> spEvent =
      std::make_shared<ManualResetEvent>(m_scheduler, m_strand);

   m_vecAllEvents.push_back(spEvent);

   Lua::Table manualResetEvent = state.AddTable(_T(""));
   spEvent->InitBindings(manualResetEvent);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(manualResetEvent));

   return vecRetValues;
}

SystemLuaBindings::ManualResetEvent::ManualResetEvent(LuaScheduler& scheduler, boost::asio::io_service::strand& strand)
:m_event(true, false), // manual-reset
m_timerWait(strand.get_io_service()),
m_scheduler(scheduler),
m_strand(strand)
{
}

void SystemLuaBindings::ManualResetEvent::InitBindings(Lua::Table& manualResetEvent)
{
   manualResetEvent.AddValue("__name", Lua::Value(GetEventName()));

   manualResetEvent.AddFunction("signal",
      std::bind(&SystemLuaBindings::ManualResetEvent::Signal, shared_from_this(),
         std::placeholders::_1, std::placeholders::_2));

   manualResetEvent.AddFunction("reset",
      std::bind(&SystemLuaBindings::ManualResetEvent::Reset, shared_from_this(),
         std::placeholders::_1, std::placeholders::_2));

   manualResetEvent.AddFunction("wait",
      std::bind(&SystemLuaBindings::ManualResetEvent::Wait, shared_from_this(),
         std::placeholders::_1, std::placeholders::_2));
}

void SystemLuaBindings::ManualResetEvent::Cancel()
{
   m_timerWait.cancel();
   m_timerWait.wait();
}

/// \details Just sets the event, so that waiting threads can check for it. See implementation
/// in ManualResetEventWait() for details.
std::vector<Lua::Value> SystemLuaBindings::ManualResetEvent::Signal(Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 1)
      throw Lua::Exception(_T("invalid number of parameters to Event:set()"), state.GetState(), __FILE__, __LINE__);

   Lua::Table manualResetEvent = vecParams[0].Get<Lua::Table>();

   CString cszName = manualResetEvent.GetValue(_T("__name")).Get<CString>();
   ATLASSERT(cszName == GetEventName());

   m_event.Set();

   return std::vector<Lua::Value>();
}

/// \details Just resets the event for new use.
std::vector<Lua::Value> SystemLuaBindings::ManualResetEvent::Reset(Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 1)
      throw Lua::Exception(_T("invalid number of parameters to Event:reset()"), state.GetState(), __FILE__, __LINE__);

   Lua::Table manualResetEvent = vecParams[0].Get<Lua::Table>();

   CString cszName = manualResetEvent.GetValue(_T("__name")).Get<CString>();
   ATLASSERT(cszName == GetEventName());

   m_event.Reset();

   return std::vector<Lua::Value>();
}

/// \details Since we use coroutines and cooperative multitasking, waiting is implemented
/// in the following way: We wait by posting a handler to the scheduler (represented by
/// the strand we got in ctor) using a deadline timer, and a small timeout value, e.g. 50 ms.
/// The handler checks the event if it is set, or if the wait timeout is reached. When
/// timeout isn't reached, another handler is dispatched, waiting again 50 ms. When the
/// wait time is up, or the event got set, resume the thread that was yielded in this
/// method and report the result. So basically we "poll" the event if it is set yet.
/// This isn't really efficient, but the alternative would be to start another thread that
/// waits for the event.
/// \param[in] paramState the Lua state for parameters to this function
/// \param[in] vecParams Lua params; [0] is the event table, and [1] (optional) is a
/// timeout value, in seconds, to wait. If not specified, the wait is infinite.
std::vector<Lua::Value> SystemLuaBindings::ManualResetEvent::Wait(Lua::State& paramState,
   const std::vector<Lua::Value>& vecParams)
{
   lua_State* L = paramState.GetState();

   if (vecParams.size() != 1 && vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to Event:wait()"), L, __FILE__, __LINE__);

   Lua::Table manualResetEvent = vecParams[0].Get<Lua::Table>();

   CString cszName = manualResetEvent.GetValue(_T("__name")).Get<CString>();
   ATLASSERT(cszName == GetEventName());

   DWORD dwWaitTimeout = INFINITE;
   if (vecParams.size() == 2)
   {
      dwWaitTimeout = GetTickCount() + static_cast<DWORD>(vecParams[1].Get<int>() * 1000);
      if (dwWaitTimeout == INFINITE)
         dwWaitTimeout++; // account for accidentally adding up to INFINITE after 49 days
   }

   // start timer
   RestartTimer(dwWaitTimeout);

   // yield until our wait handler resumes
   m_scheduler.GetThread().Yield(paramState, std::vector<Lua::Value>(),
      std::bind(&SystemLuaBindings::ManualResetEvent::Resume, shared_from_this(),
         std::placeholders::_1, std::placeholders::_2));
}

std::vector<Lua::Value> SystemLuaBindings::ManualResetEvent::Resume(Lua::State&,
   const std::vector<Lua::Value>& vecParams)
{
   return vecParams;
}

void SystemLuaBindings::ManualResetEvent::RestartTimer(DWORD dwWaitTimeout)
{
   m_timerWait.expires_from_now(boost::posix_time::milliseconds(c_uiManualResetEventPollTimeInMs));
   m_timerWait.async_wait(m_strand.wrap(
      std::bind(&SystemLuaBindings::ManualResetEvent::WaitHandler, shared_from_this(),
         std::placeholders::_1, dwWaitTimeout)
      ));
}

void SystemLuaBindings::ManualResetEvent::WaitHandler(const boost::system::error_code& error,
   DWORD dwWaitTimeout)
{
   if (error)
      return; // timer was canceled

   if (m_scheduler.GetThread().Status() != Lua::Thread::statusYield)
      return; // someone else resumed our thread already

   bool bEventIsSet;

   try
   {
      bEventIsSet = m_event.Wait(0);
   }
   catch (const SystemException& ex)
   {
      throw Exception(_T("exception while waiting for event: ") + ex.Message(),
         __FILE__, __LINE__);
   }

   bool bResume = false;
   if (bEventIsSet)
      bResume = true;
   else
   {
      // event not set; check timeout
      DWORD dwNow = GetTickCount();
      if (dwWaitTimeout != INFINITE && dwNow > dwWaitTimeout)
      {
         // timeout reached
         bResume = true;
      }

      // no timeout yet? just restart timer
   }

   if (bResume)
   {
      m_strand.post([&]()
      {
         std::vector<Lua::Value> vecRetvals;
         vecRetvals.push_back(Lua::Value(bEventIsSet));

         m_scheduler.ResumeMainThread(vecRetvals);
      });
   }
   else
      RestartTimer(dwWaitTimeout);
}

CString SystemLuaBindings::ManualResetEvent::GetEventName() const
{
   CString cszEventName;
   cszEventName.Format(_T("event-%08p"), this);

   return cszEventName;
}
