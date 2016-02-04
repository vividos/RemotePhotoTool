//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file LuaScheduler.cpp Lua scheduler class
//
#pragma once

// includes
#include "Lua.hpp"

/// scheduler class that integrates Lua thread, execution state and starting/resuming
class LuaScheduler
{
public:
   /// execution state of script processor
   enum T_enExecutionState
   {
      stateIdle = 0,    ///< processor is idle and a function can be run
      stateRunning = 1, ///< processor is currently running
      stateYield = 2,   ///< a script yielded, and processor is waiting for resuming
      stateDebug = 3,   ///< script debugger is attached and is waiting for a script debug command
      stateError = 4    ///< processor is in an error state
   };

   /// ctor
   LuaScheduler()
      :m_enExecutionState(stateIdle),
      m_thread(m_state)
   {
   }

   /// returns Lua state object
   Lua::State& GetState() throw() { return m_state; }

   /// returns Lua thread object
   Lua::Thread& GetThread() throw() { return m_thread; }

   /// function type to notify about execution state changes
   typedef std::function<void(T_enExecutionState)> T_fnOnExecutionStateChanged;

   /// sets handler to notify about execution state changes
   void SetExecutionStateChangedHandler(T_fnOnExecutionStateChanged fnOnExecutionStateChanged)
   {
      m_fnOnExecutionStateChanged = fnOnExecutionStateChanged;
   }

   /// returns current execution state
   T_enExecutionState CurrentExecutionState() const
   {
      return m_enExecutionState;
   }

   /// sets new execution state
   void CurrentExecutionState(T_enExecutionState enExecutionState)
   {
      m_enExecutionState = enExecutionState;

      if (m_fnOnExecutionStateChanged != nullptr)
         m_fnOnExecutionStateChanged(enExecutionState);
   }

   /// starts main thread with given function and params
   void StartMainThread(Lua::Function func, const std::vector<Lua::Value>& vecParam);

   /// resumes main thread with given return values
   void ResumeMainThread(const std::vector<Lua::Value>& vecRetvals);

   /// resets main thread, e.g. if it is currently yielded
   void ResetMainThread();

private:
   /// updates execution status by using thread status
   void UpdateStatus(Lua::Thread::T_enThreadStatus enThreadStatus);

private:
   /// Lua state
   Lua::State m_state;

   /// Lua thread that is used to run scripts
   Lua::Thread m_thread;

   /// current execution state
   T_enExecutionState m_enExecutionState;

   /// execution state changes handler
   T_fnOnExecutionStateChanged m_fnOnExecutionStateChanged;
};
