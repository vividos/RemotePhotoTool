//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file Instance.cpp Canon control - Instance class
//

// includes
#include "stdafx.h"
#include "Instance.hpp"
#include "EDSDK/EdsdkCommon.hpp"
#include "CDSDK/CdsdkCommon.hpp"
#include "CDSDK/CdsdkSourceInfoImpl.hpp"
#include "PSREC/PsrecCommon.hpp"
#include "gPhoto2/GPhoto2Common.hpp"
#include "WIA/WiaCommon.hpp"
#include <ulib/thread/LightweightMutex.hpp>
#include "BackgroundWorkerThread.hpp"
#include "BackgroundTimer.hpp"
#include "SdkReferenceBase.hpp"


// Instance::Impl

/// instance implementation
class Instance::Impl
{
public:
   /// ctor
   Impl();

   /// returns current impl
   static std::shared_ptr<Impl> Get();

   /// starts camera polling
   void StartPollCamera();

   /// stops camera polling
   void StopPollCamera();

private:
   /// timer handler to poll camera list
   void PollCamera();

protected:
   friend Instance;

   /// critical section for m_fnOnCameraAdded
   LightweightMutex m_mtxFnOnCameraAdded;

   /// function to call when a new camera was added
   T_fnOnCameraAdded m_fnOnCameraAdded;

   /// list of all available SDK references
   std::vector<std::shared_ptr<SdkReferenceBase>> m_allSdkReferences;

private:
   /// current instance
   static std::weak_ptr<Impl> m_wpInstance;

   /// current number of devices used in polling
   size_t m_uiPollNumDevices;

   /// worker thread for timer
   std::shared_ptr<BackgroundWorkerThread> m_spWorkerThread;

   /// thread that polls camera enumeration
   std::shared_ptr<BackgroundTimer> m_spCameraPollTimer;
};


// static members

std::weak_ptr<Instance::Impl> Instance::Impl::m_wpInstance;

std::shared_ptr<Instance::Impl> Instance::Impl::Get()
{
   std::shared_ptr<Instance::Impl> spImpl = m_wpInstance.lock();
   if (spImpl == nullptr)
      m_wpInstance = spImpl = std::make_shared<Instance::Impl>();

   return spImpl;
}

Instance::Impl::Impl()
   :m_uiPollNumDevices(0)
{
   m_allSdkReferences.push_back(std::make_shared<EDSDK::Ref>());
   m_allSdkReferences.push_back(std::make_shared<PSREC::Ref>());
   //m_allSdkReferences.push_back(std::make_shared<GPhoto2::Ref>());
   m_allSdkReferences.push_back(std::make_shared<CDSDK::Ref>());
   m_allSdkReferences.push_back(std::make_shared<WIA::Ref>());
}

void Instance::Impl::StartPollCamera()
{
   // get current number of source devices
   {
      std::vector<std::shared_ptr<SourceInfo>> sourceDevicesList;

      for (auto spRef : m_allSdkReferences)
      {
         if (!spRef->IsAsyncWaitPossible())
            spRef->EnumerateDevices(sourceDevicesList);
      }

      m_uiPollNumDevices = sourceDevicesList.size();
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
   std::vector<std::shared_ptr<SourceInfo>> sourceDevicesList;

   for (auto spRef : m_allSdkReferences)
   {
      if (!spRef->IsAsyncWaitPossible())
         spRef->EnumerateDevices(sourceDevicesList);
   }

   size_t uiPollNumDevices = sourceDevicesList.size();

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

Instance::~Instance()
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

Instance& Instance::operator=(const Instance& instance)
{
   if (&instance != this)
      m_spImpl = instance.m_spImpl;

   return *this;
}

Instance Instance::Get()
{
   return Instance(Impl::Get());
}

CString Instance::Version() const
{
   CString versionText;
   for (auto spRef : m_spImpl->m_allSdkReferences)
      spRef->AddVersionText(versionText);

   return versionText;
}

void Instance::EnableLogging(bool bEnable, const CString& cszLogfilePath)
{
   LogConfigure(bEnable, cszLogfilePath);
}

void Instance::OnCameraAddedHandler()
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
      for (auto spRef : m_spImpl->m_allSdkReferences)
      {
         if (spRef->IsAsyncWaitPossible())
            spRef->AsyncWaitForCamera(true, std::bind(&Instance::OnCameraAddedHandler, this));
      }

      m_spImpl->StartPollCamera();
   }
   else
   {
      for (auto spRef : m_spImpl->m_allSdkReferences)
      {
         if (spRef->IsAsyncWaitPossible())
            spRef->AsyncWaitForCamera(false);
      }

      m_spImpl->StopPollCamera();
   }
}

void Instance::EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& sourceDevicesList) const
{
   for (auto spRef : m_spImpl->m_allSdkReferences)
   {
      try
      {
         spRef->EnumerateDevices(sourceDevicesList);
      }
      catch (...)
      {
      }
   }
}

void Instance::OnIdle()
{
   EDSDK::Ref::OnIdle();
}
