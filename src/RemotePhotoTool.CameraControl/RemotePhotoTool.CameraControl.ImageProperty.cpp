//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.ImageProperty.cpp ImageProperty wrapper class
//

// includes
#include "stdafx.h"
#include "RemotePhotoTool.CameraControl.ImageProperty.hpp"
#include "ImageProperty.hpp"
#include "RemoteReleaseControl.hpp"

unsigned int RemotePhotoTool::CameraControl::ImageProperty::Id::get()
{
   return m_imageProperty->get()->Id();
}

System::String^ RemotePhotoTool::CameraControl::ImageProperty::Name::get()
{
   return gcnew System::String(m_imageProperty->get()->Name().GetString());
}

System::String^ RemotePhotoTool::CameraControl::ImageProperty::DisplayText::get()
{
   return gcnew System::String(m_imageProperty->get()->AsString().GetString());
}

bool RemotePhotoTool::CameraControl::ImageProperty::IsReadOnly::get()
{
   return m_imageProperty->get()->IsReadOnly();
}

System::Collections::Generic::List<RemotePhotoTool::CameraControl::ImageProperty^>^
RemotePhotoTool::CameraControl::ImageProperty::ValidValues::get()
{
   unsigned int imagePropertyId = m_imageProperty->get()->Id();

   std::vector<::ImageProperty> validValuesList;
   if (m_remoteReleaseControl != nullptr &&
      m_remoteReleaseControl->get() != nullptr)
   {
      m_remoteReleaseControl->get()->EnumImagePropertyValues(imagePropertyId, validValuesList);
   }

   auto imagePropertyCollection = gcnew System::Collections::Generic::List<RemotePhotoTool::CameraControl::ImageProperty^>();

   for (size_t index = 0, max = validValuesList.size(); index < max; index++)
   {
      ::ImageProperty imageProperty = validValuesList[index];

      imagePropertyCollection->Add(
         gcnew RemotePhotoTool::CameraControl::ImageProperty(imageProperty,
         *m_remoteReleaseControl));
   }

   return imagePropertyCollection;
}

/// cleans up unmanaged resources
RemotePhotoTool::CameraControl::ImageProperty::!ImageProperty()
{
   delete m_remoteReleaseControl;
   m_remoteReleaseControl = nullptr;

   delete m_imageProperty;
   m_imageProperty = nullptr;
}

/// cleans up managed resources
RemotePhotoTool::CameraControl::ImageProperty::~ImageProperty()
{
}

::ImageProperty* RemotePhotoTool::CameraControl::ImageProperty::GetRaw()
{
   return m_imageProperty->get();
}

RemotePhotoTool::CameraControl::ImageProperty::ImageProperty(
   const ::ImageProperty& imageProperty,
   std::shared_ptr<::RemoteReleaseControl>& remoteReleaseControl)
:m_imageProperty(new std::shared_ptr<::ImageProperty>(new ::ImageProperty(imageProperty))),
m_remoteReleaseControl(new std::shared_ptr<::RemoteReleaseControl>(remoteReleaseControl))
{
}
