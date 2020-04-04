//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2SourceDeviceImpl.cpp gPhoto2 - SourceDevice impl
//
#pragma once

#include "stdafx.h"
#include "GPhoto2Include.hpp"
#include "GPhoto2SourceDeviceImpl.hpp"
#include "Gphoto2RemoteReleaseControlImpl.hpp"
#include "GPhoto2PropertyAccess.hpp"
#include "Gphoto2CameraFileSystemImpl.hpp"
#include "CameraException.hpp"

using GPhoto2::SourceDeviceImpl;

SourceDeviceImpl::SourceDeviceImpl(RefSp ref, std::shared_ptr<_Camera> camera)
   :m_ref(ref),
   m_camera(camera),
   m_properties(new PropertyAccess(ref->GetContext(), camera))
{
}

SourceDeviceImpl::~SourceDeviceImpl()
{
}

bool SourceDeviceImpl::GetDeviceCapability(T_enDeviceCapability deviceCapability) const
{
   switch (deviceCapability)
   {
   case capRemoteReleaseControl:
      return m_properties->GetCameraOperationAbility(CameraOperation::GP_OPERATION_CAPTURE_IMAGE);

   case capRemoteViewfinder:
      return m_properties->GetCameraOperationAbility(CameraOperation::GP_OPERATION_CAPTURE_PREVIEW);

   case capCameraFileSystem:
      return true;

   default:
      ATLASSERT(false);
      break;
   }

   return false;
}

CString SourceDeviceImpl::ModelName() const
{
   return m_properties->GetText("model");
}

CString SourceDeviceImpl::SerialNumber() const
{
   return m_properties->GetText("serialnumber");
}

std::vector<unsigned int> SourceDeviceImpl::EnumDeviceProperties() const
{
   return m_properties->EnumDeviceProperties();
}

DeviceProperty SourceDeviceImpl::GetDeviceProperty(unsigned int propertyId) const
{
   return m_properties->GetDeviceProperty(propertyId);
}

std::shared_ptr<CameraFileSystem> SourceDeviceImpl::GetFileSystem()
{
   return std::make_shared<CameraFileSystemImpl>(m_ref->GetContext(), m_camera);
}

std::shared_ptr<RemoteReleaseControl> SourceDeviceImpl::EnterReleaseControl()
{
   return std::make_shared<RemoteReleaseControlImpl>(m_ref, m_camera, m_properties);
}
