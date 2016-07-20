//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.Viewfinder.hpp Viewfinder wrapper class
//
#pragma once

// includes
#include "Instance.hpp"

class Viewfinder;

namespace RemotePhotoTool
{
   namespace CameraControl
   {
      public ref class Viewfinder
      {
      public:
         // TODO implement

         /// finalizer
         !Viewfinder();

         /// dtor
         ~Viewfinder();

      internal:
         /// ctor; used in RemoteReleaseControl.StartViewfinder()
         Viewfinder(std::shared_ptr<::Viewfinder> viewfinder);

      private:
         std::shared_ptr<::Viewfinder>* m_viewfinder;
      };
   }
}
