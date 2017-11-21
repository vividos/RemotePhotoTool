//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file WiaRemoteReleaseControlImpl.hpp WIA - RemoteRelaseControl impl
//
#pragma once

// includes
#include "RemoteReleaseControl.hpp"
#include "WiaCommon.hpp"
#include "ShutterReleaseSettings.hpp"
#include <ulib/Observer.hpp>
#include <ulib/thread/LightweightMutex.hpp>

namespace WIA
{
   /// remote release control implementation for WIA
   class RemoteReleaseControlImpl : public RemoteReleaseControl
   {
   public:
      /// ctor
      RemoteReleaseControlImpl(RefSp& ref)
         :m_ref(ref)
      {
      }
      /// dtor
      virtual ~RemoteReleaseControlImpl();

      virtual bool GetCapability(RemoteReleaseControl::T_enRemoteCapability capability) const override;

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

      virtual void RemoveDownloadEventHandler(int iHandlerId)
      {
         m_subjectDownloadEvent.Remove(iHandlerId);
      }

      virtual unsigned int MapImagePropertyTypeToId(T_enImagePropertyType imagePropertyType) const override;

      virtual ImageProperty MapShootingModeToImagePropertyValue(RemoteReleaseControl::T_enShootingMode shootingMode) const override;

      virtual ImageProperty GetImageProperty(unsigned int imagePropertyID) const override;

      virtual void SetImageProperty(const ImageProperty& imageProperty) override;

      virtual void EnumImagePropertyValues(unsigned int imagePropertyID, std::vector<ImageProperty>& valuesList) const override;

      virtual std::vector<unsigned int> EnumImageProperties() const override;

      /// starts viewfinder
      virtual std::shared_ptr<Viewfinder> StartViewfinder() const override;

      /// returns number of available shots on camera memory card
      virtual unsigned int NumAvailableShots() const override;

      virtual void SendCommand(RemoteReleaseControl::T_enCameraCommand cameraCommand) override;

      virtual void Release() override;

      virtual std::shared_ptr<BulbReleaseControl> StartBulb() override;

      virtual void Close() override;

   private:
      /// WIA reference
      RefSp m_ref;

      /// mutex for locking m_shutterReleaseSettings
      LightweightMutex m_mtxShutterReleaseSettings;

      /// default shutter release settings
      ShutterReleaseSettings m_shutterReleaseSettings;

      /// subject of observer pattern; used for property events
      Subject<void(RemoteReleaseControl::T_enPropertyEvent, unsigned int)> m_subjectPropertyEvent;

      /// subject of observer pattern; used for state events
      Subject<void(RemoteReleaseControl::T_enStateEvent, unsigned int)> m_subjectStateEvent;

      /// subject of observer pattern; used for download events
      Subject<void(RemoteReleaseControl::T_enDownloadEvent, unsigned int)> m_subjectDownloadEvent;
   };

} // namespace WIA
