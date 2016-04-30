//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file IPhotoModeViewHost.hpp Interface for host of photo mode views
//
#pragma once

// forward references
class RemoteReleaseControl;
class ImageFileManager;
class PreviousImagesManager;
class ViewFinderView;
class ShutterReleaseSettings;
class AppSettings;

/// host interface for photo mode views
class IPhotoModeViewHost
{
public:
   /// dtor
   virtual ~IPhotoModeViewHost() throw() { }

   /// returns source device
   virtual std::shared_ptr<SourceDevice> GetSourceDevice() = 0;

   /// returns remote release control
   virtual std::shared_ptr<RemoteReleaseControl> GetRemoteReleaseControl() = 0;

   /// returns application settings
   virtual AppSettings& GetAppSettings() throw() = 0;

   /// returns image file manager
   virtual ImageFileManager& GetImageFileManager() throw() = 0;

   /// returns previous images manager
   virtual PreviousImagesManager& GetPreviousImagesManager() throw() = 0;

   /// returns viewfinder view (or nullptr if not active)
   virtual ViewFinderView* GetViewFinderView() throw() = 0;

   /// returns release settings
   virtual ShutterReleaseSettings& GetReleaseSettings() throw() = 0;

   /// sets new status text
   virtual void SetStatusText(const CString& cszText, unsigned int uiPane = 0) = 0;

   /// locks switching action modes
   virtual void LockActionMode(bool bLock) = 0;

   /// enables/disables UI elements
   virtual void EnableUI(int nID, bool bEnable) = 0;

   /// called by photo mode view when a new image has been taken
   virtual void OnTransferredImage(const CString& cszFilename) = 0;
};
