//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.SourceInfo.cpp SourceInfo wrapper class
//

// includes
#include "stdafx.h"
#include "RemotePhotoTool.CameraControl.SourceInfo.hpp"
#include "RemotePhotoTool.CameraControl.SourceDevice.hpp"
#include "SourceInfo.hpp"

System::String^ RemotePhotoTool::CameraControl::SourceInfo::Name::get()
{
   return gcnew System::String(m_sourceInfo->get()->Name().GetString());
}

System::String^ RemotePhotoTool::CameraControl::SourceInfo::DeviceId::get()
{
   return gcnew System::String(m_sourceInfo->get()->DeviceId().GetString());
}

RemotePhotoTool::CameraControl::SourceDevice^ RemotePhotoTool::CameraControl::SourceInfo::Open()
{
   std::shared_ptr<::SourceDevice> sourceDevice = m_sourceInfo->get()->Open();

   return gcnew RemotePhotoTool::CameraControl::SourceDevice(sourceDevice);
}

/// cleans up unmanaged resources
RemotePhotoTool::CameraControl::SourceInfo::!SourceInfo()
{
   delete m_sourceInfo;
   m_sourceInfo = nullptr;
}

/// cleans up managed resources
RemotePhotoTool::CameraControl::SourceInfo::~SourceInfo()
{
}

RemotePhotoTool::CameraControl::SourceInfo::SourceInfo(std::shared_ptr<::SourceInfo> sourceInfo)
:m_sourceInfo(new std::shared_ptr<::SourceInfo>(sourceInfo))
{
}
