//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file WiaSourceDeviceImpl.cpp WIA - SourceDevice impl
//
#pragma once

// includes
#include "stdafx.h"
#include "WiaSourceDeviceImpl.hpp"
#include "WiaPropertyAccess.hpp"
#include "WiaCameraFileSystemImpl.hpp"

using namespace WIA;

bool SourceDeviceImpl::GetDeviceCapability(SourceDevice::T_enDeviceCapability deviceCapability) const
{
   switch (deviceCapability)
   {
   case SourceDevice::capRemoteReleaseControl:
      return false; // not implemented

   case SourceDevice::capRemoteViewfinder:
      return false; // not implemented

   case SourceDevice::capCameraFileSystem:
      // supported on all camera models
      return true;

   default:
      ATLASSERT(false);
   }

   return false;
}

CString SourceDeviceImpl::ModelName() const
{
   WIA::PropertyAccess access{ m_wiaDeviceRootItem };

   return access.Get(WIA_DIP_DEV_NAME);
}

CString SourceDeviceImpl::SerialNumber() const
{
   WIA::PropertyAccess access{ m_wiaDeviceRootItem };

   // it's not the actual serial number, but the closest match we have in WIA
   return access.Get(WIA_DIP_DEV_ID);
}

std::vector<unsigned int> SourceDeviceImpl::EnumDeviceProperties() const
{
   throw CameraException(_T(""), _T(""), 0, __FILE__, __LINE__);
}

DeviceProperty SourceDeviceImpl::GetDeviceProperty(unsigned int propertyId) const
{
   propertyId;
   throw CameraException(_T(""), _T(""), 0, __FILE__, __LINE__);
}

std::shared_ptr<CameraFileSystem> SourceDeviceImpl::GetFileSystem()
{
   if (!GetDeviceCapability(capCameraFileSystem))
   {
      throw CameraException(_T("WIA::SourceDevice::GetFileSystem"),
         _T("Not supported"), 0, __FILE__, __LINE__);
   }

   return std::shared_ptr<CameraFileSystem>(new WIA::CameraFileSystemImpl(m_ref, m_wiaDeviceRootItem));
}

std::shared_ptr<RemoteReleaseControl> SourceDeviceImpl::EnterReleaseControl()
{
   throw CameraException(_T("WIA::SourceDevice::EnterReleaseControl"),
      _T("Not supported"), 0, __FILE__, __LINE__);
}
