//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.SourceInfo.hpp SourceInfo wrapper class
//
#pragma once

// includes
#include "Instance.hpp"

class SourceInfo;

namespace RemotePhotoTool
{
   namespace CameraControl
   {
      ref class SourceDevice;

      public ref class SourceInfo
      {
      public:
         property System::String^ Name
         {
            System::String^ get();
         }

         property System::String^ DeviceId
         {
            System::String^ get();
         }

         SourceDevice^ Open();

         /// finalizer
         !SourceInfo();

         /// dtor
         ~SourceInfo();

      internal:
         /// ctor; used in Instance.EnumerateDevices()
         SourceInfo(std::shared_ptr<::SourceInfo> sourceInfo);

      private:
         std::shared_ptr<::SourceInfo>* m_sourceInfo;
      };
   }
}
