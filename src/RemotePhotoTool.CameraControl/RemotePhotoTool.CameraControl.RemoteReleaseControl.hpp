//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.RemoteReleaseControl.hpp RemoteReleaseControl wrapper class
//
#pragma once

// includes
#include "Instance.hpp"

class RemoteReleaseControl;

namespace RemotePhotoTool
{
   namespace CameraControl
   {
      ref class Viewfinder;
      ref class BulbReleaseControl;

      public ref class RemoteReleaseControl
      {
      public:
         // TODO implement

         Viewfinder^ StartViewfinder();

         /// starts bulb release; only supported when GetCapability(BulbMode) returned true
         BulbReleaseControl^ StartBulb();

         /// finalizer
         !RemoteReleaseControl();

         /// dtor
         ~RemoteReleaseControl();

      internal:
         /// ctor; used in SourceDevice.EnterReleaseControl()
         RemoteReleaseControl(std::shared_ptr<::RemoteReleaseControl> remoteReleaseControl);

      private:
         /// implementation
         std::shared_ptr<::RemoteReleaseControl>* m_remoteReleaseControl;
      };
   }
}
