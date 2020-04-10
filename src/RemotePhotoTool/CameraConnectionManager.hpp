//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file CameraConnectionManager.hpp Camera connection manager
//
#pragma once

// forward references
class SourceDevice;
class RemoteReleaseControl;

/// manager for camera connections
class CameraConnectionManager
{
public:
   /// ctor
   CameraConnectionManager()
   {
   }

   /// connects to a given source device and enters remote release control
   bool Connect(HWND hWnd, std::shared_ptr<SourceDevice> spSourceDevice);

   /// disconnects from camera
   void Disconnect();

   /// returns if a camera is connected
   bool IsConnected() const
   {
      return m_spSourceDevice != nullptr;
   }

   /// returns if a camera is connected and has remote release control
   bool HasReleaseControl() const
   {
      return
         m_spSourceDevice != nullptr &&
         m_spRemoteReleaseControl != nullptr;
   }

   /// returns source device
   std::shared_ptr<SourceDevice> GetSourceDevice() { return m_spSourceDevice; }

   /// returns remote release control
   std::shared_ptr<RemoteReleaseControl> GetRemoteReleaseControl() { return m_spRemoteReleaseControl; }

   /// starts viewfinder
   std::shared_ptr<Viewfinder> StartViewfinder(HWND hWnd);

private:
   /// current source device
   std::shared_ptr<SourceDevice> m_spSourceDevice;

   /// current remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;
};
