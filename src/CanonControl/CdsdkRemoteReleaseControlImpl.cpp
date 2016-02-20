//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CdsdkRemoteReleaseControlImpl.cpp CDSDK - RemoteReleaseControl impl
//

// includes
#include "stdafx.h"
#include "CdsdkSourceDeviceImpl.hpp"
#include "CdsdkRemoteReleaseControlImpl.hpp"
#include "CdsdkViewfinderImpl.hpp"
#include "CameraException.hpp"
#include "AsyncReleaseControlThread.hpp"

using namespace CDSDK;

RemoteReleaseControlImpl::RemoteReleaseControlImpl(std::shared_ptr<SourceDeviceImpl> spSourceDevice)
:m_spSourceDevice(spSourceDevice),
m_upReleaseThread(new AsyncReleaseControlThread),
m_hEventCallback(0),
m_uiRelDataKind(cdREL_KIND_PICT_TO_PC),
m_releaseControlFaculty(0)
{
   // check: pointers (e.g. this) must fit into cdContext; may not work on 64-bit
   static_assert(sizeof(cdContext) == sizeof(void*), "this pointer must fit into cdContext");

   cdHSource hSource = spSourceDevice->GetSource();

   // may return cdINVALID_HANDLE, cdNOT_SUPPORTED, cdINVALID_PARAMETER
   cdError err = CDEnterReleaseControl(hSource, OnReleaseEventCallback_, GetContext());
   LOG_TRACE(_T("CDEnterReleaseControl(%08x, &ReleaseEventCallback, context) returned %08x\n"), hSource, err);
   CheckError(_T("CDEnterReleaseControl"), err, __FILE__, __LINE__);

   // may return cdINVALID_HANDLE
   err = CDGetReleaseControlFaculty(GetSource(), &m_releaseControlFaculty);
   LOG_TRACE(_T("CDGetReleaseControlFaculty(%08x, &faculty = %08x) returned %08x\n"), GetSource(), m_releaseControlFaculty, err);
   CheckError(_T("CDGetReleaseControlFaculty"), err, __FILE__, __LINE__);

   // may return cdINVALID_PARAMETER, cdINVALID_FN_CALL
   err = CDRegisterEventCallbackFunction(hSource,
      &RemoteReleaseControlImpl::OnEventCallback_, GetContext(), &m_hEventCallback);
   LOG_TRACE(_T("CDRegisterEventCallbackFunction(%08x, &EventCallback, context, &hEvent = %08x) returned %08x\n"),
      hSource, m_hEventCallback, err);
   CheckError(_T("CDRegisterEventCallbackFunction"), err, __FILE__, __LINE__);
}

RemoteReleaseControlImpl::~RemoteReleaseControlImpl() throw()
{
   try
   {
      Close();
   }
   catch (...)
   {
   }
}

void RemoteReleaseControlImpl::Close()
{
   cdHSource hSource = GetSource();

   if (m_hEventCallback != 0)
   {
      cdError err = CDUnregisterEventCallbackFunction(hSource, m_hEventCallback);
      LOG_TRACE(_T("CDUnregisterEventCallbackFunction(%08x, event = %08x) returned %08x\n"),
         hSource, m_hEventCallback, err);
      CheckError(_T("CDUnregisterEventCallbackFunction"), err, __FILE__, __LINE__);
   }

   // may return cdINVALID_HANDLE, cdINVALID_FN_CALL
   cdError err = CDExitReleaseControl(hSource);
   LOG_TRACE(_T("CDExitReleaseControl(%08x) returned %08x\n"), hSource, err);
   CheckError(_T("CDExitReleaseControl"), err, __FILE__, __LINE__);
}

bool RemoteReleaseControlImpl::GetCapability(RemoteReleaseControl::T_enRemoteCapability enCapability) const throw()
{
   switch (enCapability)
   {
   case RemoteReleaseControl::capChangeShootingParameter:
      return (m_releaseControlFaculty & cdRELEASE_CONTROL_CAP_SETPRM) != 0;

   case RemoteReleaseControl::capChangeShootingMode:
      return (m_releaseControlFaculty & cdRELEASE_CONTROL_CAP_SETPRM) != 0;

   case RemoteReleaseControl::capZoomControl:
      return (m_releaseControlFaculty & cdRELEASE_CONTROL_CAP_ZOOM) != 0;

   case RemoteReleaseControl::capViewfinder:
      // Not all camera models support the Viewfinder function. Cameras that support Viewfinder
      // are those cameras for which the cdRELEASE_CONTROL_CAP_VIEWFINDER bit is set. Obtain the
      // value by executing CDGetDevicePropertyData using cdDEVICE_PROP_RELEASE_CONTROL_CAP.
      // note: in remote release mode, CDGetReleaseControlFaculty can also be used
      return (m_releaseControlFaculty & cdRELEASE_CONTROL_CAP_VIEWFINDER) != 0;

   case RemoteReleaseControl::capReleaseWhileViewfinder:
      // If cdRELEASE_CONTROL_CAP_ABORT_VIEWFINDER bit is set in the connected camera model as a
      // value that can be obtained by executing CDGetDevicePropertyData with
      // cdDEVICE_PROP_RELEASE_CONTROL_CAP, and you are shooting with the Viewfinder using the
      // CDRelease function, you must stop Viewfinder first.
      return (m_releaseControlFaculty & cdRELEASE_CONTROL_CAP_ABORT_VIEWFINDER) == 0;

   case RemoteReleaseControl::capAFLock:
      return (m_releaseControlFaculty & cdRELEASE_CONTROL_CAP_AF_LOCK) != 0;

   case RemoteReleaseControl::capBulbMode:
      // bulb mode generally not supported by CDSDK
      return false;

   case RemoteReleaseControl::capUILock:
      return false;

   default:
      ATLASSERT(false);
      break;
   }

   return false;
}

ImageProperty RemoteReleaseControlImpl::MapShootingModeToImagePropertyValue(RemoteReleaseControl::T_enShootingMode enShootingMode) const
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

ImageProperty RemoteReleaseControlImpl::GetImageProperty(unsigned int uiImageProperty) const
{
   // special case: propSaveTo flag
   if (uiImageProperty == MapImagePropertyTypeToId(propSaveTo))
   {
      Variant value;

      value.Set(m_uiRelDataKind);
      value.SetType(Variant::typeUInt16);

      return ImageProperty(variantCdsdk, uiImageProperty, value, true);
   }

   // get value
   cdHSource hSource = GetSource();
   ImagePropertyAccess p(hSource);

   Variant value = p.Get(uiImageProperty);
   bool bReadOnly = p.IsReadOnly(uiImageProperty);

   return ImageProperty(variantCdsdk, uiImageProperty, value, bReadOnly);
}

void RemoteReleaseControlImpl::SetImageProperty(const ImageProperty& imageProperty)
{
   cdHSource hSource = GetSource();
   ImagePropertyAccess p(hSource);

   p.Set(imageProperty.Id(), imageProperty.Value());

   // special case: propSaveTo flag
   if (imageProperty.Id() == MapImagePropertyTypeToId(propSaveTo))
      m_uiRelDataKind = imageProperty.Value().Get<cdRelDataKind>();
}

void RemoteReleaseControlImpl::EnumImagePropertyValues(unsigned int uiImageProperty, std::vector<ImageProperty>& vecValues) const
{
   cdHSource hSource = GetSource();
   ImagePropertyAccess p(hSource);

   std::vector<Variant> vecRawValues;
   p.Enum(uiImageProperty, vecRawValues);

   bool bReadOnly = p.IsReadOnly(uiImageProperty);

   for (size_t i = 0, iMax = vecRawValues.size(); i<iMax; i++)
      vecValues.push_back(ImageProperty(variantCdsdk, uiImageProperty, vecRawValues[i], bReadOnly));
}

std::shared_ptr<Viewfinder> RemoteReleaseControlImpl::StartViewfinder() const
{
   if (!GetCapability(RemoteReleaseControl::capViewfinder))
      throw CameraException(_T("RemoteReleaseControl::StartViewfinder"),
         _T("Not supported"), cdERROR_CLIENT_COMPONENTID | cdNOT_SUPPORTED, __FILE__, __LINE__);

   return std::shared_ptr<Viewfinder>(new ViewfinderImpl(m_spSourceDevice));
}

unsigned int RemoteReleaseControlImpl::NumAvailableShots() const
{
   cdHSource hSource = GetSource();

   cdUInt32 uiAvailShots = 0;

   // may return cdINVALID_HANDLE, cdINVALID_PARAMETER
   cdError err = CDGetNumAvailableShot(hSource, &uiAvailShots);
   LOG_TRACE(_T("CDGetNumAvailableShot(%08x, &shots = %u) returned %08x\n"), hSource, uiAvailShots, err);
   CheckError(_T("CDGetNumAvailableShot"), err, __FILE__, __LINE__);

   return uiAvailShots;
}

void RemoteReleaseControlImpl::SendCommand(RemoteReleaseControl::T_enCameraCommand enCameraCommand)
{
   // the description of the SDK says to use the magic value 7 here
   cdAeAfAwbResetFlag activateFlag = 0;

   switch (enCameraCommand)
   {
   case RemoteReleaseControl::commandAdjustFocus: activateFlag |= cdAEAFAWB_RESET_AF; break;
   case RemoteReleaseControl::commandAdjustWhiteBalance: activateFlag |= cdAEAFAWB_RESET_AWB; break;
   case RemoteReleaseControl::commandAdjustExposure: activateFlag |= cdAEAFAWB_RESET_AE; break;
   default:
      ATLASSERT(false);
      break;
   }

   cdHSource hSource = GetSource();

   // may return cdINVALID_HANDLE, cdNOT_SUPPORTED, cdINVALID_PARAMETER
   cdError err = CDActViewfinderAutoFunctions(hSource, activateFlag);

   LOG_TRACE(_T("CDActViewfinderAutoFunctions(%08x, activateFlag = 7) returned %08x\n"), hSource, err);
   CheckError(_T("CDActViewfinderAutoFunctions"), err, __FILE__, __LINE__);
}

void RemoteReleaseControlImpl::Release()
{
   m_upReleaseThread->Post(std::bind(&RemoteReleaseControlImpl::AsyncRelease, this));
}

void RemoteReleaseControlImpl::AsyncRelease()
{
   // TODO check cdRELEASE_CONTROL_CAP_ABORT_VIEWFINDER if viewfinder has to be terminated to take a picture

   cdUInt32 numData = 0;
   ReleaseShutter(true, numData);

   // only save to camera? then return now
   ShutterReleaseSettings settings;
   {
      LightweightMutex::LockType lock(m_mtxShutterReleaseSettings);
      settings = m_shutterReleaseSettings;
   }

   if (settings.SaveTarget() == ShutterReleaseSettings::saveToCamera)
      return;

   // read data
   CStringA cszaFilename = settings.Filename();
   ReadReleaseData(cszaFilename, numData);

   // call finished handler
   ShutterReleaseSettings::T_fnOnFinishedTransfer fnHandler = settings.HandlerOnFinishedTransfer();
   if (fnHandler != nullptr)
      fnHandler(settings);
}

void RemoteReleaseControlImpl::ReleaseShutter(bool bSync, cdUInt32& numData)
{
   cdHSource hSource = GetSource();

   // may return cdINVALID_HANDLE, cdINVALID_PARAMETER, cdDEVICE_NOT_RELEASED, cdOPERATION_CANCELLED
   cdError err = CDRelease(hSource,
      bSync,
      &RemoteReleaseControlImpl::OnProgressCallback_,
      GetContext(),
      cdPROG_REPORT_PERIODICALLY,
      &numData);

   LOG_TRACE(_T("CDRelease(%08x, sync = %s, &ProgressCallback, context, ")
      _T("ProgReportPeriodically, &numData = %u) returned %08x\n"),
      hSource,
      bSync ? _T("true") : _T("false"),
      numData, err);
   CheckError(_T("CDRelease"), err, __FILE__, __LINE__);
}

void RemoteReleaseControlImpl::ReadReleaseData(const CStringA& cszaFilename, cdUInt32 numData)
{
   cdReleaseImageInfo imageInfo = { 0 };
   cdStgMedium stgMedium;
   stgMedium.Type = cdMEMTYPE_FILE;
   stgMedium.u.lpszFileName = const_cast<char*>(cszaFilename.GetString());

   cdHSource hSource = GetSource();
   for (cdUInt32 i = 0; i < numData; i++)
   {
      // may return cdINVALID_HANDLE, cdINVALID_PARAMETER, cdINVALID_FN_CALL, cdOPERATION_CANCELLED
      cdError err = CDGetReleasedData(hSource,
         &RemoteReleaseControlImpl::OnProgressCallback_,
         GetContext(),
         cdPROG_REPORT_PERIODICALLY,
         &imageInfo,
         &stgMedium);

      LOG_TRACE(_T("CDGetReleasedData(%08x, &ProgressCallback, context, ")
         _T("ProgReportPeriodically, &imageInfo, &stgMedium) returned %08x\n"),
         hSource, err);
      CheckError(_T("CDGetReleasedData"), err, __FILE__, __LINE__);
   }
}

std::shared_ptr<BulbReleaseControl> RemoteReleaseControlImpl::StartBulb()
{
   // bulb not supported by CDSDK
   throw CameraException(_T("RemoteReleaseControl::StartBulb"),
      _T("Not supported"), cdERROR_CDSDK_COMPONENTID | cdNOT_SUPPORTED, __FILE__, __LINE__);
}

cdUInt32 cdSTDCALL RemoteReleaseControlImpl::OnReleaseEventCallback_(
   cdReleaseEventID EventID, const void* pData, cdUInt32 DataSize, cdContext Context)
{
   Variant value;
   if (DataSize > 0)
   {
      const BYTE* pByteData = reinterpret_cast<const BYTE*>(pData);
      std::vector<BYTE> vecData(pByteData, pByteData + DataSize);

      value.SetArray<unsigned char>(vecData);
      value.SetType(Variant::typeUInt8);
   }

   LOG_TRACE(_T("OnReleaseEventCallback(eventId = %u (%s), &data = %s, size = %u, context) called\n"),
      EventID,
      EventID == cdRELEASE_EVENT_RELEASE_START ? _T("ReleaseStart") :
      EventID == cdRESEASE_EVENT_RELEASE_COMPLETE ? _T("ReleaseComplete") :
      EventID == cdRELEASE_EVENT_RESET_HW_ERROR ? _T("ResetHWError") :
      EventID == cdRELEASE_EVENT_CAM_RELEASE_ON ? _T("CamReleaseOn") :
      EventID == cdRELEASE_EVENT_CHANGED_BY_UI ? _T("ChangedByUI") :
      EventID == cdRELEASE_EVENT_ABORT_PC_EVF ? _T("AbortPCEvf") :
      EventID == cdRELEASE_EVENT_ENABLE_PC_EVF ? _T("EnablePCEvf") : _T("???"),
      DataSize > 0 ? value.ToString().GetString() : _T(""),
      DataSize);

#pragma warning(push)
#pragma warning(disable: 4312) // 'reinterpret_cast' : conversion from 'T' to 'P' of greater size
   RemoteReleaseControlImpl* pThis = reinterpret_cast<RemoteReleaseControlImpl*>(Context);
   ATLASSERT(pThis != nullptr);
#pragma warning(pop)

   pThis->OnReleaseEventCallback(EventID);

   return cdOK;
}

void RemoteReleaseControlImpl::OnReleaseEventCallback(cdReleaseEventID EventID)
{
   switch (EventID)
   {
   case cdRESEASE_EVENT_RELEASE_COMPLETE:
      // TODO start downloading image
      break;

   case cdRELEASE_EVENT_RESET_HW_ERROR:
      // TODO pass error code
      m_subjectStateEvent.Call(RemoteReleaseControl::stateEventReleaseError, 0);
      break;

   case cdRELEASE_EVENT_CAM_RELEASE_ON: // Release pushed
      Release();
      break;

   case cdRELEASE_EVENT_CHANGED_BY_UI:
      // notify that properties have changed; use 0 as id to signal that all must be updated
      m_subjectPropertyEvent.Call(RemoteReleaseControl::propEventPropertyDescChanged, 0);
      m_subjectPropertyEvent.Call(RemoteReleaseControl::propEventPropertyChanged, 0);
      break;

   default:
      break;
   }
}

cdUInt32 cdSTDCALL RemoteReleaseControlImpl::OnEventCallback_(
   cdEventID EventID, const void* pData, cdUInt32 DataSize, cdContext Context)
{
   Variant value;
   if (DataSize > 0)
   {
      const BYTE* pByteData = reinterpret_cast<const BYTE*>(pData);
      std::vector<BYTE> vecData(pByteData, pByteData + DataSize);

      value.SetArray<unsigned char>(vecData);
      value.SetType(Variant::typeUInt8);
   }

   cdEventID severityId = cdEVENT_SEVERITY(EventID);
   cdEventID realEventId = cdEVENT_EVENTID(EventID);

   LOG_TRACE(_T("OnEventCallback(eventId = %08x (%s, %s), &data = %s, size = %u, context) called\n"),
      EventID,
      severityId == cdEVENT_SEVERITY_NONE ? _T("SeverityNone") :
      severityId == cdEVENT_SEVERITY_WARNING ? _T("SeverityWarning") :
      severityId == cdEVENT_SEVERITY_SHUTDOWN ? _T("SeverityShutdown") : _T("???"),
      realEventId == cdEVENT_BATTERY_LEVEL_CHANGED ? _T("EventBatteryLevelChanged") :
      realEventId == cdEVENT_DIAL_CHANGE ? _T("EventDialChange") :
      realEventId == cdEVENT_CF_GATE_CHANGED ? _T("EventCFGateChanged") :
      realEventId == cdEVENT_BATT_COVER_CHANGED ? _T("EventBattCoverChanged") :
      realEventId == cdEVENT_CONNECTION_TIME_OUT ? _T("EventConnectionTimeOut") :
      realEventId == cdEVENT_CONNECTION_DISAPPEARED ? _T("EventConnectionDisappeared") : _T("???"),
      DataSize > 0 ? value.ToString().GetString() : _T(""),
      DataSize);

#pragma warning(push)
#pragma warning(disable: 4312) // 'reinterpret_cast' : conversion from 'T' to 'P' of greater size
   RemoteReleaseControlImpl* pThis = reinterpret_cast<RemoteReleaseControlImpl*>(Context);
   ATLASSERT(pThis != nullptr);
#pragma warning(pop)

   pThis->OnEventCallback(EventID);

   if (realEventId == cdEVENT_CONNECTION_TIME_OUT)
      return cdDEVICE_STAY_AWAKE; // keep the camera awake

   return cdOK;
}

void RemoteReleaseControlImpl::OnEventCallback(cdEventID eventId)
{
   cdEventID severityId = cdEVENT_SEVERITY(eventId);
   cdEventID realEventId = cdEVENT_EVENTID(eventId);

   switch (realEventId)
   {
   case cdEVENT_BATTERY_LEVEL_CHANGED:
      m_subjectPropertyEvent.Call(RemoteReleaseControl::propEventPropertyChanged,
         MapImagePropertyTypeToId(T_enImagePropertyType::propBatteryLevel));
      break;

   case cdEVENT_CF_GATE_CHANGED:
      m_subjectStateEvent.Call(RemoteReleaseControl::stateEventMemoryCardSlotOpen, 0);
      break;

   default:
      break;
   }

   if (cdEVENT_SEVERITY_SHUTDOWN == severityId)
      m_subjectStateEvent.Call(RemoteReleaseControl::stateEventCameraShutdown, 0);
}

cdUInt32 cdSTDCALL RemoteReleaseControlImpl::OnProgressCallback_(cdUInt32 Progress,
   cdProgressStatus Status, cdContext Context)
{
   LOG_TRACE(_T("OnProgressCallback(progress = %u%%, status = %08x (%s), context) called\n"),
      Progress, Status,
      Status == cdSTATUS_TRANSFER ? _T("StatusTransfer") :
      Status == cdSTATUS_CONVERT ? _T("StausConvert") :
      Status == cdSTATUS_DEVELOPMENT ? _T("StatusDevelopment") : _T("???"));

#pragma warning(push)
#pragma warning(disable: 4312) // 'reinterpret_cast' : conversion from 'T' to 'P' of greater size
   RemoteReleaseControlImpl* pThis = reinterpret_cast<RemoteReleaseControlImpl*>(Context);
   ATLASSERT(pThis != nullptr);
#pragma warning(pop)

   bool bRet = pThis->OnProgressCallback(Progress, Status);

   return bRet ? cdOK : cdOPERATION_CANCELLED;
}

bool RemoteReleaseControlImpl::OnProgressCallback(cdUInt32 Progress, cdProgressStatus Status)
{
   if (Status == cdSTATUS_TRANSFER)
      m_subjectDownloadEvent.Call(RemoteReleaseControl::downloadEventInProgress, Progress);

   return true;
}
