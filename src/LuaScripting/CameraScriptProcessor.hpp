//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file CameraScriptProcessor.hpp Camera Lua script processor
//
#pragma once

// forward references
class LuaScheduler;

/// camera Lua script processor
class CameraScriptProcessor
{
public:
   /// ctor
   CameraScriptProcessor();
   /// dtor
   virtual ~CameraScriptProcessor() throw();

   /// returns Lua scheduler
   LuaScheduler& GetScheduler() throw();

   /// function type to output debug strings
   typedef std::function<void(const CString&)> T_fnOutputDebugString;

   /// sets output debug string handler
   void SetOutputDebugStringHandler(T_fnOutputDebugString fnOutputDebugString);

   /// loads new script into processor
   void LoadScript(const CString& cszFilename);

   /// loads Lua script source string into processor
   void LoadSourceString(const CString& cszLuaSource);

   /// runs script in background thread
   void Run();

   /// stops running script
   void Stop();

private:
   class Impl;

   /// implementation
   std::shared_ptr<Impl> m_spImpl;
};
