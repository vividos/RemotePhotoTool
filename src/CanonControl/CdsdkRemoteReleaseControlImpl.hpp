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
#include "CdsdkViewfinderImpl.hpp"
#include "Observer.hpp"

namespace CDSDK
{

class SourceDeviceImpl;

/// remote release control impl for CDSDK
class RemoteReleaseControlImpl: public RemoteReleaseControl
{
public:
   /// ctor
   RemoteReleaseControlImpl(std::shared_ptr<SourceDeviceImpl> spSourceDevice)
      :m_spSourceDevice(spSourceDevice),
       m_uiBatteryLevel(BATTERY_LEVEL_NORMAL)
   {
   }

   /// dtor
   virtual ~RemoteReleaseControlImpl() throw()
   {
      cdHSource hSource = GetSource();

      // may return cdINVALID_HANDLE, cdINVALID_FN_CALL
      cdError err = CDExitReleaseControl(hSource);
      LOG_TRACE(_T("CDExitReleaseControl(%08x) returned %08x\n"), hSource, err);
   }

   /// returns source device
   cdHSource GetSource() const throw();

   virtual bool GetCapability(RemoteReleaseControl::T_enRemoteCapability enCapability) const throw() override
   {
      try
      {
         cdReleaseControlFaculty faculty = 0;
         cdError err = CDGetReleaseControlFaculty(GetSource(), &faculty);
         LOG_TRACE(_T("CDGetReleaseControlFaculty(%08x, &faculty) returned %08x\n"), GetSource(), err);
         CheckError(_T("CDGetReleaseControlFaculty"), err, __FILE__, __LINE__);

         switch (enCapability)
         {
         case RemoteReleaseControl::capChangeShootingParameter:
            return (faculty & cdRELEASE_CONTROL_CAP_SETPRM) != 0;

         case RemoteReleaseControl::capChangeShootingMode:
            return (faculty & cdRELEASE_CONTROL_CAP_SETPRM) != 0;

         case RemoteReleaseControl::capZoomControl:
            return (faculty & cdRELEASE_CONTROL_CAP_ZOOM) != 0;

         case RemoteReleaseControl::capViewfinder:
            // Not all camera models support the Viewfinder function. Cameras that support Viewfinder
            // are those cameras for which the cdRELEASE_CONTROL_CAP_VIEWFINDER bit is set. Obtain the
            // value by executing CDGetDevicePropertyData using cdDEVICE_PROP_RELEASE_CONTROL_CAP.
            // note: in remote release mode, CDGetReleaseControlFaculty can also be used
            return (faculty & cdRELEASE_CONTROL_CAP_VIEWFINDER) != 0;

         case RemoteReleaseControl::capReleaseWhileViewfinder:
            // If cdRELEASE_CONTROL_CAP_ABORT_VIEWFINDER bit is set in the connected camera model as a
            // value that can be obtained by executing CDGetDevicePropertyData with
            // cdDEVICE_PROP_RELEASE_CONTROL_CAP, and you are shooting with the Viewfinder using the
            // CDRelease function, you must stop Viewfinder first.
            return (faculty & cdRELEASE_CONTROL_CAP_ABORT_VIEWFINDER) == 0;

         case RemoteReleaseControl::capAFLock:
            return (faculty & cdRELEASE_CONTROL_CAP_AF_LOCK) != 0;

         default:
            ATLASSERT(false);
         }
      }
      catch(const CameraException& ex)
      {
         LOG_TRACE(_T("CameraException during GetCapability: %s\n"), ex.Message());
      }
      return false;
   }

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

   virtual ImageProperty MapShootingModeToImagePropertyValue(RemoteReleaseControl::T_enShootingMode enShootingMode) const override
   {
      cdShootingMode shootingMode = cdSHOOTING_MODE_INVALID;
      switch (enShootingMode)
      {
      case RemoteReleaseControl::shootingModeP:    shootingMode = cdSHOOTING_MODE_PROGRAM; break;
      case RemoteReleaseControl::shootingModeTv:   shootingMode = cdSHOOTING_MODE_TV; break;
      case RemoteReleaseControl::shootingModeAv:   shootingMode = cdSHOOTING_MODE_AV; break;
      case RemoteReleaseControl::shootingModeM:    shootingMode = cdSHOOTING_MODE_MANUAL; break;
      default:
         ATLASSERT(false);
         break;
      }

      Variant value;
      value.Set(shootingMode);
      value.SetType(Variant::typeUInt16);

      return ImageProperty(variantCdsdk, MapImagePropertyTypeToId(propShootingMode), value, true);
   }

   virtual std::vector<unsigned int> EnumImageProperties() const override
   {
      cdHSource hSource = GetSource();

      ImagePropertyAccess p(hSource);

      return p.EnumImageProperties();
   }

   virtual ImageProperty GetImageProperty(unsigned int uiImageProperty) const override
   {
      // special case: since battery level can only be obtained by listening to events,
      // return the last known battery level here
      if (uiImageProperty == MapImagePropertyTypeToId(propBatteryLevel))
      {
         Variant value;

         value.Set(m_uiBatteryLevel);
         value.SetType(Variant::typeUInt32);

         return ImageProperty(variantCdsdk, uiImageProperty, value, true);
      }

      // special case: propSaveTo flag
      if (uiImageProperty == MapImagePropertyTypeToId(propSaveTo))
      {
         // TODO
      }

      // get value
      cdHSource hSource = GetSource();
      ImagePropertyAccess p(hSource);

      Variant value = p.Get(uiImageProperty);
      bool bReadOnly = p.IsReadOnly(uiImageProperty);

      // return in image property object
      return ImageProperty(variantCdsdk, uiImageProperty, value, bReadOnly);
   }

   virtual void SetImageProperty(const ImageProperty& imageProperty) override
   {
      cdHSource hSource = GetSource();
      ImagePropertyAccess p(hSource);

      p.Set(imageProperty.Id(), imageProperty.Value());
   }

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

   virtual std::shared_ptr<Viewfinder> StartViewfinder() const override
   {
      if (!GetCapability(RemoteReleaseControl::capViewfinder))
         throw CameraException(_T("RemoteReleaseControl::StartViewfinder"),
            false, cdERROR_CLIENT_COMPONENTID, cdNOT_SUPPORTED, __FILE__, __LINE__);

      return std::shared_ptr<Viewfinder>(new ViewfinderImpl(m_spSourceDevice));
   }

   virtual unsigned int NumAvailableShots() const override
   {
      cdHSource hSource = GetSource();

      cdUInt32 uiAvailShots = 0;

      cdError err = CDGetNumAvailableShot(hSource, &uiAvailShots);
      ATLTRACE(_T("CDGetNumAvailableShot(%08x, &shots = %u) returned %08x\n"), hSource, uiAvailShots, err);
      CheckError(_T("CDGetNumAvailableShot"), err, __FILE__, __LINE__);

      return uiAvailShots;
   }

   virtual void SendCommand(RemoteReleaseControl::T_enCameraCommand /*enCameraCommand*/) override
   {
      // TODO implement
   }

   virtual void Release(const ShutterReleaseSettings& /*settings*/) override
   {
      // TODO implement
   }

   virtual std::shared_ptr<BulbReleaseControl> StartBulb(const ShutterReleaseSettings& /*settings*/) override
   {
      // bulb not supported by CDSDK
      throw CameraException(_T("RemoteReleaseControl::StartBulb"),
         false,
         cdERROR_CDSDK_COMPONENTID,
         cdNOT_SUPPORTED,
         __FILE__, __LINE__);
   }

private:
   /// source device
   std::shared_ptr<SourceDeviceImpl> m_spSourceDevice;

   /// subject of observer pattern; used for property events
   Subject<void(RemoteReleaseControl::T_enPropertyEvent, unsigned int)> m_subjectPropertyEvent;

   /// subject of observer pattern; used for state events
   Subject<void(RemoteReleaseControl::T_enStateEvent, unsigned int)> m_subjectStateEvent;

   /// subject of observer pattern; used for download events
   Subject<void(RemoteReleaseControl::T_enDownloadEvent, unsigned int)> m_subjectDownloadEvent;

   /// last known battery level, severity and type
   unsigned int m_uiBatteryLevel;
};

} // namespace CDSDK
