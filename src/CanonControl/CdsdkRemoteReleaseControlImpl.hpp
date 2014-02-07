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
#include "CdsdkPropertyAccess.hpp"
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
      :m_spSourceDevice(spSourceDevice)
   {
   }

   /// dtor
   virtual ~RemoteReleaseControlImpl() throw()
   {
      cdHSource hSource = GetSource();

      // may return cdINVALID_HANDLE, cdINVALID_FN_CALL
      cdError err = CDExitReleaseControl(hSource);
      LOG_TRACE(_T("CDExitReleaseControl(%08x) returned %08x\n"), hSource, err);
//      CheckError(_T("CDExitReleaseControl"), err, __FILE__, __LINE__); // as dtor is throw()
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
      catch(const CameraException& e)
      {
         e; // TODO logging
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

   virtual unsigned int MapImagePropertyTypeToId(T_enImagePropertyType /*enImagePropertyType*/) const override
   {
      // TODO
      return 0;
   }

   virtual ImageProperty MapShootingModeToImagePropertyValue(RemoteReleaseControl::T_enShootingMode /*enShootingMode*/) const override
   {
      // TODO
      return GetImageProperty(0);
   }

   virtual std::vector<unsigned int> EnumImageProperties() const override
   {
      // TODO
      return std::vector<unsigned int>();
   }

   virtual ImageProperty GetImageProperty(unsigned int uiImageProperty) const override
   {
      // get value
      cdHSource hSource = GetSource();
      ImagePropertyAccess p(hSource);
      // Note: static_cast is wrong here
      Variant value = p.Get(static_cast<T_enImagePropertyType>(uiImageProperty));

      // TODO
      bool bReadOnly = false;

      // return in image property object
      return ImageProperty(variantCdsdk, uiImageProperty, value, bReadOnly);
   }

   virtual void SetImageProperty(const ImageProperty& imageProperty) override
   {
      // set new value
      cdHSource hSource = GetSource();
      ImagePropertyAccess p(hSource);

      // Note: static_cast is wrong here
      p.Set(static_cast<T_enImagePropertyType>(imageProperty.Id()), imageProperty.Value());
   }

   virtual void EnumImagePropertyValues(unsigned int uiImageProperty, std::vector<ImageProperty>& vecValues) const override
   {
      // TODO
      cdHSource hSource = GetSource();
      ImagePropertyAccess p(hSource);

      std::vector<Variant> vecRawValues;
      // Note: static_cast is wrong here
      p.Enum(static_cast<T_enImagePropertyType>(uiImageProperty), vecRawValues);

      // TODO
      bool bReadOnly = false;

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
/*
   static ShootingMode MapShootingMode(cdShootingMode usShootingMode)
   {
      // map modes to predefined ones
      ShootingMode::T_enShootingMode enShootingMode = ShootingMode::shootingModeNA;
      switch (usShootingMode)
      {
      case cdSHOOTING_MODE_AUTO:    enShootingMode = ShootingMode::shootingModeAuto; break;
      case cdSHOOTING_MODE_PROGRAM: enShootingMode = ShootingMode::shootingModeProgram; break;
      case cdSHOOTING_MODE_TV:      enShootingMode = ShootingMode::shootingModeTv; break;
      case cdSHOOTING_MODE_AV:      enShootingMode = ShootingMode::shootingModeAv; break;
      case cdSHOOTING_MODE_MANUAL:  enShootingMode = ShootingMode::shootingModeManual; break;
      case cdSHOOTING_MODE_A_DEP:   enShootingMode = ShootingMode::shootingModeAutoDep; break;

      case cdSHOOTING_MODE_NIGHT_SCENE:   enShootingMode = ShootingMode::shootingModeNightScenePortrait; break;
      case cdSHOOTING_MODE_PORTRAIT:      enShootingMode = ShootingMode::shootingModePortrait; break;
      case cdSHOOTING_MODE_MACRO:         enShootingMode = ShootingMode::shootingModeCloseUp; break;
      case cdSHOOTING_MODE_FLASH_OFF:     enShootingMode = ShootingMode::shootingModeFlashOff; break;
      }

      return ShootingMode(enShootingMode);
   }

   static cdShootingMode UnmapShootingMode(ShootingMode shootingMode)
   {
      cdShootingMode usShootingMode = cdSHOOTING_MODE_INVALID;

      switch (shootingMode.Value())
      {
      case ShootingMode::shootingModeAuto:      usShootingMode = cdSHOOTING_MODE_AUTO;        break;
      case ShootingMode::shootingModeProgram:   usShootingMode = cdSHOOTING_MODE_PROGRAM;     break;
      case ShootingMode::shootingModeTv:        usShootingMode = cdSHOOTING_MODE_TV;          break;
      case ShootingMode::shootingModeAv:        usShootingMode = cdSHOOTING_MODE_AV;          break;
      case ShootingMode::shootingModeManual:    usShootingMode = cdSHOOTING_MODE_MANUAL;      break;
      case ShootingMode::shootingModeAutoDep:   usShootingMode = cdSHOOTING_MODE_A_DEP;       break;

      case ShootingMode::shootingModeNightScenePortrait:     usShootingMode = cdSHOOTING_MODE_NIGHT_SCENE;   break;
      case ShootingMode::shootingModePortrait:               usShootingMode = cdSHOOTING_MODE_PORTRAIT;      break;
      case ShootingMode::shootingModeCloseUp:                usShootingMode = cdSHOOTING_MODE_MACRO;         break;
      case ShootingMode::shootingModeFlashOff:               usShootingMode = cdSHOOTING_MODE_FLASH_OFF;     break;
      }

      return usShootingMode;
   }
*/
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
};

} // namespace CDSDK
