//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CdsdkRemoteReleaseControlImpl.hpp CDSDK - RemoteReleaseControl impl
//
#pragma once

// includes
#include "RemoteReleaseControl.hpp"
#include "CdsdkCommon.hpp"
#include "CdsdkImagePropertyAccess.hpp"
#include "Observer.hpp"

namespace CDSDK
{

class SourceDeviceImpl;

/// remote release control impl for CDSDK
class RemoteReleaseControlImpl: public RemoteReleaseControl
{
public:
   /// ctor
   RemoteReleaseControlImpl(std::shared_ptr<SourceDeviceImpl> spSourceDevice);

   /// dtor
   virtual ~RemoteReleaseControlImpl() throw();

   /// returns source device
   cdHSource GetSource() const throw();

   virtual bool GetCapability(RemoteReleaseControl::T_enRemoteCapability enCapability) const throw() override;

   virtual void SetDefaultReleaseSettings(const ShutterReleaseSettings& /*settings*/) override
   {
      // TODO
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

   virtual void EnumImagePropertyValues(unsigned int uiImageProperty, std::vector<ImageProperty>& vecValues) const override
   {
      cdHSource hSource = GetSource();
      ImagePropertyAccess p(hSource);

      std::vector<Variant> vecRawValues;
      p.Enum(uiImageProperty, vecRawValues);

      bool bReadOnly = p.IsReadOnly(uiImageProperty);

      for (size_t i=0,iMax=vecRawValues.size(); i<iMax; i++)
         vecValues.push_back(ImageProperty(variantCdsdk, uiImageProperty, vecRawValues[i], bReadOnly));
   }

   virtual std::shared_ptr<Viewfinder> StartViewfinder() const override;

   virtual unsigned int NumAvailableShots() const override;

   virtual void SendCommand(RemoteReleaseControl::T_enCameraCommand /*enCameraCommand*/) override;

   virtual void Release(const ShutterReleaseSettings& /*settings*/) override;

   virtual std::shared_ptr<BulbReleaseControl> StartBulb(const ShutterReleaseSettings& /*settings*/) override;

private:
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

   /// event callback handle
   cdHandle m_hEventCallback;

   /// subject of observer pattern; used for property events
   Subject<void(RemoteReleaseControl::T_enPropertyEvent, unsigned int)> m_subjectPropertyEvent;

   /// subject of observer pattern; used for state events
   Subject<void(RemoteReleaseControl::T_enStateEvent, unsigned int)> m_subjectStateEvent;

   /// subject of observer pattern; used for download events
   Subject<void(RemoteReleaseControl::T_enDownloadEvent, unsigned int)> m_subjectDownloadEvent;

   /// last known battery level, severity and type
   unsigned int m_uiBatteryLevel;

   /// current 
   cdRelDataKind m_uiRelDataKind;
};

} // namespace CDSDK
