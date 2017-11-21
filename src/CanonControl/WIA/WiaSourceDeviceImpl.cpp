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
#include "WiaCameraFileSystemImpl.hpp"
#include "WiaRemoteReleaseControlImpl.hpp"

using namespace WIA;

bool SourceDeviceImpl::GetDeviceCapability(SourceDevice::T_enDeviceCapability deviceCapability) const
{
   switch (deviceCapability)
   {
   case SourceDevice::capRemoteReleaseControl:
      return true;

   case SourceDevice::capRemoteViewfinder:
      // TODO check if supported
      return false;

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
   // TODO implement
   return CString();
}

CString SourceDeviceImpl::SerialNumber() const
{
   // TODO implement
   return CString();
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
   return std::make_shared<WIA::RemoteReleaseControlImpl>(m_ref);
}
