//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
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
      enum class ImagePropertyType;
      ref class ShutterReleaseSettings;
      ref class ImageProperty;
      ref class Viewfinder;
      ref class BulbReleaseControl;

      public ref class RemoteReleaseControl
      {
      public:
         ////////////////////////////////////////////////
         // enumerations
         ////////////////////////////////////////////////

         /// remote capabilities
         enum class RemoteCapability
         {
            ChangeShootingParameter = 0,  ///< can change shooting parameter at all
            ChangeShootingMode = 1,       ///< ability to change shooting modes
            ZoomControl = 2,              ///< can control zoom
            Viewfinder = 3,               ///< can fetch live view image
            ReleaseWhileViewfinder = 4,   ///< can capture during live view
            AFLock = 5,                   ///< supports AF lock/unlock
            BulbMode = 6,                 ///< supports bulb mode
            UILock = 7,                   ///< supports UI lock
         };

         /// property event type
         enum class PropertyEvent
         {
            PropertyChanged = 0,       ///< property on camera changed
            PropertyDescChanged = 1,   ///< possible property values changed
            Invalid
         };

         /// state event type
         enum class StateEvent
         {
            CameraShutdown = 0,     ///< camera has shut down
            RotationAngle = 1,      ///< rotation angle has changed
            MemoryCardSlotOpen = 2, ///< memory card slot was opened
            ReleaseError = 3,       ///< error while Release(), e.g. focus couldn't be determined
            BulbExposureTime = 4,   ///< signals elapsed bulb exposure time (in seconds)
            InternalError = 5,      ///< an internal error in camera occured
            Invalid
         };

         /// download event type
         enum class DownloadEvent
         {
            Started = 0,      ///< download of image has started
            InProgress = 1,   ///< download of image is in progress
            Finished = 2,     ///< download of image has finished
         };

         /// camera command
         enum class CameraCommand
         {
            /// adjusts focus (may only work in viewfinder for some camera models)
            AdjustFocus = 0,

            /// adjusts white balance
            AdjustWhiteBalance = 1,

            /// adjusts exposure
            AdjustExposure = 2,
         };

         /// shooting mode type
         enum class ShootingMode
         {
            P = 0,   ///< program mode
            Tv = 1,  ///< shutter speed priority mode
            Av = 2,  ///< aperture priority mode
            M = 3,   ///< manual mode
         };

         ////////////////////////////////////////////////
         // delegates
         ////////////////////////////////////////////////

         /// callback function type to call when property state changed
         delegate void PropertyChangedHandler(PropertyEvent propertyEvent, unsigned int propertyId);

         /// callback function type to call when camera state changed
         delegate void StateChangedHandler(StateEvent stateEvent, unsigned int stateId);

         /// callback function type to call when download state changed
         delegate void DownloadChangedHandler(DownloadEvent downloadEvent, unsigned int progressPercent);

         ////////////////////////////////////////////////
         // methods
         ////////////////////////////////////////////////

         /// returns capability in remote capture mode
         bool GetCapability(RemoteCapability remoteCapability);

         /// sets default release settings; used when calling Release(), or user presses shutter release on camera
         void SetReleaseSettings(ShutterReleaseSettings^ settings);

         ////////////////////////////////////////////////
         // events
         ////////////////////////////////////////////////

         /// property changed event
         event PropertyChangedHandler^ PropertyChanged;

         /// state changed event
         event StateChangedHandler^ StateChanged;

         /// download changed event
         event DownloadChangedHandler^ DownloadChanged;

         ////////////////////////////////////////////////
         // image property methods
         ////////////////////////////////////////////////

         /// gets image property by image property ID
         ImageProperty^ GetImageProperty(unsigned int imagePropertyId);

         /// gets image property by image property type
         ImageProperty^ GetImagePropertyByType(ImagePropertyType imagePropertyType);

         /// maps shooting mode type to image property value
         ImageProperty^ GetShootingModeImageProperty(ShootingMode shootingMode);

         /// enumerate all possible image properties that can be get/set
         System::Collections::Generic::List<ImageProperty^>^ EnumImageProperties();

         /// sets image property value
         void SetImageProperty(ImageProperty^ imageProperty);

         ////////////////////////////////////////////////
         // viewfinder
         ////////////////////////////////////////////////

         /// starts viewfinder; only supported when GetCapability(Viewfinder) returned true
         Viewfinder^ StartViewfinder();

         ////////////////////////////////////////////////
         // release
         ////////////////////////////////////////////////

         /// returns number of available shots on the memory card
         property unsigned int NumAvailableShots
         {
            unsigned int get();
         }

         /// sends camera command
         void SendCommand(CameraCommand cameraCommand);

         /// presses the shutter release, taking a photo using the set properties and given shutter release settings
         void Release();

         /// starts bulb release; only supported when GetCapability(BulbMode) returned true
         BulbReleaseControl^ StartBulb();

         ////////////////////////////////////////////////
         // cleanup
         ////////////////////////////////////////////////

         /// closes remote release control; no further method calls are possible on this instance
         void Close();

         /// finalizer
         !RemoteReleaseControl();

         /// dtor
         ~RemoteReleaseControl();

      internal:
         /// ctor; used in SourceDevice.EnterReleaseControl()
         RemoteReleaseControl(std::shared_ptr<::RemoteReleaseControl> remoteReleaseControl);

         /// initializes event handler
         void InitEventHandler();

         /// handler called when a property has changed
         void OnPropertyChanged(PropertyEvent propertyEvent, unsigned int propertyId);

         /// handler called when camera state has changed
         void OnStateChanged(StateEvent stateEvent, unsigned int stateId);

         /// handler called when download progress changed
         void OnDownloadChanged(DownloadEvent downloadEvent, unsigned int progressPercent);

      private:
         /// implementation
         std::shared_ptr<::RemoteReleaseControl>* m_remoteReleaseControl;

         /// handler id for property event
         int m_propertyEventHandlerId;

         /// handler id for state event
         int m_stateEventHandlerId;

         /// handler id for download event
         int m_downloadEventHandlerId;
      };
   }
}
