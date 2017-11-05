//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PhotoModeManager.hpp Photo mode manager classes
//
#pragma once

// includes
#include "ImageProperty.hpp"

// forward references
class IPhotoModeViewHost;
class RemoteReleaseControl;
class Viewfinder;
class ShutterReleaseSettings;

/// message sent when next HDR image can be captured
#define WM_HDR_AEB_NEXT (WM_USER+1)

/// message sent when last HDR image was captured
#define WM_HDR_AEB_LAST (WM_USER+2)


/// HDR photo mode manager
class HDRPhotoModeManager
{
public:
   /// ctor
   HDRPhotoModeManager(IPhotoModeViewHost& host, HWND& hWnd)
      :m_host(host),
      m_hWnd(hWnd),
      m_bAEBInProgress(false),
      m_bViewfinderActiveBeforeStart(false),
      m_uiCurrentAEBShutterSpeed(0)
   {
   }

   /// inits photo manager
   bool Init(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   /// returns if AEB photo taking is in progress
   bool IsStarted() const { return m_bAEBInProgress; }

   /// returns shutter speed values for bracketing
   const std::vector<ImageProperty>& ShutterSpeedValues() const { return m_vecAEBShutterSpeedValues; }

   /// checks if manual mode is switched on
   bool CheckManualMode();

   /// recalculates AEB shutter speed list
   void RecalcAEBShutterSpeedList(size_t uiNumShots);

   /// releases remote control for first AEB image
   void ReleaseAEBFirst();

   /// releases remote control for next AEB image
   void ReleaseAEBNext();

   /// called on last AEB release
   void OnReleasedAEBLast();

private:
   /// called when next AEB image has finished transfer
   void OnFinishedTransferNextAEB(const ShutterReleaseSettings& settings);

   /// called when last AEB image has finished transfer
   void OnFinishedTransferLastAEB(const ShutterReleaseSettings& settings);

private:
   /// host access
   IPhotoModeViewHost& m_host;

   /// parent window
   HWND& m_hWnd;

   /// remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;

   /// viewfinder; used as mirror lockup
   std::shared_ptr<Viewfinder> m_spViewfinder;

   /// indicates if AEB release is in progress
   bool m_bAEBInProgress;

   /// indicates if viewfinder was active before start
   bool m_bViewfinderActiveBeforeStart;

   /// indicates index of current shutter speed, from m_vecAEBShutterSpeedValues
   size_t m_uiCurrentAEBShutterSpeed;

   /// shutter speed values for AEB shots
   std::vector<ImageProperty> m_vecAEBShutterSpeedValues;

   /// filenames of bracketed shots
   std::vector<CString> m_vecAEBFilenameList;
};


/// Panorama photo mode manager
class PanoramaPhotoModeManager
{
public:
   /// ctor
   PanoramaPhotoModeManager(IPhotoModeViewHost& host, HWND& hWnd)
      :m_host(host),
      m_hWnd(hWnd),
      m_bStarted(false)
   {
   }

   /// inits photo manager
   bool Init(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   /// returns if panorama photo taking is in progress
   bool IsStarted() const { return m_bStarted; }

   /// starts panorama photo taking
   void StartPanorama();

   /// stops panorama photo taking
   void StopPanorama();

   /// starts hugin
   void StartHugin();

   /// releases shutter for panorama
   void ReleasePanorama();

private:
   /// called when image transfer is finished
   void OnFinishedTransfer(const ShutterReleaseSettings& settings);

private:
   /// host
   IPhotoModeViewHost& m_host;

   /// parent window for dialogs
   HWND& m_hWnd;

   /// remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;

   /// panorama started?
   bool m_bStarted;

   /// filenames of panorama shots
   std::vector<CString> m_vecPanoramaFilenameList;
};
