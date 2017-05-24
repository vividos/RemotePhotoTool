//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.BulbReleaseControl.hpp BulbReleaseControl wrapper class
//
#pragma once

// includes
#include "Instance.hpp"

class BulbReleaseControl;

namespace RemotePhotoTool
{
   namespace CameraControl
   {
      public ref class BulbReleaseControl
      {
      public:
         /// property elapsed time
         property System::TimeSpan ElapsedTime
         {
            System::TimeSpan get();
         }

         /// stops bulb shooting; usefuly if you don't want to Dispose() this
         /// object yet.
         void Stop();

         /// finalizer
         !BulbReleaseControl();

         /// dtor
         ~BulbReleaseControl();

      internal:
         /// ctor; used in RemoteReleaseControl.StartBulbReleaseControl()
         BulbReleaseControl(std::shared_ptr<::BulbReleaseControl> bulbReleaseControl);

      private:
         std::shared_ptr<::BulbReleaseControl>* m_bulbReleaseControl;
      };
   }
}
