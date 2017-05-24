//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.BulbReleaseControl.cpp BulbReleaseControl wrapper class
//

// includes
#include "stdafx.h"
#include "RemotePhotoTool.CameraControl.BulbReleaseControl.hpp"
#include "BulbReleaseControl.hpp"

System::TimeSpan RemotePhotoTool::CameraControl::BulbReleaseControl::ElapsedTime::get()
{
   return System::TimeSpan::FromSeconds(m_bulbReleaseControl->get()->ElapsedTime());
}

void RemotePhotoTool::CameraControl::BulbReleaseControl::Stop()
{
   m_bulbReleaseControl->get()->Stop();
}

/// cleans up unmanaged resources
RemotePhotoTool::CameraControl::BulbReleaseControl::!BulbReleaseControl()
{
   delete m_bulbReleaseControl;
   m_bulbReleaseControl = nullptr;
}

/// cleans up managed resources
RemotePhotoTool::CameraControl::BulbReleaseControl::~BulbReleaseControl()
{
}

RemotePhotoTool::CameraControl::BulbReleaseControl::BulbReleaseControl(
   std::shared_ptr<::BulbReleaseControl> bulbReleaseControl)
:m_bulbReleaseControl(new std::shared_ptr<::BulbReleaseControl>(bulbReleaseControl))
{
}
