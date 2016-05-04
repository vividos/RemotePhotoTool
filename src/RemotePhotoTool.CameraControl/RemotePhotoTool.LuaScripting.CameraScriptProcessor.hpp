//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
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
