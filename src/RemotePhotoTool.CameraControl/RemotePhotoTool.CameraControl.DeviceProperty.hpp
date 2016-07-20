//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.DeviceProperty.hpp DeviceProperty wrapper class
//
#pragma once

// includes
#include "Instance.hpp"

class DeviceProperty;

namespace RemotePhotoTool
{
   namespace CameraControl
   {
      public ref class DeviceProperty
      {
      public:
         /// device property id
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

         /// returns if property is read-only
         property bool IsReadOnly
         {
            bool get();
         }

         /// returns list of valid values
         property System::Collections::Generic::List<DeviceProperty^>^ ValidValues
         {
            System::Collections::Generic::List<DeviceProperty^>^ get();
         }

         /// finalizer
         !DeviceProperty();

         /// dtor
         ~DeviceProperty();

      internal:
         /// ctor
         DeviceProperty(const ::DeviceProperty& deviceProperty);

      private:
         std::shared_ptr<::DeviceProperty>* m_deviceProperty;
      };
   }
}
