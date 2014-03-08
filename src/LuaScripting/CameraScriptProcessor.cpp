//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CameraScriptProcessor.cpp Camera Lua script processor
//

// includes
#include "stdafx.h"
#include "CameraScriptProcessor.hpp"
#include "Lua.hpp"
#include "Instance.hpp"
#include "SourceInfo.hpp"
#include <lua.h>

/// implementation class, mostly for bindings
class CameraScriptProcessor::Impl : public std::enable_shared_from_this<CameraScriptProcessor::Impl>
{
public:
   /// ctor
   Impl()
      :m_instance(Instance::Get())
   {
   }

   /// returns Lua state object
   Lua::State& GetState() throw() { return m_state; }

   /// sets output debug string handler
   void SetOutputDebugStringHandler(T_fnOutputDebugString fnOutputDebugString)
   {
      m_fnOutputDebugString = fnOutputDebugString;
   }

   /// inits bindings to CanonControl
   void InitBindings()
   {
      // global print() function
      GetState().AddFunction(_T("print"),
         std::bind(&Impl::Print, shared_from_this(), std::placeholders::_1));

      /// global Sys table
      Lua::Table sys = GetState().AddTable(_T("Sys"));

      /// local instance = Sys:getInstance()
      sys.AddFunction("getInstance",
         std::bind(&Impl::LuaGetInstance, shared_from_this()));
   }

private:
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

   /// local instance = Sys:getInstance()
   /// instance:getVersion()
   /// instance:enumerateDevices()
   /// instance:asyncWaitForCamera(callbackFunction)
   std::vector<Lua::Value> LuaGetInstance()
   {
      Lua::Table instance = GetState().AddTable(_T(""));

      instance.AddFunction("getVersion",
         std::bind(&Impl::InstanceGetVersion, shared_from_this()));

      instance.AddFunction("enumerateDevices",
         std::bind(&Impl::InstanceEnumerateDevices, shared_from_this()));

      instance.AddFunction("asyncWaitForCamera",
         std::bind(&Impl::InstanceAsyncWaitForCamera, shared_from_this(), std::placeholders::_1));

      std::vector<Lua::Value> vecRetValues;

      vecRetValues.push_back(Lua::Value(instance));

      return vecRetValues;
   }

   /// local version = Instance:getVersion()
   std::vector<Lua::Value> InstanceGetVersion()
   {
      std::vector<Lua::Value> vecRetValues;
      vecRetValues.push_back(Lua::Value(m_instance.Version()));

      return vecRetValues;
   }

   /// local sourceInfoArray = instance:enumerateDevices()
   std::vector<Lua::Value> InstanceEnumerateDevices()
   {
      std::vector<std::shared_ptr<SourceInfo>> vecSourceInfo;
      m_instance.EnumerateDevices(vecSourceInfo);

      std::vector<Lua::Value> vecRetValues;

      if (!vecSourceInfo.empty())
      {
         Lua::Table table = GetState().AddTable(_T(""));

         size_t uiIndex = 0;
         std::for_each(vecSourceInfo.begin(), vecSourceInfo.end(), [&](std::shared_ptr<SourceInfo> spSourceInfo){
            AddSourceInfo(table, uiIndex++, spSourceInfo);
         });

         vecRetValues.push_back(Lua::Value(table));
      }

      return vecRetValues;
   }

   /// instance:asyncWaitForCamera(callbackFunction)
   std::vector<Lua::Value> InstanceAsyncWaitForCamera(const std::vector<Lua::Value>& vecParams)
   {
      if (vecParams.size() != 1)
         throw Lua::Exception(_T("instance:asyncWaitForCamera() needs callback parameter"), GetState().GetState());

      // TODO

      return std::vector<Lua::Value>();
   }

   /// adds a source info table to given table
   /// { name = "camera name", function open() ... end }
   void AddSourceInfo(Lua::Table& table, size_t uiIndex, std::shared_ptr<SourceInfo> spSourceInfo)
   {
      Lua::Table sourceInfo = GetState().AddTable(_T(""));

      sourceInfo.AddValue(_T("name"), Lua::Value(spSourceInfo->Name()));

      sourceInfo.AddFunction("open",
         std::bind(&Impl::SourceInfoOpen, shared_from_this(), spSourceInfo));

      CString cszIndex;
      cszIndex.Format(_T("%u"), uiIndex);

      table.AddValue(cszIndex, Lua::Value(table));
   }

   /// local sourceDevice = sourceInfo:open()
   std::vector<Lua::Value> SourceInfoOpen(std::shared_ptr<SourceInfo> spSourceInfo)
   {
      std::vector<Lua::Value> vecRetValues;

      // TODO

      return vecRetValues;
   }

private:
   /// Lua state
   Lua::State m_state;

   /// CanonControl instance
   Instance m_instance;

   /// output debug string handler
   T_fnOutputDebugString m_fnOutputDebugString;
};

CameraScriptProcessor::CameraScriptProcessor()
:m_spImpl(new Impl)
{
   m_spImpl->InitBindings();
}

void CameraScriptProcessor::SetOutputDebugStringHandler(T_fnOutputDebugString fnOutputDebugString)
{
   ATLASSERT(m_spImpl != nullptr);

   m_spImpl->SetOutputDebugStringHandler(fnOutputDebugString);
}

void CameraScriptProcessor::LoadScript(const CString& cszFilename)
{
   ATLASSERT(m_spImpl != nullptr);

   Stop();
   m_spImpl->GetState().LoadFile(cszFilename);
}

/// \details calls App:run() of loaded script
void CameraScriptProcessor::Run()
{
   ATLASSERT(m_spImpl != nullptr);

   Stop();

   Lua::Table app = m_spImpl->GetState().GetTable(_T("App"));

   std::vector<Lua::Value> vecParam;
   app.CallFunction(_T("run"), 0, vecParam);
}

void CameraScriptProcessor::Stop()
{
   ATLASSERT(m_spImpl != nullptr);

   // TODO impl
}
