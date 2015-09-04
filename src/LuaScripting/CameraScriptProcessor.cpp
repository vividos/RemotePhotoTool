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
#include "CanonControlLuaBindings.hpp"
#include "LuaScriptWorkerThread.hpp"
#include <lua.h>

/// implementation class, mostly for bindings
class CameraScriptProcessor::Impl : public std::enable_shared_from_this<CameraScriptProcessor::Impl>
{
public:
   /// ctor
   Impl()
      :m_enExecutionState(stateIdle)
   {
   }
   /// dtor
   virtual ~Impl()
   {
   }

   /// returns Lua state object
   Lua::State& GetState() throw() { return m_state; }

   /// sets output debug string handler
   void SetOutputDebugStringHandler(T_fnOutputDebugString fnOutputDebugString)
   {
      m_fnOutputDebugString = fnOutputDebugString;

      if (m_spCanonControlLuaBindings != nullptr)
         m_spCanonControlLuaBindings->SetOutputDebugStringHandler(fnOutputDebugString);

      m_scriptWorkerThread.SetOutputDebugStringHandler(fnOutputDebugString);
   }

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

   /// inits bindings to system functions and CanonControl
   void InitBindings()
   {
      // global print() function
      GetState().AddFunction(_T("print"),
         std::bind(&Impl::Print, shared_from_this(), std::placeholders::_1));

      /// global Sys table
      {
         Lua::Table sys = GetState().AddTable(_T("Sys"));
      }

      m_spCanonControlLuaBindings.reset(
         new CanonControlLuaBindings(GetState(), m_scriptWorkerThread.GetStrand()));

      m_spCanonControlLuaBindings->InitBindings();
   }

   /// loads script file
   void LoadFile(const CString& cszFilename)
   {
      m_scriptWorkerThread.GetStrand().post([&, cszFilename]()
      {
         GetState().LoadFile(cszFilename);
      });
   }

   /// runs script by calling Lua function App.run()
   void Run()
   {
      m_scriptWorkerThread.GetStrand().post([&]()
      {
         CurrentExecutionState(stateRunning);

         Lua::Table app = GetState().GetTable(_T("App"));

         std::vector<Lua::Value> vecParam;
         app.CallFunction(_T("run"), 0, vecParam);

         CurrentExecutionState(stateIdle);
      });
   }

   /// cancels all handlers currently registered
   void CancelHandlers()
   {
      m_scriptWorkerThread.GetStrand().post([&]()
      {
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

         // CanonControlLuaBindings must not be used in handlers anymore
         ATLASSERT(m_spCanonControlLuaBindings.unique() == true);

         // this calls CanonControlLuaBindings::CleanupBindings()
         m_spCanonControlLuaBindings.reset();
      });
   }

   /// stops Lua worker thread
   void StopThread()
   {
      m_scriptWorkerThread.Stop();
   }

private:
   // global functions

   /// prints values as output debug string
   std::vector<Lua::Value> Print(const std::vector<Lua::Value>& vecParams)
   {
      lua_concat(GetState().GetState(), vecParams.size());
      CString cszText = lua_tostring(GetState().GetState(), -1);

      ATLTRACE(_T("%s\n"), cszText.GetString());

      if (m_fnOutputDebugString != nullptr)
         m_fnOutputDebugString(cszText);

      return std::vector<Lua::Value>();
   }

private:
   /// Lua state
   Lua::State m_state;

   /// current execution state
   T_enExecutionState m_enExecutionState;

   /// output debug string handler
   T_fnOutputDebugString m_fnOutputDebugString;

   /// execution state changes handler
   T_fnOnExecutionStateChanged m_fnOnExecutionStateChanged;

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

CameraScriptProcessor::~CameraScriptProcessor() throw()
{
   ATLASSERT(m_spImpl != nullptr);

   Stop();

   m_spImpl->CleanupBindings();

   m_spImpl->StopThread();

   // Impl must not be used in handlers anymore
   ATLASSERT(m_spImpl.unique() == true);
   m_spImpl.reset();
}

void CameraScriptProcessor::SetOutputDebugStringHandler(T_fnOutputDebugString fnOutputDebugString)
{
   ATLASSERT(m_spImpl != nullptr);

   m_spImpl->SetOutputDebugStringHandler(fnOutputDebugString);
}

void CameraScriptProcessor::SetExecutionStateChangedHandler(T_fnOnExecutionStateChanged fnOnExecutionStateChanged)
{
   ATLASSERT(m_spImpl != nullptr);

   m_spImpl->SetExecutionStateChangedHandler(fnOnExecutionStateChanged);
}

CameraScriptProcessor::T_enExecutionState CameraScriptProcessor::CurrentExecutionState() const throw()
{
   ATLASSERT(m_spImpl != nullptr);

   try
   {
      return m_spImpl->CurrentExecutionState();
   }
   catch (...)
   {
      return stateError;
   }
}

void CameraScriptProcessor::LoadScript(const CString& cszFilename)
{
   ATLASSERT(m_spImpl != nullptr);

   if (CurrentExecutionState() != stateIdle)
      Stop();

   m_spImpl->LoadFile(cszFilename);
}

/// \details calls App:run() of loaded script
void CameraScriptProcessor::Run()
{
   ATLASSERT(m_spImpl != nullptr);

   if (CurrentExecutionState() != stateIdle)
      Stop();

   m_spImpl->Run();
}

void CameraScriptProcessor::Stop()
{
   ATLASSERT(m_spImpl != nullptr);

   m_spImpl->CancelHandlers();

   m_spImpl->CurrentExecutionState(stateIdle);
}
