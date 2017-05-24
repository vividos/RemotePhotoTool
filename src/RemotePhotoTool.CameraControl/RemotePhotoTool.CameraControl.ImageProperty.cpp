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

/// cleans up unmanaged resources
RemotePhotoTool::CameraControl::ImageProperty::!ImageProperty()
{
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
   const ::ImageProperty& imageProperty)
:m_imageProperty(new std::shared_ptr<::ImageProperty>(new ::ImageProperty(imageProperty)))
{
}
