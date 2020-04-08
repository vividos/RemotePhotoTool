//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2ViewfinderImpl.hpp gPhoto2 - Viewfinder impl
//
#pragma once

#include "GPhoto2Common.hpp"
#include "Viewfinder.hpp"
#include <ulib/thread/Mutex.hpp>
#include <ulib/thread/Event.hpp>

class BackgroundTimer;

namespace GPhoto2
{
   class PropertyAccess;

   /// implementation of Viewfinder for gPhoto2 access
   class ViewfinderImpl :
      public Viewfinder,
      public std::enable_shared_from_this<ViewfinderImpl>
   {
   public:
      /// ctor
      ViewfinderImpl(RefSp ref, std::shared_ptr<_Camera> camera, std::shared_ptr<PropertyAccess> properties,
         boost::asio::io_service& ioService);

      /// dtor
      virtual ~ViewfinderImpl();

      // Viewfinder virtual functions

      /// returns capability in live viewfinder mode
      virtual bool GetCapability(T_enViewfinderCapability viewfinderCapability) const override;

      /// sets viewfinder output type
      virtual void SetOutputType(T_enOutputType outputType) override;

      /// callback function type to call when viewfinder image is available
      typedef std::function<void(const std::vector<BYTE>& imageData)> T_fnOnAvailViewfinderImage;

      /// sets (or resets) viewfinder callback
      virtual void SetAvailImageHandler(T_fnOnAvailViewfinderImage onAvailViewfinderImage = T_fnOnAvailViewfinderImage()) override;

      /// returns histogram of last captured live viewfinder image (may be empty when none was captured so far)
      virtual void GetHistogram(T_enHistogramType histogramType, std::vector<unsigned int>& histogramData) override;

      /// closes viewfinder
      virtual void Close() override;

   private:
      /// starts background thread to fetch images
      void StartBackgroundThread();

      /// stops background thread
      void StopBackgroundThread();

      /// stops background thread; runs in worker thread
      void AsyncStopBackgroundThread();

      /// timer handler to retrieve viewfinder image
      void OnGetViewfinderImage();

      /// retrieves viewfinder image data
      void GetImage(std::vector<BYTE>& imageData);

   private:
      /// gPhoto2 reference
      RefSp m_ref;

      /// camera instance
      std::shared_ptr<_Camera> m_camera;

      /// camera propertiers
      std::shared_ptr<PropertyAccess> m_properties;

      /// mutex to protect m_fnOnAvailViewfinderImage
      LightweightMutex m_mutexFnOnAvailViewfinderImage;

      /// viewfinder image handler
      Viewfinder::T_fnOnAvailViewfinderImage m_onAvailViewfinderImage;

      /// thread that polls camera for viewfinder image
      std::shared_ptr<BackgroundTimer> m_viewfinderImageTimer;

      /// event to signal that timer has stopped
      ManualResetEvent m_eventTimerStopped;

      /// background thread io service
      boost::asio::io_service& m_ioService;
   };

} // namespace GPhoto2
