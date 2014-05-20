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


class HDRPhotoModeManager
{
public:
   /// ctor
   HDRPhotoModeManager(IPhotoModeViewHost& host, HWND& hWnd) throw()
      :m_host(host),
      m_hWnd(hWnd),
      m_bAEBInProgress(false),
      m_bViewfinderActiveBeforeStart(false),
      m_uiCurrentAEBShutterSpeed(0)
   {
   }

   bool Init(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   bool IsStarted() const throw() { return m_bAEBInProgress; }

   const std::vector<ImageProperty>& ShutterSpeedValues() const throw() { return m_vecAEBShutterSpeedValues; }

   bool CheckManualMode();

   /// recalculates AEB shutter speed list
   void RecalcAEBShutterSpeedList(size_t uiNumShots);

   /// releases remote control for first AEB image
   void ReleaseAEBFirst();

   /// releases remote control for next AEB image
   void ReleaseAEBNext();


   void OnReleasedAEBLast();

private:
   /// called when next AEB image has finished transfer
   void OnFinishedTransferNextAEB(const ShutterReleaseSettings& settings);

   /// called when last AEB image has finished transfer
   void OnFinishedTransferLastAEB(const ShutterReleaseSettings& settings);

private:
   /// host access
   IPhotoModeViewHost& m_host;

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


class PanoramaPhotoModeManager
{
public:
   PanoramaPhotoModeManager(IPhotoModeViewHost& host, HWND& hWnd)
      :m_host(host),
      m_hWnd(hWnd),
      m_bStarted(false)
   {
   }

   bool Init(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   bool IsStarted() const throw() { return m_bStarted; }

   void StartPanorama();

   void StopPanorama();

   /// starts hugin
   void StartHugin();

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
