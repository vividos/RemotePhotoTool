//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file PhotoModeManager.hpp Photo mode manager classes
//
#pragma once

// includes
#include "ImageProperty.hpp"
#include <ATLComTime.h>

// forward references
class IPhotoModeViewHost;
class RemoteReleaseControl;
class Viewfinder;
class ShutterReleaseSettings;
class TimeLapseScheduler;

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

/// Options for time lapse photo mode
struct TimeLapseOptions
{
   /// release trigger type
   enum T_enReleaseTrigger
   {
      /// use an interval to trigger release
      releaseTriggerInterval = 0,

      /// release just after last image was transferred
      releaseAfterLastImage = 1,

      /// release manually; user input is used for trigger
      releaseManually = 2,
   };

   /// release trigger type
   T_enReleaseTrigger m_releaseTrigger;

   // scheduler options

   /// interval time, when releaseTriggerInterval is selected
   ATL::COleDateTimeSpan m_intervalTime;

   /// indicates if start time should be used
   bool m_useStartTime;

   /// start time to start taking timelapse photos
   COleDateTime m_startTime;

   /// indicates if end time should be used
   bool m_useEndTime;

   /// stop time to end taking timelapse photos
   COleDateTime m_endTime;

   // other options

   /// HDR shooting should be used when taking photos
   bool m_useHDR;

   /// ctor
   TimeLapseOptions()
   {
      m_releaseTrigger = releaseTriggerInterval;
      m_useStartTime = false;
      m_useEndTime = false;
      m_useHDR = false;
   }
};

/// \brief Timelapse photo mode manager
/// \details The photo mode manager internally uses a state machine to keep
/// note of the current state of time lapse photographing. The TimeLapseOptions
/// values control how the state machine switches its states.
class TimeLapsePhotoModeManager
{
public:
   /// ctor
   TimeLapsePhotoModeManager(IPhotoModeViewHost& host, HWND& hWnd);

   /// inits photo manager
   bool Init(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, std::function<void()> fnFinished = std::function<void()>());

   /// returns if time lapse photo taking is in progress
   bool IsStarted() const;

   /// returns time lapse options
   TimeLapseOptions& Options() { return m_options; }

   /// starts time lapse operations; returns immediately
   void Start();

   /// stops time lapse operations; returns immediately
   void Stop();

   /// triggers a manual release; only in "releaseManually" mode
   void ManualRelease();

private:
   /// sets default release settings
   bool SetReleaseSettings();

   /// runs timelapse state machine
   void RunStateMachine();

   /// called when state "started" occurs
   void OnStateStart(bool& exit);

   /// called when state "takePhoto" occurs
   void OnStateTakePhoto(bool&exit);

   /// called when state "waitTransferFinished" occurs
   void OnStateWaitTransferFinished(bool& exit);

   /// camera state event handler
   void OnStateEvent(RemoteReleaseControl::T_enStateEvent stateEvent, unsigned int extraData);

   /// called when image transfer is finished
   void OnFinishedTransfer(const ShutterReleaseSettings& settings);

private:
   /// state machine state
   enum T_enStateMachineState
   {
      notRunning,             ///< state machine is not running
      started,                ///< state machine is started
      waitManualRelease,      ///< waiting for a manual release (release button on camera or in app)
      takePhoto,              ///< takes photo
      waitTransferFinished,   ///< waits for the transfer to have finished
      finished,               ///< timelapse is finished
   };

   /// current state machine mode
   std::atomic<T_enStateMachineState> m_stateMachineState;

   /// host
   IPhotoModeViewHost& m_host;

   /// parent window for dialogs
   HWND& m_hWnd;

   /// remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;

   /// scheduler for time lapse operations
   std::shared_ptr<TimeLapseScheduler> m_spScheduler;

   /// options for timelapse
   TimeLapseOptions m_options;

   /// date/time of last trigger to take a photo
   COleDateTime m_lastTriggerTime;

   /// handler ID for state events
   int m_stateEventHandlerId;

   /// filenames of timelapse shots
   std::vector<CString> m_timelapseFilenameList;

   /// function that is called when the "finished" state is reached; may be empty
   std::function<void()> m_fnFinished;
};
