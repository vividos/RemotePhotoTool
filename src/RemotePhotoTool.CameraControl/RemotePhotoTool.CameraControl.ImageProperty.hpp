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
class RemoteReleaseControl;

namespace RemotePhotoTool
{
   namespace CameraControl
   {
      public ref class ImageProperty : public System::IEquatable<ImageProperty^>
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

         /// returns list of currently valid values for this image property
         property System::Collections::Generic::List<ImageProperty^>^ ValidValues
         {
            System::Collections::Generic::List<ImageProperty^>^ get();
         }

         //#region IEquatable implementation
         virtual bool Equals(ImageProperty^ other);

         /// finalizer
         !ImageProperty();

         /// dtor
         ~ImageProperty();

      internal:
         /// ctor
         ImageProperty(const ::ImageProperty& imageProperty,
            std::shared_ptr<::RemoteReleaseControl>& remoteReleaseControl);

         /// returns raw image property
         ::ImageProperty* GetRaw();

      private:
         /// implementation
         std::shared_ptr<::ImageProperty>* m_imageProperty;

         /// release control
         std::shared_ptr<::RemoteReleaseControl>* m_remoteReleaseControl;
      };
   }
}
