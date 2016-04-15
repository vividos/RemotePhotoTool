//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.Instance.hpp Instance wrapper class
//
#pragma once

// includes
#include "Instance.hpp"

namespace RemotePhotoTool
{
   namespace CameraControl
   {
      ref class SourceInfo;

      public ref class Instance
      {
      public:
         delegate void CameraAddedHandler();

         static void EnableLogging(bool enable, System::String^ logFilePath);

         static Instance^ Get();

         property System::String^ Version
         {
            System::String^ get();
         }

         void AsyncWaitForCamera();

         void AsyncWaitForCamera(CameraAddedHandler^ handler);

         System::Collections::Generic::List<SourceInfo^>^ EnumerateDevices();

         /// Idle processing
         static void OnIdle();

         /// finalizer
         !Instance();

         /// dtor
         ~Instance();

      private:
         /// ctor; used in Instance.Get()
         Instance();

      private:
         /// instance implementation
         ::Instance* m_instance;
      };
   }
}
