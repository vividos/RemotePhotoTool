//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file EdsdkCommon.hpp EDSDK - Common functions
//
#pragma once

// includes
#include "edsdk.h"
#include "Instance.hpp"
#include "CameraException.hpp"
#include "BackgroundWindowThread.hpp"
#include "LightweightMutex.hpp"
#include "ErrorText.hpp"

/// EOS Digital Camera SDK interface
namespace EDSDK
{

/// checks for errors
extern void CheckError(const CString& cszFunction, EdsError err, LPCSTR pszFile, UINT uiLine);

/// SDK instance
class SDKInstance
{
public:
   /// ctor
   SDKInstance();

   /// dtor
   ~SDKInstance() throw();
};

#define USE_BACKGROUND_THREAD

/// SDK reference
class Ref: public std::enable_shared_from_this<Ref>
{
public:
   /// ctor
   Ref()
#ifdef USE_BACKGROUND_THREAD
      :m_spBackgroundThread(new BackgroundWindowThread<SDKInstance>)
#endif
   {
#ifdef USE_BACKGROUND_THREAD
      m_spBackgroundThread->Start();
#endif
   }
   /// dtor
   ~Ref() throw()
   {
   }

   /// adds EDSDK version text
   void AddVersionText(CString& cszVersionText) const;

   /// enumerates cameras
   void EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& vecSourceDevices) const;

   /// starts waiting for camera
   void AsyncWaitForCamera(bool bStart, std::function<void()> fnOnCameraConnected = std::function<void()>());

   /// called to do idle processing
   static void OnIdle();

private:
   /// handler called when camera was added
   static EdsError EDSCALLBACK OnCameraAddedHandler(EdsVoid* inContext);

private:
#ifdef USE_BACKGROUND_THREAD
   /// background thread for SDK instance
   std::shared_ptr<BackgroundWindowThread<SDKInstance>> m_spBackgroundThread;
#else
   /// SDK instance
   SDKInstance m_instance;
#endif

   /// mutex to protect m_fnOnCameraConnected
   LightweightMutex m_mtxFnOnCameraConnected;

   /// handler to call when camera is connected
   std::function<void()> m_fnOnCameraConnected;
};

/// smart pointer to SDK reference
typedef std::shared_ptr<Ref> RefSp;

/// handle to EdsBaseRef object (EdsCameraListRef, EdsCameraRef, etc.)
class Handle
{
public:
   // default ctor
   Handle()
      :m_spRef(RefSp()),
       m_objRef(0)
   {
   }

   /// ctor; takes SDK reference
   Handle(RefSp spRef)
      :m_spRef(spRef),
       m_objRef(0)
   {
   }

   /// ctor; takes object reference and SDK reference
   Handle(EdsBaseRef objRef, RefSp spRef)
      :m_spRef(spRef),
       m_objRef(objRef)
   {
      // we're starting with a objRef ref count of 1, so no need to call EdsRetain here
   }

   /// copy ctor
   Handle(const Handle& h)
   {
      Retain(h);

      m_spRef = h.m_spRef;
      m_objRef = h.m_objRef;
   }

   /// assign operator
   Handle& operator=(const Handle& h)
   {
      if (this == &h)
         return *this;

      Retain(h);

      m_spRef = h.m_spRef;
      m_objRef = h.m_objRef;

      return *this;
   }

   /// dtor
   ~Handle() throw()
   {
      try
      {
         Release();
      }
      catch(...)
      {
      }
   }

   /// returns pointer to base ref; non-const version
   EdsBaseRef* operator&() throw()
   {
      return &m_objRef;
   }

   /// returns base ref
   operator EdsBaseRef() throw()
   {
      return m_objRef;
   }

   /// returns base ref; const version
   operator EdsBaseRef() const throw()
   {
      return m_objRef;
   }

   /// returns pointer to base ref
   operator EdsBaseRef*() throw()
   {
      return &m_objRef;
   }

   /// returns base ref
   EdsBaseRef Get() throw()
   {
      return m_objRef;
   }

   /// returns base ref; const version
   EdsBaseRef Get() const throw()
   {
      return m_objRef;
   }

   /// returns SDK reference
   RefSp GetRef() throw() { return m_spRef; }

private:
   /// increases reference count of object ref in h
   void Retain(const Handle& h)
   {
      EdsUInt32 uiCount = EdsRetain(h.m_objRef);
      //LOG_TRACE(_T("EdsRetain(%08x) returned %i\n"), h.m_objRef, uiCount);
      if (uiCount == 0xFFFFFFFF)
      {
         throw CameraException(_T("EdsRetain"), "Internal error",
            EDS_CMP_ID_CLIENT_COMPONENTID | EDS_ERR_INTERNAL_ERROR, __FILE__, __LINE__);
      }
   }

   /// decreases reference count of object ref in h
   void Release()
   {
      if (m_objRef == 0)
         return;

      EdsUInt32 uiCount = EdsRelease(m_objRef);
      //LOG_TRACE(_T("EdsRelease(%08x) returned %i\n"), m_objRef, uiCount);
      if (uiCount == 0xFFFFFFFF)
      {
         throw CameraException(_T("EdsRelease"), "Internal error",
            EDS_CMP_ID_CLIENT_COMPONENTID | EDS_ERR_INTERNAL_ERROR, __FILE__, __LINE__);
      }
   }

private:
   /// SDK ref
   RefSp m_spRef;

   /// object ref
   EdsBaseRef m_objRef;
};

/// waits for message and does window and EDSDK processing while waiting
void MsgWaitForEvent(Event& evt) throw();

} // namespace EDSDK
