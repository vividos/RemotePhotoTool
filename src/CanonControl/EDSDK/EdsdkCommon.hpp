//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file EdsdkCommon.hpp EDSDK - Common functions
//
#pragma once

// includes
#include "edsdk.h"
#include "Instance.hpp"
#include "CameraException.hpp"
#include <ulib/thread/Event.hpp>
#include <ulib/thread/LightweightMutex.hpp>
#include <ulib/thread/RecursiveMutex.hpp>
#include "ErrorText.hpp"
#include "SdkReferenceBase.hpp"

/// EOS Digital Camera SDK interface
namespace EDSDK
{

/// checks for errors
extern void CheckError(const CString& cszFunction, EdsError err, LPCSTR pszFile, UINT uiLine);

/// SDK reference
class Ref: public SdkReferenceBase, public std::enable_shared_from_this<Ref>
{
public:
   /// ctor
   Ref();
   /// dtor
   ~Ref();

   /// adds EDSDK version text
   virtual void AddVersionText(CString& cszVersionText) const override;

   /// enumerates cameras
   virtual void EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& vecSourceDevices) const override;

   /// returns if AsyncWaitForCamera() is possible for this SDK
   virtual bool IsAsyncWaitPossible() const override { return true; }

   /// starts waiting for camera
   virtual void AsyncWaitForCamera(bool bStart, std::function<void()> fnOnCameraConnected = std::function<void()>()) override;

   /// called to do idle processing
   static void OnIdle();

   /// returns SDK function mutex
   RecursiveMutex& SdkFunctionMutex() { return m_mtxSdkFunctions; }

private:
   /// handler called when camera was added
   static EdsError EDSCALLBACK OnCameraAddedHandler(EdsVoid* inContext);

private:
   /// mutex to protect different threads from entering specific EDSDK functions
   RecursiveMutex m_mtxSdkFunctions;

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
   ~Handle()
   {
      try
      {
         Release();
      }
      catch (...)
      {
      }
   }

   /// returns pointer to base ref; non-const version
   EdsBaseRef* operator&()
   {
      return &m_objRef;
   }

   /// returns base ref
   operator EdsBaseRef()
   {
      return m_objRef;
   }

   /// returns base ref; const version
   operator EdsBaseRef() const
   {
      return m_objRef;
   }

   /// returns pointer to base ref
   operator EdsBaseRef*()
   {
      return &m_objRef;
   }

   /// returns base ref
   EdsBaseRef Get()
   {
      return m_objRef;
   }

   /// returns base ref; const version
   EdsBaseRef Get() const
   {
      return m_objRef;
   }

   /// returns SDK reference
   RefSp GetRef() { return m_spRef; }

   /// returns if handle is valid
   bool IsValid() const { return m_objRef != 0; }

private:
   /// increases reference count of object ref in h
   void Retain(const Handle& h)
   {
      if (h.m_objRef == 0)
         return; // null handle doesn't need to be retained

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
         return; // null handle doesn't need to be released

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
void MsgWaitForEvent(ManualResetEvent& evt);

} // namespace EDSDK
