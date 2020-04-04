//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file EdsdkRemoteReleaseControlImpl.hpp EDSDK - RemoteReleaseControl impl
//
#pragma once

// includes
#include "RemoteReleaseControl.hpp"
#include "ShutterReleaseSettings.hpp"
#include "EdsdkPropertyAccess.hpp"
#include "EdsdkViewfinderImpl.hpp"
#include "EdsdkBulbReleaseControlImpl.hpp"
#include <ulib/Observer.hpp>
#include <ulib/thread/LightweightMutex.hpp>
#include <ulib/thread/Event.hpp>
#include <thread>

// forward references
class SourceDevice;
class AsyncReleaseControlThread;

namespace EDSDK
{

/// remote release control implementation for EDSDK
class RemoteReleaseControlImpl: public RemoteReleaseControl
{
public:
   /// ctor
   RemoteReleaseControlImpl(std::shared_ptr<SourceDevice> spSourceDevice, const Handle& hCamera);

   /// dtor
   virtual ~RemoteReleaseControlImpl();

   /// callback function for property changes
   static EdsError EDSCALLBACK OnPropertyChange_(
      EdsPropertyEvent inEvent,
      EdsPropertyID inPropertyID,
      EdsUInt32 inParam,
      EdsVoid* inContext);

   /// called on property changes
   void OnPropertyChange(EdsPropertyEvent inEvent, EdsPropertyID inPropertyID, EdsUInt32 inParam);

   /// callback function for state changes
   static EdsError EDSCALLBACK OnStateChange_(EdsStateEvent inEvent, EdsUInt32 inEventData, EdsVoid *inContext);

   /// called on state changes
   void OnStateChange(EdsStateEvent inEvent, EdsUInt32 inEventData);

   /// callback function for object changes
   static EdsError EDSCALLBACK OnObjectChange_(EdsObjectEvent inEvent, EdsBaseRef inRef, EdsVoid *inContext);

   // RemoteReleaseControl methods

   virtual bool GetCapability(RemoteReleaseControl::T_enRemoteCapability enCapability) const override;

   virtual void SetReleaseSettings(const ShutterReleaseSettings& settings) override
   {
      {
         LightweightMutex::LockType lock(m_mtxShutterReleaseSettings);
         m_shutterReleaseSettings = settings;
      }

      SetSaveToFlag(settings.SaveTarget(), true);

      if ((settings.SaveTarget() & ShutterReleaseSettings::saveToHost) != 0)
         SetCapacity();
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
      return PropertyAccess::MapToPropertyID(enImagePropertyType);
   }

   virtual ImageProperty MapShootingModeToImagePropertyValue(RemoteReleaseControl::T_enShootingMode enShootingMode) const override
   {
      unsigned int uiValue = 0;
      switch(enShootingMode)
      {
      case RemoteReleaseControl::shootingModeP:    uiValue = kEdsAEMode_Program; break;
      case RemoteReleaseControl::shootingModeTv:   uiValue = kEdsAEMode_Tv; break;
      case RemoteReleaseControl::shootingModeAv:   uiValue = kEdsAEMode_Av; break;
      case RemoteReleaseControl::shootingModeM:    uiValue = kEdsAEMode_Manual; break;
      default:
         ATLASSERT(false);
         break;
      }

      Variant value;
      value.Set(uiValue);
      value.SetType(Variant::typeUInt32);

      return ImageProperty(variantEdsdk, kEdsPropID_AEMode, value, true);
   }

   virtual std::vector<unsigned int> EnumImageProperties() const override
   {
      PropertyAccess p(m_hCamera);

      std::vector<unsigned int> vecImageIds;
      p.EnumImageIds(vecImageIds);

      return vecImageIds;
   }

   virtual ImageProperty GetImageProperty(unsigned int uiImageProperty) const override
   {
      int iParam = 0;
      unsigned int uiRealImageProperty = uiImageProperty;

      // support for custom functions: passing param in the property ID
      if ((uiRealImageProperty & 0xFFFF) == kEdsPropID_CFn)
      {
         iParam = static_cast<int>(uiRealImageProperty >> 16);
         uiRealImageProperty &= 0xFFFF;
      }

      try
      {
         // get value
         PropertyAccess p(m_hCamera);
         Variant value = p.Get(uiRealImageProperty, iParam);
         bool bReadOnly = p.IsReadOnly(uiImageProperty);

         return ImageProperty(variantEdsdk, uiImageProperty, value, bReadOnly);
      }
      catch (...)
      {
         // special case: Zoom position
         if (uiImageProperty == kEdsPropID_Evf_Zoom)
         {
            Variant value;
            value.Set(m_uiCurrentZoomPos);
            value.SetType(Variant::typeUInt32);

            return ImageProperty(variantEdsdk, uiImageProperty, value, false);
         }

         // else rethrow
         throw;
      }
   }

   virtual void SetImageProperty(const ImageProperty& imageProperty) override;

   virtual void EnumImagePropertyValues(unsigned int uiImageProperty, std::vector<ImageProperty>& vecValues) const override
   {
      PropertyAccess p(m_hCamera);

      std::vector<Variant> vecRawValues;
      bool bReadOnly = false;

      unsigned int uiRealImageProperty = uiImageProperty;

      // support for custom functions: passing param in the property ID
      if ((uiRealImageProperty & 0xFFFF) == kEdsPropID_CFn)
      {
         uiRealImageProperty &= 0xFFFF;
      }

      p.Enum(uiRealImageProperty, vecRawValues, bReadOnly);

      for (size_t i=0,iMax=vecRawValues.size(); i<iMax; i++)
         vecValues.push_back(ImageProperty(variantEdsdk, uiImageProperty, vecRawValues[i], bReadOnly));
   }

   virtual std::shared_ptr<Viewfinder> StartViewfinder() const override;

   virtual unsigned int NumAvailableShots() const override
   {
      PropertyAccess p(m_hCamera);
      Variant v = p.Get(kEdsPropID_AvailableShots);

      return v.Get<unsigned int>();
   }

   virtual void SendCommand(RemoteReleaseControl::T_enCameraCommand enCameraCommand) override
   {
      EdsCameraCommand uiCommand = 0;
      EdsInt32 iParam = 0;

      switch (enCameraCommand)
      {
      case RemoteReleaseControl::commandAdjustFocus:
         uiCommand = kEdsCameraCommand_PressShutterButton;
         iParam = kEdsCameraCommand_ShutterButton_Halfway;
         break;

      case RemoteReleaseControl::commandAdjustWhiteBalance: // not supported
         break;

      case RemoteReleaseControl::commandAdjustExposure: // not supported
         break;

      default:
         ATLASSERT(false);
         return;
      }

      if (uiCommand == 0)
         return;

      EdsError err = EdsSendCommand(m_hCamera.Get(), uiCommand, iParam);
      LOG_TRACE(_T("EdsSendCommand(%08x, command = %04x, 0) returned %08x\n"), m_hCamera.Get(), uiCommand, err);
      EDSDK::CheckError(_T("EdsSendCommand"), err, __FILE__, __LINE__);
   }

   virtual void Release() override;

   virtual std::shared_ptr<BulbReleaseControl> StartBulb() override
   {
      return std::shared_ptr<BulbReleaseControl>(new BulbReleaseControlImpl(m_hCamera));
   }

   virtual void Close() override;

   /// synchronous method to set image property (called in worker thread by SetImageProperty)
   void SyncSetImageProperty(const ImageProperty& imageProperty);

   /// sets SaveTo flag
   void SetSaveToFlag(ShutterReleaseSettings::T_enSaveTarget enSaveTarget, bool bAsynchronous);

   /// sets capacity of host system
   void SetCapacity();

   /// asynchronous release method
   void AsyncRelease();

private:
   /// called when transfer request is received
   void OnReceivedObjectEventRequestTransfer(Handle hDirectoryItem);

   /// async method to download image
   void AsyncDownloadImage(Handle hDirectoryItem, ShutterReleaseSettings& settings);

   /// downloads image
   void DownloadImage(Handle hDirectoryItem, ShutterReleaseSettings& settings);

   /// progress callback function
   static EdsError EDSCALLBACK ProgressFunc(EdsUInt32 inPercent, EdsVoid* inContext, EdsBool* outCancel);

private:
   /// source device
   std::shared_ptr<SourceDevice> m_spSourceDevice;

   /// handle to camera object
   Handle m_hCamera;

   /// background thread for release control
   std::unique_ptr<AsyncReleaseControlThread> m_upReleaseThread;

   /// mutex to synchronize access to ED-SDK when using viewfinder
   std::shared_ptr<LightweightMutex> m_spMtxLock;

   /// subject of observer pattern; used for property events
   Subject<void(RemoteReleaseControl::T_enPropertyEvent, unsigned int)> m_subjectPropertyEvent;

   /// subject of observer pattern; used for state events
   Subject<void(RemoteReleaseControl::T_enStateEvent, unsigned int)> m_subjectStateEvent;

   /// subject of observer pattern; used for download events
   Subject<void(RemoteReleaseControl::T_enDownloadEvent, unsigned int)> m_subjectDownloadEvent;

   /// mutex for locking m_shutterReleaseSettings
   LightweightMutex m_mtxShutterReleaseSettings;

   /// shutter release settings
   ShutterReleaseSettings m_shutterReleaseSettings;

   /// event that gets set when a shutter release occured; used in Release()
   ManualResetEvent m_evtShutterReleaseOccured;

   /// currently set zoom position (must be managed by ourselves)
   unsigned int m_uiCurrentZoomPos;
};

} // namespace EDSDK
