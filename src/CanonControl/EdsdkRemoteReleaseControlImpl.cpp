//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file EdsdkRemoteReleaseControlImpl.cpp EDSDK - RemoteReleaseControl impl
//
#pragma once

// includes
#include "stdafx.h"
#include "EdsdkRemoteReleaseControlImpl.hpp"
#include "ShutterReleaseSettings.hpp"
#include "Filesystem.hpp"
#include "Thread.hpp"
#include "Asio.hpp"
#include <atomic>

class EDSDK::AsyncReleaseControlThread
{
public:
   AsyncReleaseControlThread()
      :m_ioService(1),
       m_upDefaultWork(new boost::asio::io_service::work(m_ioService)),
       m_bFinished(false)
   {
      m_upThread.reset(new std::thread(boost::bind(&AsyncReleaseControlThread::Run, this)));
   }

   ~AsyncReleaseControlThread() throw()
   {
      try
      {
         m_bFinished = true;
         m_upDefaultWork.reset();
         m_ioService.stop();

         m_upThread->join();
      }
      catch(...)
      {
      }
   }

   boost::asio::io_service& GetIoService() throw() { return m_ioService; }

   void Post(boost::function<void()> fn)
   {
      m_ioService.post(fn);
   }

private:
   void Run();

private:
   /// background thread
   std::unique_ptr<std::thread> m_upThread;

   /// io service
   boost::asio::io_service m_ioService;

   /// default work
   std::unique_ptr<boost::asio::io_service::work> m_upDefaultWork;

   std::atomic<bool> m_bFinished;
};

void EDSDK::AsyncReleaseControlThread::Run()
{
   Thread::SetName(_T("AsyncReleaseControlThread"));

   // run message queue
   MSG msg = {0};
   while (!m_bFinished)
   {
      // process window messages
      if (::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
      {
         BOOL bRet = ::GetMessage(&msg, nullptr, 0, 0);
         if(!bRet)
            break;   // WM_QUIT, exit message loop

         LOG_TRACE(_T("HWND=%08x MSG=%04x\n"), msg.hwnd, msg.message);

         ::TranslateMessage(&msg);
         ::DispatchMessage(&msg);
      }
      else
      {
         // process asio handlers
         boost::system::error_code ec;
         if (m_ioService.poll_one(ec) > 0)
            m_ioService.run_one(ec);
      }
   }
}



using namespace EDSDK;

RemoteReleaseControlImpl::RemoteReleaseControlImpl(const Handle& hCamera)
:m_hCamera(hCamera),
 m_upReleaseThread(new AsyncReleaseControlThread),
 m_spMtxLock(new LightweightMutex),
 m_defaultShutterReleaseSettings(ShutterReleaseSettings::saveToCamera), // default is to just save to camera
 m_evtShutterReleaseOccured(true, false) // manual-reset event
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

RemoteReleaseControlImpl::~RemoteReleaseControlImpl() throw()
{
   LOG_TRACE(_T("Start unregistering event handler\n"));

   m_subjectPropertyEvent.Clear();
   m_subjectStateEvent.Clear();

   EdsError err = EdsSetPropertyEventHandler(m_hCamera.Get(), kEdsPropertyEvent_All, nullptr, this);
   LOG_TRACE(_T("EdsSetPropertyEventHandler(ref = %08x, All, nullptr) returned %08x\n"), m_hCamera.Get(), err);

   err = EdsSetCameraStateEventHandler(m_hCamera.Get(), kEdsStateEvent_All, nullptr, this);
   LOG_TRACE(_T("EdsSetCameraStateEventHandler(ref = %08x, All, nullptr) returned %08x\n"), m_hCamera.Get(), err);

   err = EdsSetObjectEventHandler(m_hCamera.Get(), kEdsObjectEvent_All, nullptr, this);
   LOG_TRACE(_T("EdsSetObjectEventHandler(ref = %08x, All, nullptr) returned %08x\n"), m_hCamera.Get(), err);

   LOG_TRACE(_T("Finished unregistering event handler\n"));
}

EdsError RemoteReleaseControlImpl::OnPropertyChange_(
   EdsPropertyEvent inEvent,
   EdsPropertyID inPropertyID,
   EdsUInt32 inParam,
   EdsVoid* inContext)
{
   inParam;
   //LOG_TRACE(_T("OnPropertyChange(event = %u (%s), propId = %08x, param = %u, ctx) called\n"),
   //   inEvent,
   //   inEvent == kEdsPropertyEvent_PropertyChanged ? _T("PropertyChanged") :
   //      inEvent == kEdsPropertyEvent_PropertyDescChanged ? _T("PropertyDescChanged") : _T("???"),
   //   inPropertyID,
   //   inParam);

   RemoteReleaseControlImpl* prrc = reinterpret_cast<RemoteReleaseControlImpl*>(inContext);

   // map special property id to 0; this means that user has to update all known property values and descriptions
   if (inPropertyID == 0x0000FFFF)
      inPropertyID = 0;

   prrc->OnPropertyChange(inEvent, inPropertyID);

   return EDS_ERR_OK;
}

void RemoteReleaseControlImpl::OnPropertyChange(EdsPropertyEvent inEvent, EdsPropertyID inPropertyID) throw()
{
   try
   {
      PropertyAccess pa(m_hCamera);
      Variant value = pa.Get(inPropertyID);

      LOG_TRACE(
         inEvent == kEdsPropertyEvent_PropertyChanged ?
            _T("Property [%s] changed to value [%s]\n") :
            _T("Property description changed for property [%s]\n"),
         PropertyAccess::NameFromId(inPropertyID),
         PropertyAccess::DisplayTextFromIdAndValue(inPropertyID, value));
   }
   catch(...)
   {
   }

   try
   {
      RemoteReleaseControl::T_enPropertyEvent enPropertyEvent =
         inEvent == kEdsPropertyEvent_PropertyChanged ?
            RemoteReleaseControl::propEventPropertyChanged :
            RemoteReleaseControl::propEventPropertyDescChanged;

      m_subjectPropertyEvent.Call(enPropertyEvent, inPropertyID);
   }
   catch(...)
   {
   }
}

EdsError RemoteReleaseControlImpl::OnStateChange_(EdsStateEvent inEvent, EdsUInt32 inEventData, EdsVoid *inContext)
{
   LOG_TRACE(_T("OnStateChange(event = %u (%s), eventData = %08x, ctx) called\n"),
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

void RemoteReleaseControlImpl::OnStateChange(EdsStateEvent inEvent, EdsUInt32 inEventData) throw()
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
   catch(CameraException& ex)
   {
      LOG_TRACE(_T("CameraException during StateEvent handler: %s\n"), ex.Message());
   }
   catch(...)
   {
   }
}

EdsError RemoteReleaseControlImpl::OnObjectChange_(EdsObjectEvent inEvent, EdsBaseRef inRef, EdsVoid *inContext)
{
   LOG_TRACE(_T("OnObjectChange(event = %u (%s), inRef = %08x, ctx) called\n"),
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
   catch(CameraException& ex)
   {
      LOG_TRACE(_T("CameraException during ObjectEvent handler: %s\n"), ex.Message());
   }
   catch(...)
   {
   }

   LOG_TRACE(_T("OnObjectChange finished\n"));

   return EDS_ERR_OK;
}

bool RemoteReleaseControlImpl::GetCapability(RemoteReleaseControl::T_enRemoteCapability enCapability) const throw()
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
         return false; // zoom isn't supported with ED-SDK // TODO really?

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
         return false; // TODO really?
         break;

      default:
         ATLASSERT(false);
         break;
      }
   }
   catch(const CameraException& ex)
   {
      LOG_TRACE(_T("CameraException during GetCapability(): %s\n"), ex.Message());
   }

   return false;
}

void RemoteReleaseControlImpl::SetImageProperty(const ImageProperty& imageProperty)
{
   m_upReleaseThread->GetIoService().post(
      boost::bind(&RemoteReleaseControlImpl::AsyncSetImageProperty, this, imageProperty));
}

void RemoteReleaseControlImpl::AsyncSetImageProperty(const ImageProperty& imageProperty)
{
   LightweightMutex::LockType lock(*m_spMtxLock);

   PropertyAccess p(m_hCamera);
   try
   {
      p.Set(imageProperty.Id(), imageProperty.Value());
   }
   catch(CameraException& ex)
   {
      LOG_TRACE(_T("CameraException during AsyncSetImageProperty(): %s, ImageProperty: %s, Value: %s\n"),
         ex.Message(), imageProperty.Name().GetString(), imageProperty.AsString().GetString());
   }
}

std::shared_ptr<Viewfinder> RemoteReleaseControlImpl::StartViewfinder() const
{
   return std::shared_ptr<Viewfinder>(
      new ViewfinderImpl(m_hCamera, m_upReleaseThread->GetIoService(), m_spMtxLock));
}

void RemoteReleaseControlImpl::Release(const ShutterReleaseSettings& settings)
{
   m_upReleaseThread->Post(boost::bind(&RemoteReleaseControlImpl::AsyncRelease, this, settings));
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
      AsyncSetImageProperty(ip);
}

void RemoteReleaseControlImpl::AsyncRelease(const ShutterReleaseSettings& settings)
{
   m_evtShutterReleaseOccured.Reset();

   // set new settings
   ShutterReleaseSettings::T_enSaveTarget defaultSaveTarget;
   {
      LightweightMutex::LockType lock(m_mtxCurrentShutterReleaseSettings);
      m_currentShutterReleaseSettings = settings;
      defaultSaveTarget = m_defaultShutterReleaseSettings.SaveTarget();
   }

   SetSaveToFlag(settings.SaveTarget(), false); // synchronous

   // send command
   EdsError err = EdsSendCommand(m_hCamera.Get(), kEdsCameraCommand_TakePicture, 0);
   LOG_TRACE(_T("EdsSendCommand(%08x, TakePicture, 0) returned %08x\n"), m_hCamera.Get(), err);
   EDSDK::CheckError(_T("EdsSendCommand"), err, __FILE__, __LINE__);
}

void RemoteReleaseControlImpl::OnReceivedObjectEventRequestTransfer(Handle hDirectoryItem)
{
   // received an event request; put current shutter release settings into queue
   ShutterReleaseSettings settings;

   ShutterReleaseSettings::T_enSaveTarget defaultSaveTarget;
   {
      LightweightMutex::LockType lock(m_mtxCurrentShutterReleaseSettings);
      settings = m_currentShutterReleaseSettings;

      // reset release settings to default
      m_currentShutterReleaseSettings = m_defaultShutterReleaseSettings;
      defaultSaveTarget = m_defaultShutterReleaseSettings.SaveTarget();
   }

   // only save to camera? then return now
   if (settings.SaveTarget() == ShutterReleaseSettings::saveToCamera)
   {
      EdsError err = EdsDownloadCancel(hDirectoryItem);
      LOG_TRACE(_T("EdsDownloadCancel(dirItem = %08x) returned %08x\n"), hDirectoryItem.Get(), err);
      return;
   }

   // download image
   m_upReleaseThread->Post(boost::bind(&RemoteReleaseControlImpl::AsyncDownloadImage, this, hDirectoryItem, settings));
}

void RemoteReleaseControlImpl::AsyncDownloadImage(Handle hDirectoryItem, ShutterReleaseSettings& settings)
{
   DownloadImage(hDirectoryItem, settings);

   // call finished handler
   ShutterReleaseSettings::T_fnOnFinishedTransfer fnHandler = settings.HandlerOnFinishedTransfer();
   if (!fnHandler.empty())
      fnHandler(settings);

   // reset SaveTo flag to default
   {
      ShutterReleaseSettings::T_enSaveTarget defaultSaveTarget;
      {
         LightweightMutex::LockType lock(m_mtxCurrentShutterReleaseSettings);

         // reset release settings to default
         m_currentShutterReleaseSettings = m_defaultShutterReleaseSettings;

         defaultSaveTarget = m_defaultShutterReleaseSettings.SaveTarget();
      }

      SetSaveToFlag(defaultSaveTarget, true); // asynchronous
   }
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
      CString cszFilename = settings.Filename();
      cszFilename = Path_Combine(
         Path_GetDirectoryName(cszFilename),
         dirItemInfo.szFileName);

      settings.Filename(cszFilename);

      // create file stream for transfer destination
      Handle hStream(hDirectoryItem.GetRef());

      USES_CONVERSION;
      err = EdsCreateFileStream(T2CA(cszFilename),
         kEdsFileCreateDisposition_CreateAlways,
         kEdsAccess_ReadWrite, &hStream);
      LOG_TRACE(_T("EdsCreateFileStream(dirItem = %08x, &dirItemInfo) returned %08x\n"), hDirectoryItem.Get(), err);
      EDSDK::CheckError(_T("EdsCreateFileStream"), err, __FILE__, __LINE__);

      // request progress messages
      err = EdsSetProgressCallback(hStream, ProgressFunc, kEdsProgressOption_Periodically, this);
      LOG_TRACE(_T("EdsSetProgressCallback(stream = %08x, ProgressFunc, kEdsProgressOption_Periodically) returned %08x\n"),
         hStream.Get(), err);
      EDSDK::CheckError(_T("EdsSetProgressCallback"), err, __FILE__, __LINE__);

      // download image
      err = EdsDownload(hDirectoryItem, dirItemInfo.size, hStream);
      LOG_TRACE(_T("EdsDownload(dirItem = %08x, size=%u, stream = %08x) returned %08x\n"),
         hDirectoryItem.Get(), dirItemInfo.size, hStream.Get(), err);
      EDSDK::CheckError(_T("EdsDownload"), err, __FILE__, __LINE__);
   }
   catch(CameraException& ex)
   {
      // issue notification that download has been canceled
      EdsError err = EdsDownloadCancel(hDirectoryItem);
      LOG_TRACE(_T("EdsDownloadCancel(dirItem = %08x) returned %08x\n"), hDirectoryItem.Get(), err);
   
      throw ex;
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
