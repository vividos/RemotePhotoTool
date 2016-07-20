//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.Viewfinder.cpp Viewfinder wrapper class
//

// includes
#include "stdafx.h"
#include "RemotePhotoTool.CameraControl.Viewfinder.hpp"
#include "Viewfinder.hpp"

/// cleans up unmanaged resources
RemotePhotoTool::CameraControl::Viewfinder::!Viewfinder()
{
   delete m_viewfinder;
   m_viewfinder = nullptr;
}

/// cleans up managed resources
RemotePhotoTool::CameraControl::Viewfinder::~Viewfinder()
{
}

RemotePhotoTool::CameraControl::Viewfinder::Viewfinder(
   std::shared_ptr<::Viewfinder> viewfinder)
:m_viewfinder(new std::shared_ptr<::Viewfinder>(viewfinder))
{
}
