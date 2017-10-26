//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file WiaCommon.hpp WIA - Windows Image Acquisition common functions
//
#pragma once

// includes
#include "Instance.hpp"
#include "CameraException.hpp"
#include "SdkReferenceBase.hpp"
#include <ulib/thread/LightweightMutex.hpp>
#include <wia_lh.h>

/// Windows Image Acquisition interface
namespace WIA
{
   /// checks for errors and throws an exception
   extern void CheckError(const CString& function, HRESULT hr, LPCSTR file, UINT line);

   /// WIA reference
   /// \see https://msdn.microsoft.com/en-us/library/windows/desktop/ms629859%28v=vs.85%29.aspx
   class Ref :
      public SdkReferenceBase,
      public std::enable_shared_from_this<Ref>,
      public IWiaEventCallback
   {
   public:
      /// ctor
      Ref();
      /// dtor
      ~Ref() throw();

      /// adds WIA version text
      virtual void AddVersionText(CString& versionText) const override;

      /// enumerates cameras
      virtual void EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& sourceInfoList) const override;

      /// returns if AsyncWaitForCamera() is possible for this SDK
      virtual bool IsAsyncWaitPossible() const override { return true; }
      /// starts waiting for camera
      virtual void AsyncWaitForCamera(bool bStart, std::function<void()> fnOnCameraConnected = std::function<void()>()) override;

      /// returns WIA device manager
      CComPtr<IWiaDevMgr> GetWiaDeviceManager()
      {
         return m_wiaDevMgr;
      }

   private:
      // virtual methods from IUnknown

      /// queries COM object for given interface and returns it
      virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;

      /// increases reference count
      virtual ULONG STDMETHODCALLTYPE AddRef() override;

      /// decreases reference count
      virtual ULONG STDMETHODCALLTYPE Release() override;

      // virtual methods from IWiaEventCallback

      /// handler called when camera was added
      virtual HRESULT STDMETHODCALLTYPE ImageEventCallback(
         const GUID* pEventGUID,
         BSTR bstrEventDescription,
         BSTR bstrDeviceID,
         BSTR bstrDeviceDescription,
         DWORD dwDeviceType,
         BSTR bstrFullItemName,
         ULONG* pulEventType,
         ULONG ulReserved) override;

   private:
      /// WIA 1.0 device manager
      CComPtr<IWiaDevMgr> m_wiaDevMgr;

      /// mutex to protect m_fnOnCameraConnected
      LightweightMutex m_mtxFnOnCameraConnected;

      /// handler to call when camera is connected
      std::function<void()> m_fnOnCameraConnected;

      /// event object; must be held until no more events should be received
      CComPtr<IUnknown> m_eventObject;
   };

   /// smart pointer to WIA reference
   typedef std::shared_ptr<Ref> RefSp;

} // namespace WIA
