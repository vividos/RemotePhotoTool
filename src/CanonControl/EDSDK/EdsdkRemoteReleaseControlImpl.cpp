//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file EdsdkRemoteReleaseControlImpl.cpp EDSDK - RemoteReleaseControl impl
//

// includes
#include "stdafx.h"
#include "EdsdkRemoteReleaseControlImpl.hpp"
#include "ShutterReleaseSettings.hpp"
#include <ulib/Path.hpp>
#include "AsyncReleaseControlThread.hpp"

using namespace EDSDK;

RemoteReleaseControlImpl::RemoteReleaseControlImpl(std::shared_ptr<SourceDevice> spSourceDevice, const Handle& hCamera)
:m_spSourceDevice(spSourceDevice),
 m_hCamera(hCamera),
 m_upReleaseThread(new AsyncReleaseControlThread),
 m_spMtxLock(new LightweightMutex),
 m_shutterReleaseSettings(ShutterReleaseSettings::saveToCamera), // default is to just save to camera
 m_evtShutterReleaseOccured(false),
 m_uiCurrentZoomPos(kEdsEvfZoom_Fit)
{
   LOG_TRACE(_T("Start registering event handler\n"));

   // start receiving property events
   EdsError err = EdsSetPropertyEventHandler(hCamera.Get(), kEdsPropertyEvent_All, &RemoteReleaseControlImpl::OnPropertyChange_, this);
   LOG_TRACE(_T("EdsSetPropertyEventHandler(ref = %08x, All, &OnPropertyChange) returned %08x\n"), hCamera.Get(), err);
   EDSDK::CheckError(_T("EdsSetPropertyEventHandler"), err, __FILE__, __LINE__);

   // start receiving state events
   err = EdsSetCameraStateEventHandler(hCamera.Get(), kEdsStateEvent_All, &RemoteReleaseControlImpl::OnStateChange_, this);
   LOG_TRACE(_T("EdsSetCameraStateEventHandler(ref = %08x, All, &OnStateChange) returned %08x\n"), hCamera.Get(), err);
   EDSDK::CheckError(_T("EdsSetCameraStateEventHandler"), err, __FILE__, __LINE__);

   // start receiving object events
   err = EdsSetObjectEventHandler(hCamera.Get(), kEdsObjectEvent_All, &RemoteReleaseControlImpl::OnObjectChange_, this);
   LOG_TRACE(_T("EdsSetObjectEventHandler(ref = %08x, All, &OnObjectChange) returned %08x\n"), hCamera.Get(), err);
   EDSDK::CheckError(_T("EdsSetObjectEventHandler"), err, __FILE__, __LINE__);

   LOG_TRACE(_T("Finished registering event handler\n"));
}

RemoteReleaseControlImpl::~RemoteReleaseControlImpl()
{
   m_subjectPropertyEvent.Clear();
   m_subjectStateEvent.Clear();

   LOG_TRACE(_T("Start unregistering event handler\n"));

   try
   {
      Close();
   }
   catch (...)
   {
   }

   LOG_TRACE(_T("Finished unregistering event handler\n"));
}

void RemoteReleaseControlImpl::Close()
{
   EdsError err = EdsSetPropertyEventHandler(m_hCamera.Get(), kEdsPropertyEvent_All, nullptr, this);
   LOG_TRACE(_T("EdsSetPropertyEventHandler(ref = %08x, All, nullptr) returned %08x\n"), m_hCamera.Get(), err);
   EDSDK::CheckError(_T("EdsSetPropertyEventHandler"), err, __FILE__, __LINE__);

   err = EdsSetCameraStateEventHandler(m_hCamera.Get(), kEdsStateEvent_All, nullptr, this);
   LOG_TRACE(_T("EdsSetCameraStateEventHandler(ref = %08x, All, nullptr) returned %08x\n"), m_hCamera.Get(), err);
   EDSDK::CheckError(_T("EdsSetCameraStateEventHandler"), err, __FILE__, __LINE__);

   err = EdsSetObjectEventHandler(m_hCamera.Get(), kEdsObjectEvent_All, nullptr, this);
   LOG_TRACE(_T("EdsSetObjectEventHandler(ref = %08x, All, nullptr) returned %08x\n"), m_hCamera.Get(), err);
   EDSDK::CheckError(_T("EdsSetObjectEventHandler"), err, __FILE__, __LINE__);

   m_hCamera = Handle();
}

EdsError RemoteReleaseControlImpl::OnPropertyChange_(
   EdsPropertyEvent inEvent,
   EdsPropertyID inPropertyID,
   EdsUInt32 inParam,
   EdsVoid* inContext)
{
   UNUSED(inParam);
   //LOG_TRACE(_T("OnPropertyChange(event = %04x (%s), propId = %08x, param = %u, ctx) called\n"),
   //   inEvent,
   //   inEvent == kEdsPropertyEvent_PropertyChanged ? _T("PropertyChanged") :
   //      inEvent == kEdsPropertyEvent_PropertyDescChanged ? _T("PropertyDescChanged") : _T("???"),
   //   inPropertyID,
   //   inParam);

   RemoteReleaseControlImpl* prrc = reinterpret_cast<RemoteReleaseControlImpl*>(inContext);

   // map special property id to 0; this means that user has to update all known property values and descriptions
   if (inPropertyID == 0x0000FFFF)
      inPropertyID = 0;

   prrc->OnPropertyChange(inEvent, inPropertyID, inParam);

   return EDS_ERR_OK;
}

void RemoteReleaseControlImpl::OnPropertyChange(EdsPropertyEvent inEvent, EdsPropertyID inPropertyID, EdsUInt32 inParam)
{
   // support for custom functions: passing param in the property ID
   unsigned int uiCombinedPropertyId = inPropertyID;
   if (inPropertyID == kEdsPropID_CFn)
      uiCombinedPropertyId |= (inParam << 16);

   try
   {
      PropertyAccess pa(m_hCamera);
      Variant value = pa.Get(inPropertyID, inParam);

      LOG_TRACE(
         inEvent == kEdsPropertyEvent_PropertyChanged ?
            _T("Property [%s] changed to value [%s]\n") :
            _T("Property description changed for property [%s]\n"),
         PropertyAccess::NameFromId(uiCombinedPropertyId),
         PropertyAccess::DisplayTextFromIdAndValue(uiCombinedPropertyId, value).GetString());
   }
   catch (...)
   {
   }

   try
   {
      RemoteReleaseControl::T_enPropertyEvent enPropertyEvent =
         inEvent == kEdsPropertyEvent_PropertyChanged ?
            RemoteReleaseControl::propEventPropertyChanged :
            RemoteReleaseControl::propEventPropertyDescChanged;

      m_subjectPropertyEvent.Call(enPropertyEvent, uiCombinedPropertyId);
   }
   catch (...)
   {
   }
}

EdsError RemoteReleaseControlImpl::OnStateChange_(EdsStateEvent inEvent, EdsUInt32 inEventData, EdsVoid *inContext)
{
   LOG_TRACE(_T("OnStateChange(event = %04x (%s), eventData = %08x, ctx) called\n"),
      inEvent,
      inEvent == kEdsStateEvent_Shutdown ? _T("Shutdown") :
         inEvent == kEdsStateEvent_JobStatusChanged ? _T("JobStatusChanged") :
         inEvent == kEdsStateEvent_WillSoonShutDown ? _T("WillSoonShutDown") :
         inEvent == kEdsStateEvent_ShutDownTimerUpdate ? _T("ShutDownTimerUpdate") :
         inEvent == kEdsStateEvent_CaptureError ? _T("CaptureError") :
         inEvent == kEdsStateEvent_InternalError ? _T("InternalError") :
         inEvent == kEdsStateEvent_AfResult ? _T("AfResult") :
         inEvent == kEdsStateEvent_BulbExposureTime ? _T("BulbExposureTime") : _T("???"),
      inEventData);

   RemoteReleaseControlImpl* prrc = reinterpret_cast<RemoteReleaseControlImpl*>(inContext);

   prrc->OnStateChange(inEvent, inEventData);

   LOG_TRACE(_T("OnStateChange finished\n"));

   return EDS_ERR_OK;
}

void RemoteReleaseControlImpl::OnStateChange(EdsStateEvent inEvent, EdsUInt32 inEventData)
{
   RemoteReleaseControl::T_enStateEvent enStateEvent = RemoteReleaseControl::stateEventInvalid;

   // map from EDSDK states to CanonControl states
   switch(inEvent)
   {
   case kEdsStateEvent_Shutdown:       enStateEvent = RemoteReleaseControl::stateEventCameraShutdown; break;
   case kEdsStateEvent_CaptureError:   enStateEvent = RemoteReleaseControl::stateEventReleaseError; break;
   case kEdsStateEvent_BulbExposureTime: enStateEvent = RemoteReleaseControl::stateEventBulbExposureTime; break;
   case kEdsStateEvent_InternalError:  enStateEvent = RemoteReleaseControl::stateEventInternalError; break;
   }

   try
   {
      if (enStateEvent != RemoteReleaseControl::stateEventInvalid)
         m_subjectStateEvent.Call(enStateEvent, inEventData);
   }
   catch (const CameraException& ex)
   {
      LOG_TRACE(_T("CameraException during StateEvent handler: %s\n"), ex.Message().GetString());
   }
   catch (...)
   {
      LOG_TRACE(_T("unknown exception during StateEvent handler\n"));
   }

   if (inEvent == kEdsStateEvent_WillSoonShutDown)
   {
      // keep the camera awake
      EdsError err = EdsSendCommand(m_hCamera.Get(), kEdsCameraCommand_ExtendShutDownTimer, 0);
      LOG_TRACE(_T("EdsSendCommand(%08x, ExtendShutDownTimer, 0) returned %08x\n"), m_hCamera.Get(), err);
   }
}

EdsError RemoteReleaseControlImpl::OnObjectChange_(EdsObjectEvent inEvent, EdsBaseRef inRef, EdsVoid *inContext)
{
   LOG_TRACE(_T("OnObjectChange(event = %04x (%s), inRef = %08x, ctx) called\n"),
      inEvent,
      inEvent == kEdsObjectEvent_VolumeInfoChanged ? _T("VolumeInfoChanged") :
      inEvent == kEdsObjectEvent_VolumeUpdateItems ? _T("VolumeUpdateItems") :
      inEvent == kEdsObjectEvent_FolderUpdateItems ? _T("FolderUpdateItems") :
      inEvent == kEdsObjectEvent_DirItemCreated ? _T("DirItemCreated") :
      inEvent == kEdsObjectEvent_DirItemRemoved ? _T("DirItemRemoved") :
      inEvent == kEdsObjectEvent_DirItemInfoChanged ? _T("DirItemInfoChanged") :
      inEvent == kEdsObjectEvent_DirItemContentChanged ? _T("DirItemContentChanged") :
      inEvent == kEdsObjectEvent_DirItemRequestTransfer ? _T("DirItemRequestTransfer") :
      inEvent == kEdsObjectEvent_DirItemRequestTransferDT ? _T("DirItemRequestTransferDT") :
      inEvent == kEdsObjectEvent_DirItemCancelTransferDT ? _T("DirItemCancelTransferDT") :
      inEvent == kEdsObjectEvent_VolumeAdded ? _T("VolumeAdded") :
      inEvent == kEdsObjectEvent_VolumeRemoved ? _T("VolumeRemoved") : _T("???"),
      inRef);

   RemoteReleaseControlImpl* prrc = reinterpret_cast<RemoteReleaseControlImpl*>(inContext);

   try
   {
      Handle h(inRef, prrc->m_hCamera.GetRef());

      if (inEvent == kEdsObjectEvent_DirItemRequestTransfer)
         prrc->OnReceivedObjectEventRequestTransfer(h);
   }
   catch (const CameraException& ex)
   {
      LOG_TRACE(_T("CameraException during ObjectEvent handler: %s\n"), ex.Message().GetString());
   }
   catch (...)
   {
      LOG_TRACE(_T("unknown exception during ObjectEvent handler\n"));
   }

   LOG_TRACE(_T("OnObjectChange finished\n"));

   return EDS_ERR_OK;
}

bool RemoteReleaseControlImpl::GetCapability(RemoteReleaseControl::T_enRemoteCapability enCapability) const
{
   try
   {
      switch (enCapability)
      {
      case RemoteReleaseControl::capChangeShootingParameter:
         return true; // always possible

      case RemoteReleaseControl::capChangeShootingMode:
         {
            // try to enum shooting modes; when an empty list of modes comes back, changing
            // shooting mode isn't available
            PropertyAccess p(m_hCamera);

            std::vector<Variant> vecValues;
            bool bReadOnly = false;
            p.Enum(kEdsPropID_AEMode, vecValues, bReadOnly);

            return !vecValues.empty() && !bReadOnly;
         }
         break;

      case RemoteReleaseControl::capZoomControl:
         return true;

      case RemoteReleaseControl::capViewfinder:
         {
            PropertyAccess p(m_hCamera);

            std::vector<Variant> vecValues;
            bool bReadOnly = false;
            p.Enum(kEdsPropID_Evf_OutputDevice, vecValues, bReadOnly);

            // if it contains kEdsEvfOutputDevice_PC, viewfinder is possible
            bool bFound = false;
            std::for_each(vecValues.begin(), vecValues.end(), [&](const Variant& v){
               if (v.Get<unsigned int>() == kEdsEvfOutputDevice_PC)
                  bFound = true;
            });

            return bFound && !bReadOnly;
         }

      case RemoteReleaseControl::capReleaseWhileViewfinder:
         return true; // can always release while viewfinder, if we have one at all

      case RemoteReleaseControl::capAFLock:
         return true;

      case RemoteReleaseControl::capBulbMode:
         // we can't find out here if Bulb is supported;
         // enum Tv values instead and check if Bulb appears, set Bulb mode and call StartBulb()
         return true;

      case RemoteReleaseControl::capUILock:
         // generally supported through kEdsCameraStatusCommand_UILock command
         return true;

      default:
         ATLASSERT(false);
         break;
      }
   }
   catch (const CameraException& ex)
   {
      LOG_TRACE(_T("CameraException during GetCapability(): %s\n"), ex.Message().GetString());
   }
   catch (...)
   {
      LOG_TRACE(_T("unknown exception during GetCapability()\n"));
   }

   return false;
}

void RemoteReleaseControlImpl::SetImageProperty(const ImageProperty& imageProperty)
{
   if (imageProperty.Id() == kEdsPropID_Evf_Zoom)
   {
      // also remember value in this class
      m_uiCurrentZoomPos = imageProperty.Value().Get<unsigned int>();
   }

   m_upReleaseThread->GetIoService().post(
      std::bind(&RemoteReleaseControlImpl::SyncSetImageProperty, this, imageProperty));
}

void RemoteReleaseControlImpl::SyncSetImageProperty(const ImageProperty& imageProperty)
{
   LightweightMutex::LockType lock(*m_spMtxLock);

   int iParam = 0;
   unsigned int uiRealImageProperty = imageProperty.Id();

   // support for custom functions: passing param in the property ID
   if ((uiRealImageProperty & 0xFFFF) == kEdsPropID_CFn)
   {
      iParam = static_cast<int>(uiRealImageProperty >> 16);
      uiRealImageProperty &= 0xFFFF;
   }

   PropertyAccess p(m_hCamera);
   try
   {
      p.Set(uiRealImageProperty, imageProperty.Value(), iParam);
   }
   catch (const CameraException& ex)
   {
      LOG_TRACE(_T("CameraException during SyncSetImageProperty(): %s, ImageProperty: %s, Value: %s\n"),
         ex.Message().GetString(),
         imageProperty.Name().GetString(),
         imageProperty.AsString().GetString());
   }

   // it seems for the Zoom property no notification is sent
   if (imageProperty.Id() == kEdsPropID_Evf_Zoom)
      OnPropertyChange(kEdsPropertyEvent_PropertyChanged, kEdsPropID_Evf_Zoom, 0);
}

std::shared_ptr<Viewfinder> RemoteReleaseControlImpl::StartViewfinder() const
{
   return std::shared_ptr<Viewfinder>(
      new ViewfinderImpl(m_hCamera, m_upReleaseThread->GetIoService(), m_spMtxLock));
}

void RemoteReleaseControlImpl::Release()
{
   m_upReleaseThread->Post(std::bind(&RemoteReleaseControlImpl::AsyncRelease, this));
}

void RemoteReleaseControlImpl::SetSaveToFlag(ShutterReleaseSettings::T_enSaveTarget enSaveTarget, bool bAsynchronous)
{
   Variant value;
   value.Set<unsigned int>(enSaveTarget);
   value.SetType(Variant::typeUInt32);

   ImageProperty ip(variantEdsdk, kEdsPropID_SaveTo, value, true);

   if (bAsynchronous)
      SetImageProperty(ip);
   else
      SyncSetImageProperty(ip);
}

void RemoteReleaseControlImpl::SetCapacity()
{
   // assume a max. image size of 100 MB
   EdsCapacity capacity = { 0 };
   capacity.bytesPerSector = 512;
   capacity.numberOfFreeClusters = 100 * 1000 * 2;
   capacity.reset = true;

   EdsError err = EdsSetCapacity(m_hCamera.Get(), capacity);
   LOG_TRACE(_T("EdsSetCapacity(%08x, Capacity) returned %08x\n"), m_hCamera.Get(), err);
   EDSDK::CheckError(_T("EdsSetCapacity"), err, __FILE__, __LINE__);
}

void RemoteReleaseControlImpl::AsyncRelease()
{
   m_evtShutterReleaseOccured.Reset();

   // set new settings
   ShutterReleaseSettings::T_enSaveTarget saveTarget;
   {
      LightweightMutex::LockType lock(m_mtxShutterReleaseSettings);
      saveTarget = m_shutterReleaseSettings.SaveTarget();
   }

   SetSaveToFlag(saveTarget, false); // synchronous

   if ((saveTarget & ShutterReleaseSettings::saveToHost) != 0)
      SetCapacity();

   // lock UI
   EdsError errUILock = EdsSendStatusCommand(m_hCamera.Get(), kEdsCameraStatusCommand_UILock, 0);
   LOG_TRACE(_T("EdsSendCommand(%08x, UILock, 0) returned %08x\n"), m_hCamera.Get(), errUILock);

   // send command
   EdsError errTakePicture = EdsSendCommand(m_hCamera.Get(), kEdsCameraCommand_TakePicture, 0);
   LOG_TRACE(_T("EdsSendCommand(%08x, TakePicture, 0) returned %08x\n"), m_hCamera.Get(), errTakePicture);

   // unlock UI
   errUILock = EdsSendStatusCommand(m_hCamera.Get(), kEdsCameraStatusCommand_UIUnLock, 0);
   LOG_TRACE(_T("EdsSendCommand(%08x, UIUnLock, 0) returned %08x\n"), m_hCamera.Get(), errUILock);

   // no error checking with EDSDK::CheckError(), since we're on the background thread
   if (errTakePicture != EDS_ERR_OK)
      m_subjectStateEvent.Call(RemoteReleaseControl::stateEventReleaseError, errTakePicture);
}

void RemoteReleaseControlImpl::OnReceivedObjectEventRequestTransfer(Handle hDirectoryItem)
{
   // received an event request; put current shutter release settings into queue
   ShutterReleaseSettings settings;
   {
      LightweightMutex::LockType lock(m_mtxShutterReleaseSettings);
      settings = m_shutterReleaseSettings;
   }

   // only save to camera? then return now
   if (settings.SaveTarget() == ShutterReleaseSettings::saveToCamera)
   {
      EdsError err = EdsDownloadCancel(hDirectoryItem);
      LOG_TRACE(_T("EdsDownloadCancel(dirItem = %08x) returned %08x\n"), hDirectoryItem.Get(), err);
      return;
   }

   // download image
   m_upReleaseThread->Post(std::bind(&RemoteReleaseControlImpl::AsyncDownloadImage, this, hDirectoryItem, settings));
}

void RemoteReleaseControlImpl::AsyncDownloadImage(Handle hDirectoryItem, ShutterReleaseSettings& settings)
{
   DownloadImage(hDirectoryItem, settings);

   // call finished handler
   ShutterReleaseSettings::T_fnOnFinishedTransfer fnHandler = settings.HandlerOnFinishedTransfer();
   if (fnHandler != nullptr)
      fnHandler(settings);
}

void RemoteReleaseControlImpl::DownloadImage(Handle hDirectoryItem, ShutterReleaseSettings& settings)
{
   m_subjectDownloadEvent.Call(RemoteReleaseControl::downloadEventStarted, 0);

   try
   {
      // get directory item information
      EdsDirectoryItemInfo dirItemInfo = {0};
      EdsError err = EdsGetDirectoryItemInfo(hDirectoryItem, &dirItemInfo);

      LOG_TRACE(_T("EdsGetDirectoryItemInfo(dirItem = %08x, &dirItemInfo) returned %08x\n"), hDirectoryItem.Get(), err);
      EDSDK::CheckError(_T("EdsGetDirectoryItemInfo"), err, __FILE__, __LINE__);

      // camera tells us name of file; add to output folder here
      CString cszFilename = Path::Combine(
         Path(settings.Filename()).FolderName(),
         dirItemInfo.szFileName);

      settings.Filename(cszFilename);

      // create file stream for transfer destination
      Handle hStream(hDirectoryItem.GetRef());

      err = EdsCreateFileStreamEx(cszFilename,
         kEdsFileCreateDisposition_CreateAlways,
         kEdsAccess_ReadWrite, &hStream);
      LOG_TRACE(_T("EdsCreateFileStreamEx(\"%s\", CreateAlways, ReadWrite, &stream = %08x) returned %08x\n"),
         cszFilename.GetString(),
         hStream.Get(), err);
      EDSDK::CheckError(_T("EdsCreateFileStreamEx"), err, __FILE__, __LINE__);

      // request progress messages
      err = EdsSetProgressCallback(hStream, ProgressFunc, kEdsProgressOption_Periodically, this);
      LOG_TRACE(_T("EdsSetProgressCallback(stream = %08x, ProgressFunc, kEdsProgressOption_Periodically) returned %08x\n"),
         hStream.Get(), err);
      EDSDK::CheckError(_T("EdsSetProgressCallback"), err, __FILE__, __LINE__);

      // download image
      err = EdsDownload(hDirectoryItem, dirItemInfo.size, hStream);
      LOG_TRACE(_T("EdsDownload(dirItem = %08x, size=%I64u, stream = %08x) returned %08x\n"),
         hDirectoryItem.Get(), dirItemInfo.size, hStream.Get(), err);
      EDSDK::CheckError(_T("EdsDownload"), err, __FILE__, __LINE__);
   }
   catch (const CameraException& ex)
   {
      LOG_TRACE(_T("Exception while downloading image: %s"), ex.Message().GetString());

      // issue notification that download has been canceled
      EdsError err = EdsDownloadCancel(hDirectoryItem);
      LOG_TRACE(_T("EdsDownloadCancel(dirItem = %08x) returned %08x\n"), hDirectoryItem.Get(), err);

      throw;
   }

   // issue notification that download is complete
   EdsError err = EdsDownloadComplete(hDirectoryItem);
   LOG_TRACE(_T("EdsDownloadComplete(dirItem = %08x) returned %08x\n"), hDirectoryItem.Get(), err);
   EDSDK::CheckError(_T("EdsDownloadComplete"), err, __FILE__, __LINE__);

   m_subjectDownloadEvent.Call(RemoteReleaseControl::downloadEventFinished, 0);
}

EdsError RemoteReleaseControlImpl::ProgressFunc(EdsUInt32 inPercent, EdsVoid* inContext, EdsBool* /*outCancel*/)
{
   LOG_TRACE(_T("Image Download Progress: Percent = %u%%\n"), inPercent);

   RemoteReleaseControlImpl* prrc = reinterpret_cast<RemoteReleaseControlImpl*>(inContext);

   prrc->m_subjectDownloadEvent.Call(RemoteReleaseControl::downloadEventInProgress, inPercent);

   return EDS_ERR_OK;
}
