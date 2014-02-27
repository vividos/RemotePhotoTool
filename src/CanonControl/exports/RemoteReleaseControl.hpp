//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file RemoteReleaseControl.hpp Canon control - Remote release control
//
#pragma once

// includes
#include "ImageProperty.hpp"

// forward references
class ShutterReleaseSettings;
class Viewfinder;
class BulbReleaseControl;

/// remote release control
class RemoteReleaseControl
{
public:
   /// dtor
   virtual ~RemoteReleaseControl() throw() {}

   ////////////////////////////////////////////////
   // enumerations
   ////////////////////////////////////////////////

   /// capabilities in remote capture mode
   enum T_enRemoteCapability
   {
      capChangeShootingParameter = 0,  ///< can change shooting parameter at all
      capChangeShootingMode = 1,       ///< ability to change shooting modes
      capZoomControl = 2,              ///< can control zoom
      capViewfinder = 3,               ///< can fetch live view image
      capReleaseWhileViewfinder = 4,   ///< can capture during live view
      capAFLock = 5,                   ///< supports AF lock/unlock
      capBulbMode = 6,                 ///< supports bulb mode
      capUILock = 7,                   ///< supports UI lock
   };

   /// property event type
   enum T_enPropertyEvent
   {
      propEventPropertyChanged = 0,       ///< property on camera changed
      propEventPropertyDescChanged = 1,   ///< possible property values changed
      propEventInvalid
   };

   /// state event type
   enum T_enStateEvent
   {
      stateEventCameraShutdown = 0, ///< camera has shut down
      stateEventRotationAngle = 1,  ///< rotation angle has changed
      stateEventMemoryCardSlotOpen = 2,   ///< memory card slot was opened
      stateEventReleaseError = 3,   ///< error while Release(), e.g. focus couldn't be determined
      stateEventBulbExposureTime = 4,  ///< signals elapsed bulb exposure time (in seconds)
      stateEventInternalError = 5,  ///< an internal error in camera occured
      stateEventInvalid
   };

   /// download event type
   enum T_enDownloadEvent
   {
      downloadEventStarted = 0,     ///< download of image has started
      downloadEventInProgress = 1,  ///< download of image is in progress
      downloadEventFinished = 2,    ///< download of image has finished
   };

   /// camera command
   enum T_enCameraCommand
   {
      /// adjusts focus (may only work in viewfinder for some camera models)
      commandAdjustFocus = 0,

      /// adjusts white balance
      commandAdjustWhiteBalance = 1,
   };

   /// shooting mode type
   enum T_enShootingMode
   {
      shootingModeP = 0,   ///< program mode
      shootingModeTv = 1,  ///< shutter speed priority mode
      shootingModeAv = 2,  ///< aperture priority mode
      shootingModeM = 3,   ///< manual mode
   };

   ////////////////////////////////////////////////
   // typedefs
   ////////////////////////////////////////////////

   /// callback function type to call when property changed
   typedef std::function<void(T_enPropertyEvent, unsigned int)> T_fnOnPropertyChanged;

   /// callback function type to call when camera state changed
   typedef std::function<void(T_enStateEvent, unsigned int)> T_fnOnStateChanged;

   /// callback function type to call when download state changed
   typedef std::function<void(T_enDownloadEvent, unsigned int)> T_fnOnDownloadChanged;

   ////////////////////////////////////////////////
   // methods
   ////////////////////////////////////////////////

   /// returns capability in remote capture mode
   virtual bool GetCapability(T_enRemoteCapability enRemoteCapability) const throw() = 0;

   /// sets default release settings; used when user presses shutter release on camera
   virtual void SetDefaultReleaseSettings(const ShutterReleaseSettings& settings) = 0;

   ////////////////////////////////////////////////
   // events
   ////////////////////////////////////////////////

   // property events

   /// adds property event handler
   virtual int AddPropertyEventHandler(T_fnOnPropertyChanged fnOnPropertyChanged) = 0;

   /// removes property event handler again, by id returned from AddPropertyEventHandler()
   virtual void RemovePropertyEventHandler(int iHandlerId) = 0;

   // state events

   /// adds state event handler
   virtual int AddStateEventHandler(T_fnOnStateChanged fnOnStateChanged) = 0;

   /// removes state event handler again, by id returned from AddStateEventHandler()
   virtual void RemoveStateEventHandler(int iHandlerId) = 0;

   // download events

   /// adds download event handler
   virtual int AddDownloadEventHandler(T_fnOnDownloadChanged fnOnDownloadChanged) = 0;

   /// removes download event handler again, by id returned from AddDownloadEventHandler()
   virtual void RemoveDownloadEventHandler(int iHandlerId) = 0;

   ////////////////////////////////////////////////
   // image property methods
   ////////////////////////////////////////////////

   /// maps image property type to image property id
   virtual unsigned int MapImagePropertyTypeToId(T_enImagePropertyType enImagePropertyType) const = 0;

   /// maps shooting mode type to image property value
   virtual ImageProperty MapShootingModeToImagePropertyValue(T_enShootingMode enShootingMode) const = 0;

   /// enumerate all possible image properties that can be get/set
   virtual std::vector<unsigned int> EnumImageProperties() const = 0;

   /// retrieves image property by id
   virtual ImageProperty GetImageProperty(unsigned int uiImagePropertyId) const = 0;

   /// sets image property
   virtual void SetImageProperty(const ImageProperty& imageProperty) = 0;

   /// enumerates possible values of given image property
   virtual void EnumImagePropertyValues(unsigned int uiImagePropertyId, std::vector<ImageProperty>& vecValues) const = 0;

   ////////////////////////////////////////////////
   // viewfinder
   ////////////////////////////////////////////////

   /// starts live view
   virtual std::shared_ptr<Viewfinder> StartViewfinder() const = 0;

   ////////////////////////////////////////////////
   // release
   ////////////////////////////////////////////////

   /// returns number of available shots on the memory card
   virtual unsigned int NumAvailableShots() const = 0;

   /// sends camera command // TODO move to Viewfinder?
   virtual void SendCommand(T_enCameraCommand enCameraCommand) = 0;

   /// presses the shutter release, taking a photo using the set properties and given shutter release settings
   virtual void Release(const ShutterReleaseSettings& settings) = 0;

   /// starts bulb release; only supported when GetCapability(capBulbMode) returned true
   virtual std::shared_ptr<BulbReleaseControl> StartBulb(const ShutterReleaseSettings& settings) = 0;
};
