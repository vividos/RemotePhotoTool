//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
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
         // TODO implement

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
