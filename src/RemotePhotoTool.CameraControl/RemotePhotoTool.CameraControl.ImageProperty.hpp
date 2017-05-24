//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.ImageProperty.hpp ImageProperty wrapper class
//
#pragma once

// includes
#include "Instance.hpp"

class ImageProperty;

namespace RemotePhotoTool
{
   namespace CameraControl
   {
      public ref class ImageProperty
      {
      public:
         /// image property id
         property unsigned int Id
         {
            unsigned int get();
         }

         /// property name
         property System::String^ Name
         {
            System::String^ get();
         }

         /// property value as string
         property System::String^ DisplayText
         {
            System::String^ get();
         }

         /// returns if image property is read-only
         property bool IsReadOnly
         {
            bool get();
         }

         /// finalizer
         !ImageProperty();

         /// dtor
         ~ImageProperty();

      internal:
         /// ctor
         ImageProperty(const ::ImageProperty& imageProperty);

         /// returns raw image property
         ::ImageProperty* GetRaw();

      private:
         /// implementation
         std::shared_ptr<::ImageProperty>* m_imageProperty;
      };
   }
}
