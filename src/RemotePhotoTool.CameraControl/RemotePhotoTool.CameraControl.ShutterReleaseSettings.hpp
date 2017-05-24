//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.ShutterReleaseSettings.hpp ShutterReleaseSettings wrapper class
//
#pragma once

// includes
#include "Instance.hpp"

class ShutterReleaseSettings;

namespace RemotePhotoTool
{
   namespace CameraControl
   {
      public ref class ShutterReleaseSettings
      {
      public:
         /// possible save targets
         enum class SaveTarget
         {
            Camera = 1, ///< on shutter release, image is saved to camera storage only
            Host = 2,   ///< image is transferred to host only
            Both = Camera | Host,   ///< image is saved to camera storage and transferred to host
         };

         /// handler function that is called at end of transfer
         delegate void FinishedTransferHandler(ShutterReleaseSettings^);

         /// property for filename
         property System::String^ Filename
         {
            System::String^ get();
            void set(System::String^ value);
         }

         /// property for filename
         property SaveTarget Target
         {
            SaveTarget get();
            void set(SaveTarget value);
         }

         /// property for handler for finished transfer
         property FinishedTransferHandler^ HandlerOnFinishedTransfer
         {
            FinishedTransferHandler^ get();
            void set(FinishedTransferHandler^ value);
         }


         /// ctor
         ShutterReleaseSettings(SaveTarget saveTarget /*= SaveTarget.Camera*/,
            FinishedTransferHandler^ finishedTransferHandler /*= nullptr*/);

         /// finalizer
         !ShutterReleaseSettings();

         /// dtor
         ~ShutterReleaseSettings();

      internal:
         /// ctor
         ShutterReleaseSettings(const ::ShutterReleaseSettings& ShutterReleaseSettings);

         /// returns raw shutter release settings
         ::ShutterReleaseSettings* GetRaw();

      private:
         /// implementation
         std::shared_ptr<::ShutterReleaseSettings>* m_shutterReleaseSettings;
      };
   }
}
