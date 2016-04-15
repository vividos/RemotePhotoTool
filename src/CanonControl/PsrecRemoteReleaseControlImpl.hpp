//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PsrecRemoteReleaseControlImpl.hpp PS-ReC - RemoteReleaseControl impl
//
#pragma once

// includes
#include "RemoteReleaseControl.hpp"
#include "ShutterReleaseSettings.hpp"
#include "PsrecCommon.hpp"
#include "PsrecPropertyAccess.hpp"
#include "PsrecDeviceInfo.hpp"
#include "PsrecViewfinderImpl.hpp"
#include "Observer.hpp"
#include "LightweightMutex.hpp"
#include "Event.hpp"

// forward references
class AsyncReleaseControlThread;

namespace PSREC
{

struct CameraEventData;
class SourceDeviceImpl;

/// remote release control implementation
class RemoteReleaseControlImpl: public RemoteReleaseControl
{
public:
   /// ctor
   RemoteReleaseControlImpl(prHandle hCamera, std::shared_ptr<SourceDeviceImpl> spSourceDevice);

   /// dtor
   virtual ~RemoteReleaseControlImpl() throw();

   virtual bool GetCapability(RemoteReleaseControl::T_enRemoteCapability enCapability) const throw() override;

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

   virtual void RemoveDownloadEventHandler(int iHandlerId)
   {
      m_subjectDownloadEvent.Remove(iHandlerId);
   }

   virtual unsigned int MapImagePropertyTypeToId(T_enImagePropertyType enImagePropertyType) const override;

   virtual ImageProperty MapShootingModeToImagePropertyValue(RemoteReleaseControl::T_enShootingMode enShootingMode) const override;

   virtual ImageProperty GetImageProperty(unsigned int uiImageProperty) const override;

   virtual void SetImageProperty(const ImageProperty& imageProperty) override
   {
      PropertyAccess access(m_hCamera);
      access.Set(static_cast<prUInt16>(imageProperty.Id()), imageProperty.Value());
   }

   virtual void EnumImagePropertyValues(unsigned int uiImagePropertyId, std::vector<ImageProperty>& vecValues) const override
   {
      PropertyAccess access(m_hCamera);

      if (uiImagePropertyId == PSREC_PROP_IMAGE_FORMAT)
      {
         access.EnumImageFormatPropertyValues(vecValues);
         return;
      }

      std::vector<Variant> vecRawValues;
      access.Enum(static_cast<prUInt16>(uiImagePropertyId), vecRawValues);

      // return in image property object
      for (size_t i=0,iMax=vecRawValues.size(); i<iMax; i++)
         vecValues.push_back(ImageProperty(variantPsrec, uiImagePropertyId, vecRawValues[i], false));
   }

   virtual std::vector<unsigned int> EnumImageProperties() const override;

   /// starts viewfinder
   virtual std::shared_ptr<Viewfinder> StartViewfinder() const override
   {
      if (!GetCapability(RemoteReleaseControl::capViewfinder))
      {
         throw CameraException(_T("PSREC::RemoteReleaseControl::StartViewfinder"), _T("Not supported"),
            prERROR_PRSDK_COMPONENTID | prNOT_SUPPORTED, __FILE__, __LINE__);
      }

      return std::shared_ptr<Viewfinder>(new ViewfinderImpl(m_spSourceDevice, m_hCamera));
   }

   /// returns number of available shots on camera memory card
   virtual unsigned int NumAvailableShots() const override;

   virtual void SendCommand(RemoteReleaseControl::T_enCameraCommand enCameraCommand) override;

   /// releases the camera shutter; blocks until image transfer is started
   virtual void Release() override;

   /// starts bulb mode, which is not supported by PSREC
   virtual std::shared_ptr<BulbReleaseControl> StartBulb() override
   {
      // bulb not supported by PSREC
      throw CameraException(_T("PSREC::RemoteReleaseControl::StartBulb"), _T("Not supported"),
         prERROR_PRSDK_COMPONENTID | prNOT_SUPPORTED, __FILE__, __LINE__);
   }

   virtual void Close() override;

private:
   /// releases shutter; called in worker thread
   void AsyncRelease();

   /// translates from event code to event name
   static LPCTSTR EventNameFromCode(prUInt16 uiEventCode) throw();

   /// camera event handler
   static prResponse prSTDCALL OnCameraEvent_(prHandle hCamera, prContext Context, prVoid* pEventData);

   /// camera event handler; non-static function
   void OnCameraEvent(const CameraEventData& eventData);

   /// gets changed properties from camera
   void GetChangedProperties(std::vector<prUInt16>& vecChangedProperties);

   /// called when device or image property has changed
   void OnEventPropertyChanged(prUInt16 propId, bool bAlsoUpdateDescription);

   /// called when camera state has changed
   void OnEventStateChanged(prUInt16 code) throw();

   /// starts download of image or thumbnail (async transfer)
   void StartImageDownload(prObjectHandle hObject, bool bFullView);

   /// callback function for async transfer of image or thumbnail
   static prResponse prSTDCALL OnDownloadImageData_(
      prHandle       hCamera,
      prObjectHandle hObject,
      prContext      Context,
      prProgress*    pProgress);

   /// called on transfer of image or thumbnail
   bool OnDownloadImageData(prProgressMsg message, prUInt8* pbData, prUInt32 uiLength);

   /// called on start of transfer of image data
   void OnStartDownloadImageData();

   /// called on end of transfer of image data
   void OnFinishedDownloadImageData();

private:
   /// source device
   std::shared_ptr<SourceDeviceImpl> m_spSourceDevice;

   /// camera handle
   prHandle m_hCamera;

   /// background thread for release control
   std::unique_ptr<AsyncReleaseControlThread> m_upReleaseThread;

   /// device info; from source device
   std::shared_ptr<DeviceInfo> m_spDeviceInfo;

   /// mutex for locking m_shutterReleaseSettings
   LightweightMutex m_mtxShutterReleaseSettings;

   /// default shutter release settings
   ShutterReleaseSettings m_shutterReleaseSettings;

   /// event that is set when image is ready after calling Release()
   Event m_evtReleaseImageReady;

   /// indicates that an image transfer is in progress, from a call to Release()
   Event m_evtReleaseImageTransferInProgress;

   /// indicates that image transfer is done
   Event m_evtReleaseImageTransferDone;

   /// handle to image created by Release() call
   prObjectHandle m_hReleaseImage;

   /// file to store transferred image data
   FILE* m_fdImageTransfer;

   /// subject of observer pattern; used for property events
   Subject<void(RemoteReleaseControl::T_enPropertyEvent, unsigned int)> m_subjectPropertyEvent;

   /// subject of observer pattern; used for state events
   Subject<void(RemoteReleaseControl::T_enStateEvent, unsigned int)> m_subjectStateEvent;

   /// subject of observer pattern; used for download events
   Subject<void(RemoteReleaseControl::T_enDownloadEvent, unsigned int)> m_subjectDownloadEvent;
};

} // namespace PSREC
