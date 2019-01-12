//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file CameraScriptProcessor.cpp Camera Lua script processor
//

// includes
#include "stdafx.h"
#include "CameraScriptProcessor.hpp"
#include "Lua.hpp"
#include "SystemLuaBindings.hpp"
#include "CanonControlLuaBindings.hpp"
#include "LuaScriptWorkerThread.hpp"
#include "LuaScheduler.hpp"
#include <lua.h>
#include <lualib.h>

/// implementation class, mostly for bindings
class CameraScriptProcessor::Impl : public std::enable_shared_from_this<CameraScriptProcessor::Impl>
{
public:
   /// ctor
   Impl()
   {
   }
   /// dtor
   virtual ~Impl()
   {
   }

   /// returns Lua scheduler object
   LuaScheduler& GetScheduler() { return m_scheduler; }

   /// returns Lua state object
   Lua::State& GetState() { return m_scheduler.GetState(); }

   /// sets output debug string handler
   void SetOutputDebugStringHandler(T_fnOutputDebugString fnOutputDebugString)
   {
      m_fnOutputDebugString = fnOutputDebugString;

      if (m_spCanonControlLuaBindings != nullptr)
         m_spCanonControlLuaBindings->SetOutputDebugStringHandler(fnOutputDebugString);

      m_scriptWorkerThread.SetOutputDebugStringHandler(fnOutputDebugString);
   }

   /// inits bindings to system functions and CanonControl
   void InitBindings()
   {
      InitBuiltinLibs();
      SandboxBuiltinLibs();
      InitGlobalFunctions();
      InitExtraBindings();
   }

   /// loads script file
   void LoadFile(const CString& cszFilename)
   {
      m_scriptWorkerThread.GetStrand().post([&, cszFilename]()
      {
         m_scheduler.GetState().LoadFile(cszFilename);
      });
   }

   /// loads Lua source string
   void LoadSourceString(const CString& cszLuaSource)
   {
      m_scriptWorkerThread.GetStrand().post([&, cszLuaSource]()
      {
         m_scheduler.GetState().LoadSourceString(cszLuaSource);
      });
   }

   /// runs script by calling Lua function App.run()
   void Run()
   {
      m_scriptWorkerThread.GetStrand().post([&]()
      {
         Lua::Table app = m_scheduler.GetThread().GetTable(_T("App"));
         Lua::Function func = app.GetValue(_T("run")).Get<Lua::Function>();

         std::vector<Lua::Value> vecParam;
         vecParam.push_back(Lua::Value(app));

         m_scheduler.StartMainThread(func, vecParam);
      });
   }

   /// cancels all handlers currently registered
   void CancelHandlers()
   {
      m_spCanonControlLuaBindings->StopTimer();

      m_scriptWorkerThread.GetStrand().post([&]()
      {
         m_spSystemLuaBindings->CancelHandlers();
         m_spCanonControlLuaBindings->CancelHandlers();
      });
   }

   /// cleans up bindings
   void CleanupBindings()
   {
      m_scriptWorkerThread.GetStrand().post([&]()
      {
         // set these to nil, to allow garbage collection
         GetState().AddValue(_T("print"), Lua::Value());
         GetState().AddValue(_T("Sys"), Lua::Value());
         GetState().AddValue(_T("App"), Lua::Value());

         GetState().CollectGarbage();

         // SystemLuaBindings must not be used in handlers anymore
         ATLASSERT(m_spSystemLuaBindings.use_count() == 1);

         // this calls SystemLuaBindings::CleanupBindings()
         m_spSystemLuaBindings.reset();

         // CanonControlLuaBindings must not be used in handlers anymore
         ATLASSERT(m_spCanonControlLuaBindings.use_count() == 1);

         // this calls CanonControlLuaBindings::CleanupBindings()
         m_spCanonControlLuaBindings.reset();
      });
   }

   /// resets status to "idle", e.g. when currently yielding
   void ResetStatus()
   {
      m_scriptWorkerThread.GetStrand().post([&]()
      {
         m_scheduler.ResetMainThread();
      });
   }

   /// stops Lua worker thread
   void StopThread()
   {
      m_scriptWorkerThread.Stop();
   }

private:
   /// initializes built-in libs
   void InitBuiltinLibs()
   {
      Lua::State& state = GetState();

      // set up built-in libs
      state.RequireLib(LUA_BASICLIBNAME);
      state.RequireLib(LUA_COLIBNAME);
      state.RequireLib(LUA_STRLIBNAME);
      state.RequireLib(LUA_UTF8LIBNAME);
      state.RequireLib(LUA_TABLIBNAME);
      state.RequireLib(LUA_MATHLIBNAME);
      state.RequireLib(LUA_OSLIBNAME);

      // omitted:
      // package (can load external scripts)
      // io (can write files)
      // debug (can modify own code)
   }

   /// "sandboxes" the built-in libraries by removing functions not deemed
   /// as "secure". The requirements are:
   /// - Lua code must not load other code outside the loaded script
   /// - Lua code must not load other libraries than specified
   /// - Lua code must not execute external code or call the OS
   /// Non-requirements are:
   /// - Lua code may modify its environment (it's the only script running)
   /// \see http://lua-users.org/wiki/SandBoxes
   void SandboxBuiltinLibs()
   {
      Lua::State& state = GetState();

      // basic lib
      state.AddValue(_T("dofile"), Lua::Value());
      state.AddValue(_T("load"), Lua::Value());
      state.AddValue(_T("loadfile"), Lua::Value());

      // os lib
      Lua::Table os = state.GetTable(_T("os"));
      os.AddValue(_T("execute"), Lua::Value());
      os.AddValue(_T("exit"), Lua::Value());
      os.AddValue(_T("getenv"), Lua::Value());
      os.AddValue(_T("remove"), Lua::Value());
      os.AddValue(_T("rename"), Lua::Value());
      os.AddValue(_T("tmpname"), Lua::Value());
   }

   /// initializes global functions
   void InitGlobalFunctions()
   {
      // global print() function
      GetState().AddFunction(_T("print"),
         std::bind(&Impl::Print, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
   }

   /// inits extra bindings stored in other classes
   void InitExtraBindings()
   {
      Lua::State& state = GetState();

      m_spSystemLuaBindings.reset(
         new SystemLuaBindings(m_scheduler, m_scriptWorkerThread.GetStrand()));

      m_spSystemLuaBindings->InitBindings();


      m_spCanonControlLuaBindings.reset(
         new CanonControlLuaBindings(state, m_scriptWorkerThread.GetStrand()));

      m_spCanonControlLuaBindings->InitBindings();
   }

   // global functions

   /// prints values as output debug string
   std::vector<Lua::Value> Print(Lua::State& state, const std::vector<Lua::Value>& vecParams)
   {
      lua_State* L = state.GetState();

      lua_concat(L, vecParams.size());
      CString cszText = lua_tostring(L, -1);

      ATLTRACE(_T("%s\n"), cszText.GetString());

      if (m_fnOutputDebugString != nullptr)
         m_fnOutputDebugString(cszText);

      return std::vector<Lua::Value>();
   }

private:
   /// scheduler
   LuaScheduler m_scheduler;

   /// output debug string handler
   T_fnOutputDebugString m_fnOutputDebugString;

   /// bindings for System library
   std::shared_ptr<SystemLuaBindings> m_spSystemLuaBindings;

   /// bindings for CanonControl library
   std::shared_ptr<CanonControlLuaBindings> m_spCanonControlLuaBindings;

   /// worker thread that runs all Lua scripts
   LuaScriptWorkerThread m_scriptWorkerThread;
};

CameraScriptProcessor::CameraScriptProcessor()
:m_spImpl(new Impl)
{
   m_spImpl->InitBindings();
}

CameraScriptProcessor::~CameraScriptProcessor()
{
   ATLASSERT(m_spImpl != nullptr);

   try
   {
      Stop();
   }
   catch (...)
   {
   }

   m_spImpl->CleanupBindings();

   m_spImpl->StopThread();

   // Impl must not be used in handlers anymore
   ATLASSERT(m_spImpl.use_count() == 1);
   m_spImpl.reset();
}

LuaScheduler& CameraScriptProcessor::GetScheduler()
{
   ATLASSERT(m_spImpl != nullptr);

   return m_spImpl->GetScheduler();
}

void CameraScriptProcessor::SetOutputDebugStringHandler(T_fnOutputDebugString fnOutputDebugString)
{
   ATLASSERT(m_spImpl != nullptr);

   m_spImpl->SetOutputDebugStringHandler(fnOutputDebugString);
}

void CameraScriptProcessor::LoadScript(const CString& cszFilename)
{
   ATLASSERT(m_spImpl != nullptr);

   if (m_spImpl->GetScheduler().CurrentExecutionState() != LuaScheduler::stateIdle)
      Stop();

   m_spImpl->LoadFile(cszFilename);
}

void CameraScriptProcessor::LoadSourceString(const CString& cszLuaSource)
{
   ATLASSERT(m_spImpl != nullptr);

   if (m_spImpl->GetScheduler().CurrentExecutionState() != LuaScheduler::stateIdle)
      Stop();

   m_spImpl->LoadSourceString(cszLuaSource);
}

/// \details calls App:run() of loaded script
void CameraScriptProcessor::Run()
{
   ATLASSERT(m_spImpl != nullptr);

   if (m_spImpl->GetScheduler().CurrentExecutionState() != LuaScheduler::stateIdle)
      Stop();

   m_spImpl->Run();
}

void CameraScriptProcessor::Stop()
{
   ATLASSERT(m_spImpl != nullptr);

   m_spImpl->CancelHandlers();

   m_spImpl->ResetStatus();

   m_spImpl->GetScheduler().CurrentExecutionState(LuaScheduler::stateIdle);
}
