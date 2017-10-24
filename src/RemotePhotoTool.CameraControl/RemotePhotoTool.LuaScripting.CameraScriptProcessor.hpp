//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file RemotePhotoTool.LuaScripting.CameraScriptProcessor.hpp Camera script processor
//
#pragma once

// includes
#include "CameraScriptProcessor.hpp"

namespace RemotePhotoTool
{
   namespace LuaScripting
   {
      /// execution state of Lua scheduler
      public enum LuaSchedulerExecutionState
      {
         StateIdle = 0,    ///< processor is idle and a function can be run
         StateRunning = 1, ///< processor is currently running
         StateYield = 2,   ///< a script yielded, and processor is waiting for resuming
         StateDebug = 3,   ///< script debugger is attached and is waiting for a script debug command
         StateError = 4    ///< processor is in an error state
      };

      public ref class CameraScriptProcessor
      {
      public:
         /// ctor
         CameraScriptProcessor();

         /// finalizer
         !CameraScriptProcessor();

         /// dtor
         ~CameraScriptProcessor();

         /// loads new script into processor
         void LoadScript(System::String^ filename);

         /// loads Lua script source string into processor
         void LoadSourceString(System::String^ luaSource);

         /// runs script in background thread
         void Run();

         /// stops running script
         void Stop();

      internal:

      private:
         std::shared_ptr<::CameraScriptProcessor>* m_cameraScriptProcessor;
      };

   }
}
