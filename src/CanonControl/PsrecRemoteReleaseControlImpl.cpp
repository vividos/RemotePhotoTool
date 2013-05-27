//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file PsrecRemoteReleaseControlImpl.cpp PS-ReC - RemoteReleaseControl impl
//

// includes
#include "stdafx.h"
#include "ShutterReleaseSettings.hpp"
#include "PsrecRemoteReleaseControlImpl.hpp"
#include "PsrecSourceDeviceImpl.hpp"
#include "PsrecVarDataParser.hpp"

using namespace PSREC;

/// 256 kb per transfer
const unsigned int c_uiImageDownloadBufferSizeKb = 256;

/// camera event data
struct PSREC::CameraEventData: public VarDataParser
{
public:
   /// ctor; parses event data
   CameraEventData(prVoid* pEventData)
   {
      BYTE* pbData = reinterpret_cast<BYTE*>(pEventData);

      std::vector<BYTE>& vecBuffer = GetBuffer();
      vecBuffer.assign(pbData, pbData+12);// at least 12 bytes

      // read length of event data
      prUInt32 uiLength = ReadUint32();

      vecBuffer.assign(pbData, pbData+uiLength);

      unsigned int uiNumParams = (uiLength - 12) / 4;
      Parse(uiNumParams);
   }

   /// parses event data
   void Parse(unsigned int uiNumParam)
   {
      // note that buffer pointer is at pos 4 already
      prUInt16 uiContainerType = ReadUint16();
      m_uiEventCode = ReadUint16();
      prUInt32 uiTransactionId = ReadUint32();

      uiContainerType; uiTransactionId; // unused

      // read all parameters
      for (unsigned int ui=0; ui<uiNumParam; ui++)
         m_vecParams.push_back(ReadUint32());
   }

   /// event code
   prUInt16 m_uiEventCode;

   /// event parameters (optional)
   std::vector<prUInt32> m_vecParams;
};


RemoteReleaseControlImpl::RemoteReleaseControlImpl(prHandle hCamera, std::shared_ptr<SourceDeviceImpl> spSourceDevice)
:m_spSourceDevice(spSourceDevice),
 m_hCamera(hCamera),
 m_evtReleaseImageReady(false, false), // auto-reset event
 m_evtReleaseImageTransferInProgress(true, false), // manual-reset event
 m_evtReleaseImageTransferDone(true, false), // manual-reset event
 m_fdImageTransfer(nullptr)
{
   // may return prINVALID_FN_CALL, prINVALID_HANDLE, prMEM_ALLOC_FAILED or @ERR
   prResponse err = PR_InitiateReleaseControl(hCamera);
   LOG_TRACE(_T("PR_InitiateReleaseControl(%08x) returned %08x\n"), hCamera, err);
   CheckError(_T("PR_InitiateReleaseControl"), err, __FILE__, __LINE__);

   // register event handler
   // may return prINVALID_FN_CALL, prINVALID_HANDLE, prINVALID_PARAMETER, prEVENT_CALLBACK_EXIST or @ERR
   err = PR_SetEventCallBack(hCamera, reinterpret_cast<prContext>(this), &RemoteReleaseControlImpl::OnCameraEvent_);
   LOG_TRACE(_T("PR_SetEventCallBack(%08x, this, OnCameraEvent) returned %08x\n"), hCamera, err);
   CheckError(_T("PR_SetEventCallBack"), err, __FILE__, __LINE__);

   // get device info from source device so that we don't have to query every time
   //m_spDeviceInfo = spSourceDevice->GetDeviceInfo();
   // get device info again; when in remote release control, the list contains changeable properties
   m_spDeviceInfo.reset(new DeviceInfo(m_hCamera));
}

RemoteReleaseControlImpl::~RemoteReleaseControlImpl() throw()
{
   // unregister event handler
   // may return prINVALID_FN_CALL, prINVALID_HANDLE or @ERR
   prResponse err = PR_ClearEventCallBack(m_hCamera);
   LOG_TRACE(_T("PR_ClearEventCallBack(%08x) returned %08x\n"), m_hCamera, err);

   // may return prINVALID_FN_CALL, prINVALID_HANDLE, prMEM_ALLOC_FAILED or @ERR
   err = PR_TerminateReleaseControl(m_hCamera);
   LOG_TRACE(_T("PR_TerminateReleaseControl(%08x) returned %08x\n"), m_hCamera, err);
}

bool RemoteReleaseControlImpl::GetCapability(RemoteReleaseControl::T_enRemoteCapability enCapability) const throw()
{
   ATLASSERT(m_spDeviceInfo != nullptr);

   try
   {
      // get device capabilities
      DeviceInfo& di = *m_spDeviceInfo.get();

      switch (enCapability)
      {
      case RemoteReleaseControl::capChangeShootingParameter:
         return di.m_setEventsSupported.find(prPTP_DEVICE_PROP_CHANGED) != di.m_setEventsSupported.end();

      case RemoteReleaseControl::capChangeShootingMode:
         {
            // check image property
            DevicePropDesc desc(m_hCamera, prPTP_DEV_PROP_EXPOSURE_MODE, false);

            return desc.IsSetAllowed();
         }

      case RemoteReleaseControl::capZoomControl:
         {
            // check image property
            DevicePropDesc desc(m_hCamera, prPTP_DEV_PROP_ZOOM_POS, false);

            return desc.IsSetAllowed();
         }

      case RemoteReleaseControl::capViewfinder:
         return di.m_setOperationsSupported.find(prPTP_RC_INITIATE_VIEW_FINDER) != di.m_setOperationsSupported.end();

      case RemoteReleaseControl::capReleaseWhileViewfinder:
         // generally possible with PSREC
         return true;

      case RemoteReleaseControl::capAFLock:
         return di.m_setOperationsSupported.find(prPTP_RC_FOCUS_LOCK) != di.m_setOperationsSupported.end();

      case RemoteReleaseControl::capBulbMode:
         // bulb mode generally not supported by PSREC
         return false;

      default:
         ATLASSERT(false);
      }
   }
   catch(const CameraException& ex)
   {
      LOG_TRACE(_T("GetCapability(): caught CameraException: %s\n"), ex.Message());
   }
   return false;
}

std::vector<unsigned int> RemoteReleaseControlImpl::EnumImageProperties() const
{
   ATLASSERT(m_spDeviceInfo != nullptr);
   DeviceInfo& info = *m_spDeviceInfo.get();

   DeviceInfo& parentInfo = *m_spSourceDevice->GetDeviceInfo();

   // put the device property id's into the vector
   std::vector<unsigned int> vecProperties;
   std::for_each(info.m_setDevicePropertiesSupported.begin(), info.m_setDevicePropertiesSupported.end(), [&](prUInt16 uiPropId)
   {
      // filter out ids already in device properties
      if (parentInfo.m_setDevicePropertiesSupported.find(uiPropId) == parentInfo.m_setDevicePropertiesSupported.end())
         vecProperties.push_back(uiPropId);
      else
      {
         LOG_TRACE(_T("image property ignored, already in device properties: %04x, %s\n"),
            uiPropId,
            PSREC::PropertyAccess::NameFromId(uiPropId));
      }
   });

   return vecProperties;
}

void RemoteReleaseControlImpl::SendCommand(RemoteReleaseControl::T_enCameraCommand enCameraCommand)
{
   prptpAeAfAwbResetFlag resetFlag = 0;

   switch (enCameraCommand)
   {
   case RemoteReleaseControl::commandAdjustFocus:
      resetFlag = prptpAEAFAWB_RESET_AF;
      break;

   case RemoteReleaseControl::commandAdjustWhiteBalance:
      resetFlag = prptpAEAFAWB_RESET_AWB;
      break;

   // TODO support prptpAEAFAWB_RESET_AE ?

   default:
      ATLASSERT(false);
      return;
   }

   if (resetFlag == 0)
      return;

   // may return prINVALID_FN_CALL, prINVALID_HANDLE, prMEM_ALLOC_FAILED or @ERR
   prResponse err = PR_RC_DoAeAfAwb(m_hCamera, resetFlag);
   LOG_TRACE(_T("PR_RC_DoAeAfAwb(%08x, resetFlag = %u) returned %08x\n"), m_hCamera, resetFlag, err);
   CheckError(_T("PR_RC_DoAeAfAwb"), err, __FILE__, __LINE__);
}

void RemoteReleaseControlImpl::Release(const ShutterReleaseSettings& settings)
{
   // if another transfer is in progress, wait for completion
   if (m_evtReleaseImageTransferInProgress.Wait(0))
   {
      m_evtReleaseImageTransferDone.Wait();
   }

   // TODO hopefully no further request comes between waiting and setting the event below
   m_evtReleaseImageTransferInProgress.Set();


   // set save target
   ShutterReleaseSettings::T_enSaveTarget enSaveTarget = settings.SaveTarget();

   PropertyAccess pa(m_hCamera);
   Variant val;
   // note: PSREC doesn't have a value for saveToBoth
   val.Set<prUInt16>(
      enSaveTarget == ShutterReleaseSettings::saveToHost ? 0x0002 :
      enSaveTarget == ShutterReleaseSettings::saveToCamera ? 0x0008 : 0x000a );
   val.SetType(Variant::typeUInt16);
   pa.Set(prPTP_DEV_PROP_CAPTURE_TRANSFER_MODE, val);


   {
      LightweightMutex::LockType lock(m_mtxCurrentShutterReleaseSettings);
      m_currentShutterReleaseSettings = settings;
   }


   DWORD dwStart = GetTickCount();

   // may return prWAIT_TIMEOUT_ERROR?, prINVALID_FN_CALL, prINVALID_HANDLE, prMEM_ALLOC_FAILED or @ERR
   prResponse err = PR_RC_Release(m_hCamera);
   LOG_TRACE(_T("PR_RC_Release(%08x) returned %08x\n"), m_hCamera, err);
   CheckError(_T("PR_RC_Release"), err, __FILE__, __LINE__);

   DWORD dwEnd = GetTickCount();
   LOG_TRACE(_T("PR_RC_Release took %u ms\n"), dwEnd-dwStart);


   // wait for event
   m_evtReleaseImageReady.Wait();

   StartImageDownload(m_hReleaseImage, true);
}

LPCTSTR RemoteReleaseControlImpl::EventNameFromCode(prUInt16 uiEventCode)
{
   switch (uiEventCode)
   {
   case prPTP_DEVICE_PROP_CHANGED: return _T("Device property has been changed.");
   case prPTP_CAPTURE_COMPLETE: return _T("Capture has finished.");
   case prPTP_SHUTDOWN_CF_GATE_WAS_OPENED: return _T("The Device has shut down due to the opening of the SD card cover.");
   case prPTP_RESET_HW_ERROR: return _T("The device has generated a hardware error.");
   case prPTP_ABORT_PC_EVF: return _T("The Viewfinder mode has been cancelled.");
   case prPTP_ENABLE_PC_EVF: return _T("The Viewfinder mode has been enabled.");
   case prPTP_FULL_VIEW_RELEASED: return _T("Transfer timing of main image data");
   case prPTP_THUMBNAIL_RELEASED: return _T("Transfer timing of thumbnail image data");
   case prPTP_CHANGE_BATTERY_STATUS: return _T("The power condition of the camera has changed.");
   case prPTP_PUSHED_RELEASE_SW: return _T("User has pressed the release swtich on camera.");
   case prPTP_RC_PROP_CHANGED: return _T("A group of properties relating to release control have been changed.");
   case prPTP_RC_ROTATION_ANGLE_CHANGED: return _T("The angle of rotation of the camera has been changed.");
   case prPTP_RC_CHANGED_BY_CAM_UI: return _T("An operation control on the camera has been operated.");
   case prCAL_SHUTDOWN: return _T("Shutdown");
   default:
      return _T("???");
      break;
   }
}

prResponse prSTDCALL RemoteReleaseControlImpl::OnCameraEvent_(prHandle hCamera, prContext Context, prVoid* pEventData)
{
   hCamera;

   RemoteReleaseControlImpl* pThis = reinterpret_cast<RemoteReleaseControlImpl*>(Context);

   PSREC::CameraEventData eventData(pEventData);

   LOG_TRACE(_T("PSREC::OnCameraEvent(%08x, this, event) called with event code 0x%04x \"%s\", %u params\n"),
      hCamera, eventData.m_uiEventCode,
      EventNameFromCode(eventData.m_uiEventCode),
      eventData.m_vecParams.size());

   LOG_TRACE(_T("   Parameters:"));
   for (size_t i=0,iMax=eventData.m_vecParams.size(); i<iMax; i++)
      LOG_TRACE(_T(" %08x"), eventData.m_vecParams[i]);

   LOG_TRACE(_T("\n"));

   pThis->OnCameraEvent(eventData);

   return prOK;
}

void RemoteReleaseControlImpl::OnCameraEvent(const CameraEventData& eventData)
{
   switch (eventData.m_uiEventCode)
   {
   case prPTP_DEVICE_PROP_CHANGED:
   case prPTP_RC_PROP_CHANGED:
      {
         bool bImageProperty = eventData.m_uiEventCode == prPTP_RC_PROP_CHANGED;

         prUInt16 uiPropertyId = 0;
         if (!bImageProperty)
         {
            ATLASSERT(eventData.m_vecParams.size() == 1);
            uiPropertyId = static_cast<prUInt16>(eventData.m_vecParams[0] & 0xFFFF);
         }

         // update description, too, when it's an image property
         OnEventPropertyChanged(uiPropertyId, bImageProperty);
      }
      break;

   case prPTP_CAPTURE_COMPLETE: // capture complete: sent when transfer to host wasn't set
      ATLASSERT(eventData.m_vecParams.size() >= 1);
      LOG_TRACE(_T("prPTP_CAPTURE_COMPLETED: error=%08x\n"), eventData.m_vecParams[0]);
      break;

   case prPTP_SHUTDOWN_CF_GATE_WAS_OPENED:
      LOG_TRACE(_T("prPTP_SHUTDOWN_CF_GATE_WAS_OPENED\n"));

      OnEventStateChanged(eventData.m_uiEventCode);
      break;

   case prPTP_RESET_HW_ERROR:
      ATLASSERT(eventData.m_vecParams.size() >= 1);
      LOG_TRACE(_T("prPTP_RESET_HW_ERROR: error=%08x\n"), eventData.m_vecParams[0]);

      OnEventStateChanged(eventData.m_uiEventCode);
      break;

   case prPTP_ABORT_PC_EVF:
      LOG_TRACE(_T("prPTP_ABORT_PC_EVF\n"));
      break;

   case prPTP_ENABLE_PC_EVF:
      LOG_TRACE(_T("prPTP_ENABLE_PC_EVF\n"));
      break;

   case prPTP_FULL_VIEW_RELEASED:
   case prPTP_THUMBNAIL_RELEASED:
      // full image/thumbnail is about to be transferred
      {
         ATLASSERT(eventData.m_vecParams.size() >= 1);

         prObjectHandle hObject = static_cast<prObjectHandle>(eventData.m_vecParams[0]);
         //bool bFullView = prPTP_FULL_VIEW_RELEASED == eventData.m_uiEventCode;

         m_hReleaseImage = hObject;
         m_evtReleaseImageReady.Set();

         //StartImageDownload(hObject, bFullView);
      }
      break;

   case prPTP_CHANGE_BATTERY_STATUS:
      {
         ATLASSERT(eventData.m_vecParams.size() >= 2);
         LOG_TRACE(_T("prPTP_CHANGE_BATTERY_STATUS type=%08x level=%08x\n"),
            eventData.m_vecParams[0],
            eventData.m_vecParams[1]);

         // also send property changes
         OnEventPropertyChanged(prPTP_DEV_PROP_BATTERY_STATUS, false);
         OnEventPropertyChanged(prPTP_DEV_PROP_BATTERY_KIND, false);
      }
      break;

   case prPTP_PUSHED_RELEASE_SW:
      // user pushed the release
      // note: we have to start the release ourselves, here
      {
         LOG_TRACE(_T("prPTP_PUSHED_RELEASE_SW: user pressed shutter release switch\n"));

         // TODO another thread has to call Release() to allow PRLIB thread to continue work
         //const ShutterReleaseSettings& settings = m_defaultShutterReleaseSettings;
         //Release(settings);
      }
      break;

   case prPTP_RC_ROTATION_ANGLE_CHANGED:
      ATLASSERT(eventData.m_vecParams.size() >= 1);
      LOG_TRACE(_T("prPTP_RC_ROTATION_ANGLE_CHANGED: rotation=%08x\n"), eventData.m_vecParams[0]);

      OnEventStateChanged(eventData.m_uiEventCode);

      // also send property changes
      OnEventPropertyChanged(prPTP_DEV_PROP_ROTATION_ANGLE, false);
      break;

   case prPTP_RC_CHANGED_BY_CAM_UI:
      LOG_TRACE(_T("prPTP_RC_CHANGED_BY_CAM_UI\n"));
      OnEventPropertyChanged(0, true);
      break;

   case prCAL_SHUTDOWN:
      // camera is shut down
      OnEventStateChanged(eventData.m_uiEventCode);
      break;

   case 0x400c:
      LOG_TRACE(_T("unknown camera event 0x400c: param1=%08x\n"), eventData.m_vecParams[0]);
      break;

   default:
      LOG_TRACE(_T("unknown camera event %04x: param1=%08x\n"), eventData.m_uiEventCode, eventData.m_vecParams[0]);
      //ATLASSERT(false);
      break;
   }
}

void RemoteReleaseControlImpl::GetChangedProperties(std::vector<prUInt16>& vecChangedProperties)
{
   // note: we try this even when prPTP_RC_GET_CHANGED_RELEASE_PARAMS_LIST
   // is not in the OperationSupported list of device info.

   // may return prINVALID_FN_CALL, prINVALID_HANDLE, prINVALID_PARAMETER, prMEM_ALLOC_FAILED, prINSUFFICIENT_BUFFER or @ERR
   prUInt32 uiSize = 0;
   prResponse err = PR_RC_GetChangedReleaseParamesList(m_hCamera, &uiSize, nullptr);
   if ((err & prERROR_ERRORID_MASK) != prINSUFFICIENT_BUFFER)
   {
      LOG_TRACE(_T("PR_RC_GetChangedReleaseParamesList(handle = %08x, 0, nullptr) returned %08x, buffer size %u\n"),
         m_hCamera, err, uiSize);
      CheckError(_T("PR_RC_GetChangedReleaseParamesList"), err, __FILE__, __LINE__);
   }

   if (uiSize == 0)
      return; // no data available

   // read data bytes
   std::vector<BYTE> vecBuffer(uiSize);
   prVoid* pChangedParams = reinterpret_cast<prVoid*>(&vecBuffer[0]);

   // may return prINVALID_FN_CALL, prINVALID_HANDLE, prINVALID_PARAMETER, prMEM_ALLOC_FAILED, prINSUFFICIENT_BUFFER or @ERR
   err = PR_RC_GetChangedReleaseParamesList(m_hCamera, &uiSize, pChangedParams);

   LOG_TRACE(_T("PR_RC_GetChangedReleaseParamesList(hCamera = %08x, size = %u, &p) returned %08x\n"),
      m_hCamera, uiSize, err);
   CheckError(_T("PR_RC_GetChangedReleaseParamesList"), err, __FILE__, __LINE__);

   // check array
   prUInt16& numElements = *(reinterpret_cast<prUInt16*>(pChangedParams));
   prptpDevicePropCode* pProperties = reinterpret_cast<prptpDevicePropCode*>(pChangedParams) + 2;

   for (prUInt16 ui=0; ui<numElements; ui++)
   {
      LOG_TRACE(_T("   changed property: %04x (%s\n"),
         pProperties[ui],
         PropertyAccess::NameFromId(pProperties[ui]));

      vecChangedProperties.push_back(pProperties[ui]);
   }
}

void RemoteReleaseControlImpl::OnEventPropertyChanged(prUInt16 propId, bool bAlsoUpdateDescription)
{
   if (propId == 0)
   {
      // try to get changed properties
      std::vector<prUInt16> vecChangedProperties;
      GetChangedProperties(vecChangedProperties);

      if (!vecChangedProperties.empty())
      {
         std::for_each(vecChangedProperties.begin(), vecChangedProperties.end(), [&](std::vector<prUInt16>::value_type& val)
         {
            if (bAlsoUpdateDescription)
               m_subjectPropertyEvent.Call(RemoteReleaseControl::propEventPropertyDescChanged, val);

            m_subjectPropertyEvent.Call(RemoteReleaseControl::propEventPropertyChanged, val);
         });
         return;
      }
   }

   try
   {
      if (bAlsoUpdateDescription)
         m_subjectPropertyEvent.Call(RemoteReleaseControl::propEventPropertyDescChanged, propId);

      m_subjectPropertyEvent.Call(RemoteReleaseControl::propEventPropertyChanged, propId);
   }
   catch(...)
   {
   }
}

void RemoteReleaseControlImpl::OnEventStateChanged(prUInt16 code)
{
   RemoteReleaseControl::T_enStateEvent enStateEvent =
      RemoteReleaseControl::stateEventInvalid;

   switch(code)
   {
   case prPTP_SHUTDOWN_CF_GATE_WAS_OPENED:
      enStateEvent = RemoteReleaseControl::stateEventMemoryCardSlotOpen;
      break;
   case prPTP_RESET_HW_ERROR:
      enStateEvent = RemoteReleaseControl::stateEventCameraShutdown;
      break;
   case prPTP_RC_ROTATION_ANGLE_CHANGED:
      enStateEvent = RemoteReleaseControl::stateEventRotationAngle;
      break;
   case prCAL_SHUTDOWN:
      enStateEvent = RemoteReleaseControl::stateEventCameraShutdown;
      break;
   default:
      ATLASSERT(false);
      break;
   }

   try
   {
      m_subjectStateEvent.Call(enStateEvent, 0);
   }
   catch(...)
   {
   }
}

void RemoteReleaseControlImpl::StartImageDownload(prObjectHandle hObject, bool bFullView)
{
   m_subjectDownloadEvent.Call(RemoteReleaseControl::downloadEventStarted, 0);

   m_evtReleaseImageTransferInProgress.Set(); // start transfer progress

   prUInt32 uiTransSize = c_uiImageDownloadBufferSizeKb * 1024;

   // may return prINVALID_FN_CALL, prINVALID_HANDLE, prMEM_ALLOC_FAILED, prINVALID_PARAMETER or @ERR
   prResponse err = PR_RC_GetReleasedData(m_hCamera, hObject,
      bFullView ? prPTP_FULL_VIEW_RELEASED : prPTP_THUMBNAIL_RELEASED,
      uiTransSize,
      reinterpret_cast<prContext>(this),
      &RemoteReleaseControlImpl::OnDownloadImageData_);

   LOG_TRACE(_T("PR_RC_GetReleasedData(%08x, objectHandle = %08x) returned %08x\n"), m_hCamera, hObject, err);
   CheckError(_T("PR_RC_GetReleasedData"), err, __FILE__, __LINE__);

   m_subjectDownloadEvent.Call(RemoteReleaseControl::downloadEventFinished, 0);
}

prResponse RemoteReleaseControlImpl::OnDownloadImageData_(
   prHandle       /*hCamera*/,
   prObjectHandle /*hObject*/,
   prContext      Context,
   prProgress*    pProgress)
{
   bool bCanceled = false; // TODO move to member variable, to allow canceling transfer
   if (bCanceled)
      return prOPERATION_CANCELLED;

   LOG_TRACE(_T("PSREC::OnDownloadImageData handler called\n"));
   LOG_TRACE(_T("   message=%08x, status=%08x, percent=%u%%, offset=%08x, length=%08x\n"),
      pProgress->lMessage,
      pProgress->lStatus,
      pProgress->lPercentComplete,
      pProgress->lOffset,
      pProgress->lLength);

   // TODO move code to own function
   RemoteReleaseControlImpl* pThis = reinterpret_cast<RemoteReleaseControlImpl*>(Context);

   pThis->m_subjectDownloadEvent.Call(RemoteReleaseControl::downloadEventInProgress, pProgress->lPercentComplete);

   bool bRet = pThis->OnDownloadImageData(pProgress->lMessage, pProgress->pbData, pProgress->lLength);

   return bRet ? prOK : prOPERATION_CANCELLED;
}

bool RemoteReleaseControlImpl::OnDownloadImageData(prProgressMsg message, prUInt8* pbData, prUInt32 uiLength)
{
   switch(message)
   {
   case prMSG_DATA_HEADER:
      OnStartDownloadImageData();
      if (m_fdImageTransfer == nullptr)
         return false;
      break;

   case prMSG_DATA:
      // actual data available
      ATLASSERT(m_fdImageTransfer != nullptr);
      fwrite(pbData, 1, uiLength, m_fdImageTransfer);
      break;

   case prMSG_TERMINATION:
      OnFinishedDownloadImageData();
      break;
   }

   return true;
}

void RemoteReleaseControlImpl::OnStartDownloadImageData()
{
   ShutterReleaseSettings settings;
   {
      LightweightMutex::LockType lock(m_mtxCurrentShutterReleaseSettings);
      settings = m_currentShutterReleaseSettings;
   }

   // note that settings stay the same during transfer, by locking with the two events

   CString cszFilename = settings.Filename();

   FILE* fd = nullptr;
   errno_t err = _tfopen_s(&fd, cszFilename, _T("wb"));
   if (err != 0 || fd == nullptr)
      return; // couldn't open file

   m_fdImageTransfer = fd;
}

void RemoteReleaseControlImpl::OnFinishedDownloadImageData()
{
   fclose(m_fdImageTransfer);
   m_fdImageTransfer = nullptr;

   // need to set this before notifying client
   m_evtReleaseImageTransferDone.Set();

   // notify client
   ShutterReleaseSettings settings;
   {
      LightweightMutex::LockType lock(m_mtxCurrentShutterReleaseSettings);
      settings = m_currentShutterReleaseSettings;
   }

   ShutterReleaseSettings::T_fnOnFinishedTransfer fnOnFinishedTransfer = settings.HandlerOnFinishedTransfer();
   if (!fnOnFinishedTransfer.empty())
      fnOnFinishedTransfer(settings);
}
