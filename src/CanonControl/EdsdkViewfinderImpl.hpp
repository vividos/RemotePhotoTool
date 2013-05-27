//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file EdsdkViewfinderImpl.hpp EDSDK - Viewfinder impl
//
#pragma once

// includes
#include "Viewfinder.hpp"
#include "EdsdkCommon.hpp"

// forward references
class BackgroundWorkerThread;
class BackgroundTimer;

namespace EDSDK
{

/// viewfinder impl for EDSDK
class ViewfinderImpl: public Viewfinder
{
public:
   /// ctor
   ViewfinderImpl(Handle hSourceDevice);
   /// dtor
   virtual ~ViewfinderImpl();

   virtual void SetAvailImageHandler(Viewfinder::T_fnOnAvailViewfinderImage fnOnAvailViewfinderImage) override;

private:
   /// starts background thread to fetch images
   void StartBackgroundThread();

   /// stops background thread
   void StopBackgroundThread();

   /// retrieves viewfinder image data
   void GetImage(std::vector<BYTE>& vecImage);

   /// timer handler to retrieve viewfinder image
   void OnGetViewfinderImage();

private:
   /// source device
   Handle m_hSourceDevice;

   /// worker thread
   std::shared_ptr<BackgroundWorkerThread> m_spWorkerThread;

   /// thread that polls camera for viewfinder image
   std::shared_ptr<BackgroundTimer> m_spViewfinderImageThread;

   /// mutex to protect m_fnOnAvailViewfinderImage
   LightweightMutex m_mtxFnOnAvailViewfinderImage;

   /// viewfinder image handler
   Viewfinder::T_fnOnAvailViewfinderImage m_fnOnAvailViewfinderImage;
};

} // namespace EDSDK
