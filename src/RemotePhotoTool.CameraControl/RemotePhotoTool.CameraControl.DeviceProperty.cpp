//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.DeviceProperty.cpp DeviceProperty wrapper class
//

// includes
#include "stdafx.h"
#include "RemotePhotoTool.CameraControl.DeviceProperty.hpp"
#include "DeviceProperty.hpp"

unsigned int RemotePhotoTool::CameraControl::DeviceProperty::Id::get()
{
   return m_deviceProperty->get()->Id();
}

System::String^ RemotePhotoTool::CameraControl::DeviceProperty::Name::get()
{
   return gcnew System::String(m_deviceProperty->get()->Name().GetString());
}

System::String^ RemotePhotoTool::CameraControl::DeviceProperty::DisplayText::get()
{
   return gcnew System::String(m_deviceProperty->get()->AsString().GetString());
}

bool RemotePhotoTool::CameraControl::DeviceProperty::IsReadOnly::get()
{
   return m_deviceProperty->get()->IsReadOnly();
}

System::Collections::Generic::List<RemotePhotoTool::CameraControl::DeviceProperty^>^
RemotePhotoTool::CameraControl::DeviceProperty::ValidValues::get()
{
   auto devicePropertyCollection =
      gcnew System::Collections::Generic::List<DeviceProperty^>();

   const std::vector<::Variant> valuesList = m_deviceProperty->get()->ValidValues();

   for (size_t index = 0, max = valuesList.size(); index < max; index++)
   {
      const ::Variant& value = valuesList[index];

      ::DeviceProperty deviceProperty =
         m_deviceProperty->get()->PropertyFromVariantValue(value);

      devicePropertyCollection->Add(gcnew RemotePhotoTool::CameraControl::DeviceProperty(deviceProperty));
   }

   return devicePropertyCollection;
}

/// cleans up unmanaged resources
RemotePhotoTool::CameraControl::DeviceProperty::!DeviceProperty()
{
   delete m_deviceProperty;
   m_deviceProperty = nullptr;
}

/// cleans up managed resources
RemotePhotoTool::CameraControl::DeviceProperty::~DeviceProperty()
{
}

RemotePhotoTool::CameraControl::DeviceProperty::DeviceProperty(
   const ::DeviceProperty& deviceProperty)
:m_deviceProperty(new std::shared_ptr<::DeviceProperty>(new ::DeviceProperty(deviceProperty)))
{
}
