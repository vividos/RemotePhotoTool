//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file GPhoto2SourceDeviceImpl.cpp gPhoto2 - SourceDevice impl
//
#pragma once

// includes
#include "stdafx.h"
#include "GPhoto2SourceDeviceImpl.hpp"
#include "Gphoto2RemoteReleaseControlImpl.hpp"
#include "GPhoto2PropertyAccess.hpp"
#include "CameraException.hpp"
#include "GPhoto2Include.hpp"

using GPhoto2::SourceDeviceImpl;

SourceDeviceImpl::SourceDeviceImpl(std::shared_ptr<_GPContext> spContext, std::shared_ptr<_Camera> spCamera)
   :m_spContext(spContext),
   m_spCamera(spCamera),
   m_properties(spContext, spCamera)
{
}

SourceDeviceImpl::~SourceDeviceImpl() throw()
{
}

bool SourceDeviceImpl::GetDeviceCapability(T_enDeviceCapability enDeviceCapability) const
{
   switch (enDeviceCapability)
   {
   case capRemoteReleaseControl:
      return m_properties.GetCameraOperationAbility(CameraOperation::GP_OPERATION_CAPTURE_IMAGE);

   case capRemoteViewfinder:
      return m_properties.GetCameraOperationAbility(CameraOperation::GP_OPERATION_CAPTURE_PREVIEW);

   default:
      ATLASSERT(false);
      break;
   }

   return false;
}

CString SourceDeviceImpl::ModelName() const
{
   return m_properties.GetText("model");
}

CString SourceDeviceImpl::SerialNumber() const
{
   return m_properties.GetText("serialnumber");
}

std::vector<unsigned int> SourceDeviceImpl::EnumDeviceProperties() const
{
   return m_properties.EnumDeviceProperties();
}

DeviceProperty SourceDeviceImpl::GetDeviceProperty(unsigned int uiPropertyId) const
{
   return m_properties.GetDeviceProperty(uiPropertyId);
}

std::shared_ptr<RemoteReleaseControl> SourceDeviceImpl::EnterReleaseControl()
{
   return std::make_shared<RemoteReleaseControlImpl>(m_spContext, m_spCamera);
}
