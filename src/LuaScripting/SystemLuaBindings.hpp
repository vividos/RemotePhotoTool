//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file SystemLuaBindings.hpp Lua bindings for system functionality
//
#pragma once

// includes
#include "Lua.hpp"
#include "Asio.hpp"
#include "Event.hpp"

// forward references
struct SystemEvent;
class LuaScheduler;

/// Lua bindings for System library
class SystemLuaBindings : public std::enable_shared_from_this<SystemLuaBindings>
{
public:
   /// ctor; inits bindings
   SystemLuaBindings(LuaScheduler& scheduler, boost::asio::io_service::strand& strand);

   /// dtor; cleans up bindings
   virtual ~SystemLuaBindings() throw();

   /// inits bindings to CanonControl; since the this parameter is needed in
   /// the bindings, call this immediately after the ctor
   void InitBindings();

   /// cancels all handlers of async operations
   void CancelHandlers();

private:
   /// returns Lua state object
   Lua::State& GetState() throw();

   void CleanupBindings();

   // Sys functions

   /// system function; returns if current execution is on the main thread
   std::vector<Lua::Value> SysIsMainThread(Lua::State& state);

   /// system function; creates a manual reset event that can be set and waited on
   std::vector<Lua::Value> SysCreateEvent(Lua::State& state);

   // manual reset event functions

   /// manual reset event for System library
   class ManualResetEvent : public std::enable_shared_from_this<ManualResetEvent>
   {
   public:
      /// ctor
      ManualResetEvent(LuaScheduler& scheduler, boost::asio::io_service::strand& strand);

      /// init event object in given table
      void InitBindings(Lua::Table& manualResetEvent);

      /// cancels processing event
      void Cancel();

   private:
      /// signals the event
      std::vector<Lua::Value> Signal(Lua::State& state, const std::vector<Lua::Value>& vecParams);

      /// resets the event
      std::vector<Lua::Value> Reset(Lua::State& state, const std::vector<Lua::Value>& vecParams);

      /// waits for event; yields the main thread
      std::vector<Lua::Value> Wait(Lua::State& state, const std::vector<Lua::Value>& vecParams);

      /// called when main thread is resumed
      std::vector<Lua::Value> Resume(Lua::State& state, const std::vector<Lua::Value>& vecParams);

      /// restarts timer to wait for event
      void RestartTimer(DWORD dwWaitTimeout);

      /// wait handler; checks for event state and resumes thread (yielded in wait())
      void WaitHandler(const boost::system::error_code& error, DWORD dwWaitTimeout);

      /// returns internal event name
      CString GetEventName() const;

   private:
      /// actual manual-reset event
      Event m_event;

      /// wait timer
      boost::asio::deadline_timer m_timerWait;

      /// Lua scheduler
      LuaScheduler& m_scheduler;

      /// strand to execute all Lua calls on
      boost::asio::io_service::strand& m_strand;
   };

private:
   /// Lua scheduler
   LuaScheduler& m_scheduler;

   /// strand to execute all Lua calls on
   boost::asio::io_service::strand& m_strand;

   /// all events created by SysCreateEvent()
   std::vector<std::shared_ptr<ManualResetEvent>> m_vecAllEvents;
};
