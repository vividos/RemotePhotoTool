//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file CdsdkRemoteReleaseControlImpl.hpp CDSDK - RemoteReleaseControl impl
//
#pragma once

#include "RemoteReleaseControl.hpp"
#include "CdsdkCommon.hpp"
#include "CdsdkImagePropertyAccess.hpp"
#include "ShutterReleaseSettings.hpp"
#include <ulib/Observer.hpp>
#include <ulib/thread/LightweightMutex.hpp>

class SingleThreadExecutor;

namespace CDSDK
{

class SourceDeviceImpl;

// These types and constants are not documented in CDSDK, but were discovered in the C# project
// "Canon SDK wrapper for CDSDK and PRSDK", authors Benjamin Liedblad & Christian Graus,
// available on CodeProject:
// http://www.codeproject.com/Articles/17344/A-wrapper-for-the-canon-CDSDK-and-PRSDK-for-remote

/// AE, AF, AWB reset flag type
typedef cdUInt32 cdAeAfAwbResetFlag;

const cdAeAfAwbResetFlag cdAEAFAWB_RESET_AE = 0x00000001; ///< AE reset
const cdAeAfAwbResetFlag cdAEAFAWB_RESET_AF = 0x00000002; ///< AF reset
const cdAeAfAwbResetFlag cdAEAFAWB_RESET_AWB = 0x00000004; ///< AWB reset

/// remote release control impl for CDSDK
class RemoteReleaseControlImpl: public RemoteReleaseControl
{
public:
   /// ctor
   RemoteReleaseControlImpl(std::shared_ptr<SourceDeviceImpl> spSourceDevice);

   /// dtor
   virtual ~RemoteReleaseControlImpl();

   /// returns source device
   cdHSource GetSource() const;

   virtual bool GetCapability(RemoteReleaseControl::T_enRemoteCapability enCapability) const override;

   virtual void SetReleaseSettings(const ShutterReleaseSettings& settings) override
   {
      LightweightMutex::LockType lock(m_mtxShutterReleaseSettings);

      m_shutterReleaseSettings = settings;
   }

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

   virtual unsigned int MapImagePropertyTypeToId(T_enImagePropertyType enImagePropertyType) const override
   {
      return ImagePropertyAccess::MapToPropertyID(enImagePropertyType);
   }

   virtual ImageProperty MapShootingModeToImagePropertyValue(RemoteReleaseControl::T_enShootingMode enShootingMode) const override;

   virtual std::vector<unsigned int> EnumImageProperties() const override
   {
      cdHSource hSource = GetSource();

      ImagePropertyAccess p(hSource);

      return p.EnumImageProperties();
   }

   virtual ImageProperty GetImageProperty(unsigned int uiImageProperty) const override;

   virtual void SetImageProperty(const ImageProperty& imageProperty) override;

   virtual void EnumImagePropertyValues(unsigned int uiImageProperty, std::vector<ImageProperty>& vecValues) const override;

   virtual std::shared_ptr<Viewfinder> StartViewfinder() const override;

   virtual unsigned int NumAvailableShots() const override;

   virtual void SendCommand(RemoteReleaseControl::T_enCameraCommand enCameraCommand) override;

   virtual void Release() override;

   virtual std::shared_ptr<BulbReleaseControl> StartBulb() override;

   virtual void Close() override;

private:
   /// returns context value used by callback functions
   cdContext GetContext() const
   {
#pragma warning(push)
#pragma warning(disable: 4311) // 'reinterpret_cast' : pointer truncation from 'P' to 'T'
      cdContext context = reinterpret_cast<cdContext>(this);
      return context;
#pragma warning(pop)
   }

   /// releases shutter; called in worker thread
   void AsyncRelease();

   /// executes shutter release
   void ReleaseShutter(bool bSync, cdUInt32& numData);

   /// reads release data
   void ReadReleaseData(const CStringA& cszaFilename, cdUInt32 numData);

   /// release event callback
   static cdUInt32 cdSTDCALL OnReleaseEventCallback_(
      cdReleaseEventID EventID, const void* pData, cdUInt32 DataSize, cdContext Context);

   /// release event callback handler
   void OnReleaseEventCallback(cdReleaseEventID EventID);

   /// event callback
   static cdUInt32 cdSTDCALL OnEventCallback_(
      cdEventID EventID, const void* pData, cdUInt32 DataSize, cdContext Context);

   /// event callback handler
   void OnEventCallback(cdEventID eventId);

   /// progress callback
   static cdUInt32 cdSTDCALL OnProgressCallback_(cdUInt32 Progress,
      cdProgressStatus Status, cdContext Context);

   /// progress callback handler; returns true to continue progress
   bool OnProgressCallback(cdUInt32 Progress, cdProgressStatus Status);

private:
   /// source device
   std::shared_ptr<SourceDeviceImpl> m_spSourceDevice;

   /// background thread executor for release control
   std::unique_ptr<SingleThreadExecutor> m_executor;

   /// mutex to protect m_shutterReleaseSettings
   LightweightMutex m_mtxShutterReleaseSettings;

   /// shutter release settings
   ShutterReleaseSettings m_shutterReleaseSettings;

   /// event callback handle
   cdHandle m_hEventCallback;

   /// subject of observer pattern; used for property events
   Subject<void(RemoteReleaseControl::T_enPropertyEvent, unsigned int)> m_subjectPropertyEvent;

   /// subject of observer pattern; used for state events
   Subject<void(RemoteReleaseControl::T_enStateEvent, unsigned int)> m_subjectStateEvent;

   /// subject of observer pattern; used for download events
   Subject<void(RemoteReleaseControl::T_enDownloadEvent, unsigned int)> m_subjectDownloadEvent;

   /// current "save to" target
   cdRelDataKind m_uiRelDataKind;

   /// release control faculty value
   cdReleaseControlFaculty m_releaseControlFaculty;
};

} // namespace CDSDK
