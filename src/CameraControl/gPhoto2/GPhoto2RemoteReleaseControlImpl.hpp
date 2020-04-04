//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2RemoteReleaseControlImpl.hpp gPhoto2 - Remote release control impl
//
#pragma once

#include "GPhoto2Common.hpp"
#include "RemoteReleaseControl.hpp"
#include "ShutterReleaseSettings.hpp"
#include <ulib/Observer.hpp>
#include <ulib/thread/LightweightMutex.hpp>

class AsyncReleaseControlThread;

namespace GPhoto2
{
   /// implementation of RemoteReleaseControl for gPhoto2 access
   class RemoteReleaseControlImpl : public RemoteReleaseControl
   {
   public:
      /// ctor
      RemoteReleaseControlImpl(std::shared_ptr<_GPContext> context, std::shared_ptr<_Camera> camera);
      /// dtor
      virtual ~RemoteReleaseControlImpl();

      // RemoteReleaseControl virtual functions

      virtual bool GetCapability(T_enRemoteCapability remoteCapability) const override;

      virtual void SetReleaseSettings(const ShutterReleaseSettings& settings) override;

      virtual int AddPropertyEventHandler(RemoteReleaseControl::T_fnOnPropertyChanged onPropertyChanged) override
      {
         return m_subjectPropertyEvent.Add(onPropertyChanged);
      }

      virtual void RemovePropertyEventHandler(int handlerId) override
      {
         m_subjectPropertyEvent.Remove(handlerId);
      }

      virtual int AddStateEventHandler(RemoteReleaseControl::T_fnOnStateChanged onStateChanged) override
      {
         return m_subjectStateEvent.Add(onStateChanged);
      }

      virtual void RemoveStateEventHandler(int handlerId) override
      {
         m_subjectStateEvent.Remove(handlerId);
      }

      virtual int AddDownloadEventHandler(RemoteReleaseControl::T_fnOnDownloadChanged onDownloadChanged) override
      {
         return m_subjectDownloadEvent.Add(onDownloadChanged);
      }

      virtual void RemoveDownloadEventHandler(int handlerId) override
      {
         m_subjectDownloadEvent.Remove(handlerId);
      }

      virtual unsigned int MapImagePropertyTypeToId(T_enImagePropertyType imagePropertyType) const override;

      virtual ImageProperty MapShootingModeToImagePropertyValue(T_enShootingMode shootingMode) const override;

      virtual std::vector<unsigned int> EnumImageProperties() const override;

      virtual ImageProperty GetImageProperty(unsigned int imagePropertyId) const override;

      virtual void SetImageProperty(const ImageProperty& imageProperty) override;

      virtual void EnumImagePropertyValues(unsigned int imagePropertyId, std::vector<ImageProperty>& valuesList) const override;

      virtual std::shared_ptr<Viewfinder> StartViewfinder() const override;

      virtual unsigned int NumAvailableShots() const override;

      virtual void SendCommand(T_enCameraCommand cameraCommand) override;

      virtual void Release() override;

      virtual std::shared_ptr<BulbReleaseControl> StartBulb() override;

      virtual void Close() override;

   private:
      /// releases shutter; called in worker thread
      void AsyncRelease();

   private:
      /// gPhoto2 context
      std::shared_ptr<_GPContext> m_context;

      /// camera instance
      std::shared_ptr<_Camera> m_camera;

      /// background thread for release control
      std::unique_ptr<AsyncReleaseControlThread> m_releaseThread;

      /// mutex to protect m_shutterReleaseSettings
      LightweightMutex m_mutexShutterReleaseSettings;

      /// shutter release settings
      ShutterReleaseSettings m_shutterReleaseSettings;

      /// subject of observer pattern; used for property events
      Subject<void(RemoteReleaseControl::T_enPropertyEvent, unsigned int)> m_subjectPropertyEvent;

      /// subject of observer pattern; used for state events
      Subject<void(RemoteReleaseControl::T_enStateEvent, unsigned int)> m_subjectStateEvent;

      /// subject of observer pattern; used for download events
      Subject<void(RemoteReleaseControl::T_enDownloadEvent, unsigned int)> m_subjectDownloadEvent;
   };

} // namespace GPhoto2
