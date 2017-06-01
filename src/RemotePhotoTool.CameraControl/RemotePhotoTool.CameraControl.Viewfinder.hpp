//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.Viewfinder.hpp Viewfinder wrapper class
//
#pragma once

// includes
#include "Instance.hpp"

class Viewfinder;

namespace RemotePhotoTool
{
   namespace CameraControl
   {
      /// Viewfinder class
      public ref class Viewfinder
      {
      public:
         /// device capabilities
         enum class ViewfinderCapability
         {
            /// can switch to output type Video Out
            OutputTypeVideoOut = 0,

            /// can get last histogram values
            GetHistogram = 1,
         };

         /// viewfinder output type
         enum class OutputType
         {
            /// undefined; don't use
            Undefined = 0,

            /// live viewfinder is also shown on LCD
            LCD = 1,

            /// live viewfinder is also shown on Video Out
            VideoOut = 2,

            /// live viewfinder is only transferred to PC
            Off = 3,
         };

         /// histogram type
         enum class HistogramType
         {
            /// luminance histogram
            Luminance = 0,

            /// histogram for red color channel only
            Red = 1,

            /// histogram for green color channel only
            Green = 2,

            /// histogram for blue color channel only
            Blue = 3,
         };

         /// returns capability in live viewfinder mode
         bool GetCapability(ViewfinderCapability viewfinderCapability);

         /// viewfinder output type
         property OutputType Output
         {
            OutputType get();
            void set(OutputType value);
         }

         /// handler function that is called when viewfinder image is available
         delegate void AvailViewfinderImageHandler(array<unsigned char>^ jpegImageData);

         /// event called when new viewfinder image is available
         event AvailViewfinderImageHandler^ AvailViewfinderImage;

         /// returns histogram of last captured live viewfinder image (may be empty when none was captured so far)
         array<unsigned int>^ GetHistogram(HistogramType histogramType);

         /// closes viewfinder
         void Close();

         /// finalizer
         !Viewfinder();

         /// dtor
         ~Viewfinder();

      internal:
         /// ctor; used in RemoteReleaseControl.StartViewfinder()
         Viewfinder(std::shared_ptr<::Viewfinder> viewfinder);

         /// initializes event handler
         void InitEventHandler();

         /// called when new viewfinder image data is available
         void OnAvailViewfinderImage(array<unsigned char>^ jpegImageData);

      private:
         /// implementation
         std::shared_ptr<::Viewfinder>* m_viewfinder;

         /// last set output type
         OutputType m_lastOutputType;
      };
   }
}
