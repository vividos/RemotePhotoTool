//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file CameraScriptProcessor.hpp Camera Lua script processor
//
#pragma once

/// camera Lua script processor
class CameraScriptProcessor
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
   CameraScriptProcessor();
   /// dtor
   virtual ~CameraScriptProcessor() throw();

   /// function type to output debug strings
   typedef std::function<void(const CString&)> T_fnOutputDebugString;

   /// sets output debug string handler
   void SetOutputDebugStringHandler(T_fnOutputDebugString fnOutputDebugString);

   /// function type to notify about execution state changes
   typedef std::function<void(T_enExecutionState)> T_fnOnExecutionStateChanged;

   /// sets handler to notify about execution state changes
   void SetExecutionStateChangedHandler(T_fnOnExecutionStateChanged fnOnExecutionStateChanged);

   /// returns current execution state
   T_enExecutionState CurrentExecutionState() const throw();

   /// loads new script into processor
   void LoadScript(const CString& cszFilename);

   /// runs script in background thread
   void Run();

   /// stops running script
   void Stop();

private:
   class Impl;

   /// implementation
   std::shared_ptr<Impl> m_spImpl;
};
