//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.RemoteReleaseControl.cpp RemoteReleaseControl wrapper class
//

// includes
#include "stdafx.h"
#include "RemotePhotoTool.CameraControl.RemoteReleaseControl.hpp"
#include "RemotePhotoTool.CameraControl.Viewfinder.hpp"
#include "RemotePhotoTool.CameraControl.BulbReleaseControl.hpp"
#include "RemoteReleaseControl.hpp"

RemotePhotoTool::CameraControl::Viewfinder^
RemotePhotoTool::CameraControl::RemoteReleaseControl::StartViewfinder()
{
   std::shared_ptr<::Viewfinder> spViewfinder =
      m_remoteReleaseControl->get()->StartViewfinder();

   return gcnew RemotePhotoTool::CameraControl::Viewfinder(spViewfinder);
}

RemotePhotoTool::CameraControl::BulbReleaseControl^
RemotePhotoTool::CameraControl::RemoteReleaseControl::StartBulb()
{
   std::shared_ptr<::BulbReleaseControl> spBulbReleaseControl =
      m_remoteReleaseControl->get()->StartBulb();

   return gcnew RemotePhotoTool::CameraControl::BulbReleaseControl(spBulbReleaseControl);
}

/// cleans up unmanaged resources
RemotePhotoTool::CameraControl::RemoteReleaseControl::!RemoteReleaseControl()
{
   delete m_remoteReleaseControl;
   m_remoteReleaseControl = nullptr;
}

/// cleans up managed resources
RemotePhotoTool::CameraControl::RemoteReleaseControl::~RemoteReleaseControl()
{
}

RemotePhotoTool::CameraControl::RemoteReleaseControl::RemoteReleaseControl(
   std::shared_ptr<::RemoteReleaseControl> remoteReleaseControl)
:m_remoteReleaseControl(new std::shared_ptr<::RemoteReleaseControl>(remoteReleaseControl))
{
}
