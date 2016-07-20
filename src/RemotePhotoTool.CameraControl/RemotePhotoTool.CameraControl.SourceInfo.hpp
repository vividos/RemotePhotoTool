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

      /// infos about a source camera
      public ref class SourceInfo
      {
      public:
         /// returns name of source camera
         property System::String^ Name
         {
            System::String^ get();
         }

         /// returns device id of source camera
         property System::String^ DeviceId
         {
            System::String^ get();
         }

         /// opens source device for use
         SourceDevice^ Open();

         /// finalizer
         !SourceInfo();

         /// dtor
         ~SourceInfo();

      internal:
         /// ctor; used in Instance.EnumerateDevices()
         SourceInfo(std::shared_ptr<::SourceInfo> sourceInfo);

      private:
         /// implementation
         std::shared_ptr<::SourceInfo>* m_sourceInfo;
      };
   }
}
