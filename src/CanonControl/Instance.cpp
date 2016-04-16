//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file Instance.cpp Canon control - Instance class
//

// includes
#include "stdafx.h"
#include "Instance.hpp"
#include "EdsdkCommon.hpp"
#include "CdsdkCommon.hpp"
#include "CdsdkSourceInfoImpl.hpp"
#include "PsrecCommon.hpp"
#include "GPhoto2Common.hpp"
#include "LightweightMutex.hpp"
#include "BackgroundWorkerThread.hpp"
#include "BackgroundTimer.hpp"
#include "Wtl.hpp" // for RunTimeHelper


// Instance::Impl

/// instance implementation
class Instance::Impl
{
public:
   /// ctor
   Impl(EDSDK::RefSp spEdSdkRef,
      CDSDK::RefSp spCdSdkRef,
      PSREC::RefSp spPsRecRef,
      GPhoto2::RefSp spGPhoto2Ref)
      :m_spEdSdkRef(spEdSdkRef),
       m_spCdSdkRef(spCdSdkRef),
       m_spPsRecRef(spPsRecRef),
       m_spGPhoto2Ref(spGPhoto2Ref),
       m_uiPollNumDevices(0)
   {
   }

   static std::weak_ptr<EDSDK::Ref> m_wpEdSdkRef; ///< weak ref to EDSDK
   static std::weak_ptr<CDSDK::Ref> m_wpCdSdkRef; ///< weak ref to CDSDK
   static std::weak_ptr<PSREC::Ref> m_wpPsRecRef; ///< weak ref to PSREC
   static std::weak_ptr<GPhoto2::Ref> m_wpGPhoto2Ref; ///< weak ref to gPhoto2

   EDSDK::RefSp m_spEdSdkRef; ///< EDSDK reference
   CDSDK::RefSp m_spCdSdkRef; ///< CDSDK reference
   PSREC::RefSp m_spPsRecRef; ///< PSREC reference
   GPhoto2::RefSp m_spGPhoto2Ref; ///< gPhoto2 reference

   /// returns current impl
   static std::shared_ptr<Impl> Get();

   /// starts camera polling
   void StartPollCamera();

   /// stops camera polling
   void StopPollCamera();

private:
   /// timer handler to poll camera list
   void PollCamera();

public:
   /// critical section for m_fnOnCameraAdded
   LightweightMutex m_mtxFnOnCameraAdded;

   /// function to call when a new camera was added
   T_fnOnCameraAdded m_fnOnCameraAdded;

private:
   /// current number of devices used in polling
   size_t m_uiPollNumDevices;

   /// worker thread for timer
   std::shared_ptr<BackgroundWorkerThread> m_spWorkerThread;

   /// thread that polls camera enumeration
   std::shared_ptr<BackgroundTimer> m_spCameraPollTimer;
};


// static members

std::weak_ptr<EDSDK::Ref> Instance::Impl::m_wpEdSdkRef;
std::weak_ptr<CDSDK::Ref> Instance::Impl::m_wpCdSdkRef;
std::weak_ptr<PSREC::Ref> Instance::Impl::m_wpPsRecRef;
std::weak_ptr<GPhoto2::Ref> Instance::Impl::m_wpGPhoto2Ref;


std::shared_ptr<Instance::Impl> Instance::Impl::Get()
{
   EDSDK::RefSp spEdSdkRef = m_wpEdSdkRef.lock();
   if (spEdSdkRef == nullptr)
      m_wpEdSdkRef = spEdSdkRef = EDSDK::RefSp(new EDSDK::Ref);

   CDSDK::RefSp spCdSdkRef = m_wpCdSdkRef.lock();
   if (spCdSdkRef == nullptr)
      m_wpCdSdkRef = spCdSdkRef = CDSDK::RefSp(new CDSDK::Ref);

   PSREC::RefSp spPsRecRef = m_wpPsRecRef.lock();
   if (spPsRecRef == nullptr)
      m_wpPsRecRef = spPsRecRef = PSREC::RefSp(new PSREC::Ref);

   GPhoto2::RefSp spGPhoto2Ref = m_wpGPhoto2Ref.lock();
   if (spGPhoto2Ref == nullptr)
      m_wpGPhoto2Ref = spGPhoto2Ref = GPhoto2::RefSp(new GPhoto2::Ref);

   return std::make_shared<Instance::Impl>(spEdSdkRef, spCdSdkRef, spPsRecRef, spGPhoto2Ref);
}

void Instance::Impl::StartPollCamera()
{
   // get current number of source devices
   {
      std::vector<std::shared_ptr<SourceInfo>> vecSourceDevices;

      if (!RunTimeHelper::IsVista())
         m_spCdSdkRef->EnumerateDevices(vecSourceDevices);

      m_spPsRecRef->EnumerateDevices(vecSourceDevices);
      //m_spGPhoto2Ref->EnumerateDevices(vecSourceDevices);

      m_uiPollNumDevices = vecSourceDevices.size();
   }

   // start thread that polls for new cameras on CDSDK, PSREC
   m_spWorkerThread.reset(new BackgroundWorkerThread);

   m_spCameraPollTimer.reset(
      new BackgroundTimer(
         m_spWorkerThread->GetIoService(),
         1000,
         std::bind(&Instance::Impl::PollCamera, this)
         ));

   m_spCameraPollTimer->Start();
}

void Instance::Impl::StopPollCamera()
{
   if (m_spCameraPollTimer != nullptr)
      m_spCameraPollTimer->Stop();

   m_spCameraPollTimer.reset();
   m_spWorkerThread.reset();
}

void Instance::Impl::PollCamera()
{
   std::vector<std::shared_ptr<SourceInfo>> vecSourceDevices;

   // Note: CDSDK shows wait cursor for a small time, so only enumerate on XP or lower
   // since the camera would't work on Vista and above anyway.
   if (!RunTimeHelper::IsVista())
      m_spCdSdkRef->EnumerateDevices(vecSourceDevices);

   m_spPsRecRef->EnumerateDevices(vecSourceDevices);
   //m_spGPhoto2Ref->EnumerateDevices(vecSourceDevices);

   size_t uiPollNumDevices = vecSourceDevices.size();

   if (uiPollNumDevices != m_uiPollNumDevices)
   {
      LightweightMutex::LockType lock(m_mtxFnOnCameraAdded);
      if (m_fnOnCameraAdded != nullptr)
         m_fnOnCameraAdded();
   }

   m_uiPollNumDevices = uiPollNumDevices;
}


// Instance

Instance::Instance(std::shared_ptr<Impl> spImpl)
:m_spImpl(spImpl)
{
}

Instance::Instance(const Instance& instance)
:m_spImpl(instance.m_spImpl)
{
}

Instance::~Instance() throw()
{
   try
   {
      // deregister camera handler
      AsyncWaitForCamera();
   }
   catch (...)
   {
   }
}

Instance Instance::Get()
{
   return Instance(Impl::Get());
}

CString Instance::Version() const
{
   CString cszVersionText;
   m_spImpl->m_spEdSdkRef->AddVersionText(cszVersionText);
   m_spImpl->m_spPsRecRef->AddVersionText(cszVersionText);
   m_spImpl->m_spCdSdkRef->AddVersionText(cszVersionText);
   m_spImpl->m_spGPhoto2Ref->AddVersionText(cszVersionText);

   return cszVersionText;
}

void Instance::EnableLogging(bool bEnable, const CString& cszLogfilePath) throw()
{
   LogConfigure(bEnable, cszLogfilePath);
}

void Instance::OnCameraAddedHandler() throw()
{
   LightweightMutex::LockType lock(m_spImpl->m_mtxFnOnCameraAdded);

   Instance::T_fnOnCameraAdded fnOnCameraAdded = m_spImpl->m_fnOnCameraAdded;

   if (fnOnCameraAdded != nullptr)
   try
   {
      fnOnCameraAdded();
   }
   catch (...)
   {
   }
}

void Instance::AsyncWaitForCamera(T_fnOnCameraAdded fnOnCameraAdded)
{
   {
      LightweightMutex::LockType lock(m_spImpl->m_mtxFnOnCameraAdded);
      m_spImpl->m_fnOnCameraAdded = fnOnCameraAdded;
   }

   if (fnOnCameraAdded != nullptr)
   {
      m_spImpl->m_spEdSdkRef->AsyncWaitForCamera(true, std::bind(&Instance::OnCameraAddedHandler, this));

      m_spImpl->StartPollCamera();
   }
   else
   {
      m_spImpl->m_spEdSdkRef->AsyncWaitForCamera(false);

      m_spImpl->StopPollCamera();
   }
}

void Instance::EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& vecSourceDevices) const
{
   try
   {
      if (!RunTimeHelper::IsVista())
         m_spImpl->m_spCdSdkRef->EnumerateDevices(vecSourceDevices);
   }
   catch (...)
   {
   }

   try
   {
      m_spImpl->m_spEdSdkRef->EnumerateDevices(vecSourceDevices);
   }
   catch (...)
   {
   }

   try
   {
      m_spImpl->m_spPsRecRef->EnumerateDevices(vecSourceDevices);
   }
   catch (...)
   {
   }

   try
   {
      //m_spImpl->m_spGPhoto2Ref->EnumerateDevices(vecSourceDevices);
   }
   catch (...)
   {
   }
}

void Instance::OnIdle()
{
   EDSDK::Ref::OnIdle();
}
