//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file LuaScheduler.cpp Lua scheduler class
//

// includes
#include "stdafx.h"
#include "LuaScheduler.hpp"

void LuaScheduler::StartMainThread(Lua::Function func, const std::vector<Lua::Value>& vecParam)
{
   CurrentExecutionState(stateRunning);

   std::pair<Lua::Thread::T_enThreadStatus, std::vector<Lua::Value>> retVal;
   try
   {
      retVal = m_thread.Start(func, vecParam);
   }
   catch (const Lua::Exception&)
   {
      CurrentExecutionState(stateIdle);
      throw;
   }

   UpdateStatus(retVal.first);
}

void LuaScheduler::ResumeMainThread(const std::vector<Lua::Value>& vecRetvals)
{
   CurrentExecutionState(stateRunning);

   std::pair<Lua::Thread::T_enThreadStatus, std::vector<Lua::Value>> retVal;
   try
   {
      retVal = m_thread.Resume(vecRetvals);
   }
   catch (const Lua::Exception&)
   {
      CurrentExecutionState(stateIdle);
      throw;
   }

   UpdateStatus(retVal.first);
}

void LuaScheduler::UpdateStatus(Lua::Thread::T_enThreadStatus enThreadStatus)
{
   if (enThreadStatus == Lua::Thread::statusOK)
      CurrentExecutionState(stateIdle);
   else
      if (enThreadStatus == Lua::Thread::statusYield)
         CurrentExecutionState(stateYield);
      else
         ATLASSERT(false); // unknown status code
}
