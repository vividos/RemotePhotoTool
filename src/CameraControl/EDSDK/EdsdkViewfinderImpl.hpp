//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file EdsdkViewfinderImpl.hpp EDSDK - Viewfinder impl
//
#pragma once

#include "Viewfinder.hpp"
#include "EdsdkCommon.hpp"
#include <ulib/thread/Event.hpp>
#include <array>
#include <atomic>

class SingleThreadExecutor;
class PeriodicExecuteTimer;

namespace EDSDK
{

/// viewfinder impl for EDSDK
class ViewfinderImpl:
   public Viewfinder,
   public std::enable_shared_from_this<ViewfinderImpl>
{
public:
   /// ctor
   ViewfinderImpl(Handle hSourceDevice, SingleThreadExecutor& executor, std::shared_ptr<LightweightMutex> spMtxLock);
   /// dtor
   virtual ~ViewfinderImpl();

   virtual bool GetCapability(T_enViewfinderCapability enViewfinderCapability) const override;

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

   /// background thread executor
   SingleThreadExecutor& m_executor;

   /// lock to synchronize access to ED-SDK during viewfinder
   std::shared_ptr<LightweightMutex> m_spMtxLock;

   /// event to signal that timer has stopped
   ManualResetEvent m_evtTimerStopped;

   /// indicates if worker thread is in GetImage()
   std::atomic<bool> m_bInGetImage;

   /// timer for polling camera for viewfinder image
   std::shared_ptr<PeriodicExecuteTimer> m_spViewfinderImageTimer;

   /// mutex to protect m_fnOnAvailViewfinderImage
   LightweightMutex m_mtxFnOnAvailViewfinderImage;

   /// viewfinder image handler
   Viewfinder::T_fnOnAvailViewfinderImage m_fnOnAvailViewfinderImage;

   /// mutex to protect histogram arrays
   LightweightMutex m_mtxHistogram;

   std::array<EdsUInt32, 256> m_histogramY; ///< histogram for luminance
   std::array<EdsUInt32, 256> m_histogramR; ///< histogram for red channel
   std::array<EdsUInt32, 256> m_histogramG; ///< histogram for green channel
   std::array<EdsUInt32, 256> m_histogramB; ///< histogram for blue channel
};

} // namespace EDSDK
