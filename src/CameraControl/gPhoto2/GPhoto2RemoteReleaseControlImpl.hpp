//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file GPhoto2RemoteReleaseControlImpl.hpp gPhoto2 - Remote release control impl
//
#pragma once

// includes
#include "GPhoto2Common.hpp"
#include "RemoteReleaseControl.hpp"
#include "ShutterReleaseSettings.hpp"
#include <ulib/Observer.hpp>
#include <ulib/thread/LightweightMutex.hpp>

// forward references
class AsyncReleaseControlThread;

namespace GPhoto2
{

/// implementation of RemoteReleaseControl for gPhoto2 access
class RemoteReleaseControlImpl : public RemoteReleaseControl
{
public:
   /// ctor
   RemoteReleaseControlImpl(std::shared_ptr<_GPContext> spContext, std::shared_ptr<_Camera> spCamera);
   /// dtor
   virtual ~RemoteReleaseControlImpl();

   // RemoteReleaseControl virtual functions

   virtual bool GetCapability(T_enRemoteCapability enRemoteCapability) const override;

   virtual void SetReleaseSettings(const ShutterReleaseSettings& settings) override;

   virtual int AddPropertyEventHandler(RemoteReleaseControl::T_fnOnPropertyChanged fnOnPropertyChanged) override
   {
      return m_subjectPropertyEvent.Add(fnOnPropertyChanged);
   }

   virtual void RemovePropertyEventHandler(int iHandlerId) override
   {
      m_subjectPropertyEvent.Remove(iHandlerId);
   }

   virtual int AddStateEventHandler(RemoteReleaseControl::T_fnOnStateChanged fnOnStateChanged) override
   {
      return m_subjectStateEvent.Add(fnOnStateChanged);
   }

   virtual void RemoveStateEventHandler(int iHandlerId) override
   {
      m_subjectStateEvent.Remove(iHandlerId);
   }

   virtual int AddDownloadEventHandler(RemoteReleaseControl::T_fnOnDownloadChanged fnOnDownloadChanged) override
   {
      return m_subjectDownloadEvent.Add(fnOnDownloadChanged);
   }

   virtual void RemoveDownloadEventHandler(int iHandlerId) override
   {
      m_subjectDownloadEvent.Remove(iHandlerId);
   }

   virtual unsigned int MapImagePropertyTypeToId(T_enImagePropertyType enImagePropertyType) const override;

   virtual ImageProperty MapShootingModeToImagePropertyValue(T_enShootingMode enShootingMode) const override;

   virtual std::vector<unsigned int> EnumImageProperties() const override;

   virtual ImageProperty GetImageProperty(unsigned int uiImagePropertyId) const override;

   virtual void SetImageProperty(const ImageProperty& imageProperty) override;

   virtual void EnumImagePropertyValues(unsigned int uiImagePropertyId, std::vector<ImageProperty>& vecValues) const override;

   virtual std::shared_ptr<Viewfinder> StartViewfinder() const override;

   virtual unsigned int NumAvailableShots() const override;

   virtual void SendCommand(T_enCameraCommand enCameraCommand) override;

   virtual void Release() override;

   virtual std::shared_ptr<BulbReleaseControl> StartBulb() override;

   virtual void Close() override;

private:
   /// releases shutter; called in worker thread
   void AsyncRelease();

private:
   /// gPhoto2 context
   std::shared_ptr<_GPContext> m_spContext;

   /// camera instance
   std::shared_ptr<_Camera> m_spCamera;

   /// background thread for release control
   std::unique_ptr<AsyncReleaseControlThread> m_upReleaseThread;

   /// mutex to protect m_shutterReleaseSettings
   LightweightMutex m_mtxShutterReleaseSettings;

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
