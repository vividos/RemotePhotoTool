//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.SourceDevice.hpp SourceDevice wrapper class
//
#pragma once

// includes
#include "Instance.hpp"

class SourceDevice;

namespace RemotePhotoTool
{
   namespace CameraControl
   {
      public ref class SourceDevice
      {
      public:
         property System::String^ ModelName
         {
            System::String^ get();
         }

         property System::String^ SerialNumber
         {
            System::String^ get();
         }

         /// finalizer
         !SourceDevice();

         /// dtor
         ~SourceDevice();

      internal:
         /// ctor; used in SourceInfo.Open()
         SourceDevice(std::shared_ptr<::SourceDevice> sourceDevice);

      private:
         std::shared_ptr<::SourceDevice>* m_sourceDevice;
      };
   }
}
