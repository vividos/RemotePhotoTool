//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file EdsdkViewfinderImpl.hpp EDSDK - Viewfinder impl
//
#pragma once

// includes
#include "Viewfinder.hpp"
#include "EdsdkCommon.hpp"
#include "Asio.hpp"
#include "Event.hpp"
#include <atomic>

// forward references
class BackgroundTimer;

namespace EDSDK
{

/// viewfinder impl for EDSDK
class ViewfinderImpl:
   public Viewfinder,
   public std::enable_shared_from_this<ViewfinderImpl>
{
public:
   /// ctor
   ViewfinderImpl(Handle hSourceDevice, boost::asio::io_service& ioService, std::shared_ptr<LightweightMutex> spMtxLock);
   /// dtor
   virtual ~ViewfinderImpl() throw();

   virtual bool GetCapability(T_enViewfinderCapability enViewfinderCapability) const throw() override;

   virtual void SetOutputType(Viewfinder::T_enOutputType enOutputType) override;

   virtual void SetAvailImageHandler(Viewfinder::T_fnOnAvailViewfinderImage fnOnAvailViewfinderImage) override;

   virtual void GetHistogram(T_enHistogramType enHistogramType, std::vector<unsigned int>& vecHistogramData) override;

   virtual void Close() override;

private:
   /// starts background thread to fetch images
   void StartBackgroundThread();

   /// stops background thread
   void StopBackgroundThread();

   /// stops background thread; runs in worker thread
   void AsyncStopBackgroundThread();

   /// retrieves viewfinder image data
   void GetImage(std::vector<BYTE>& vecImage);

   /// reads histogram data from image
   void ReadHistogram(Handle& hEvfImage);

   /// timer handler to retrieve viewfinder image
   void OnGetViewfinderImage();

private:
   /// source device
   Handle m_hSourceDevice;

   /// background thread io service
   boost::asio::io_service& m_ioService;

   /// lock to synchronize access to ED-SDK during viewfinder
   std::shared_ptr<LightweightMutex> m_spMtxLock;

   /// event to signal that timer has stopped
   Event m_evtTimerStopped;

   /// indicates if worker thread is in GetImage()
   std::atomic<bool> m_bInGetImage;

   /// thread that polls camera for viewfinder image
   std::shared_ptr<BackgroundTimer> m_spViewfinderImageTimer;

   /// mutex to protect m_fnOnAvailViewfinderImage
   LightweightMutex m_mtxFnOnAvailViewfinderImage;

   /// viewfinder image handler
   Viewfinder::T_fnOnAvailViewfinderImage m_fnOnAvailViewfinderImage;

   /// mutex to protect histogram arrays
   LightweightMutex m_mtxHistogram;

   EdsUInt32 m_histogramY[256]; ///< histogram for luminance
   EdsUInt32 m_histogramR[256]; ///< histogram for red channel
   EdsUInt32 m_histogramG[256]; ///< histogram for green channel
   EdsUInt32 m_histogramB[256]; ///< histogram for blue channel
};

} // namespace EDSDK
