//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file WiaRemoteReleaseControlImpl.cpp WIA - RemoteRelaseControl impl
//
#pragma once

// includes
#include "stdafx.h"
#include "WiaRemoteReleaseControlImpl.hpp"

using WIA::RemoteReleaseControlImpl;

RemoteReleaseControlImpl::~RemoteReleaseControlImpl()
{
   try
   {
      Close();
   }
   catch (...)
   {
   }
}

bool RemoteReleaseControlImpl::GetCapability(RemoteReleaseControl::T_enRemoteCapability capability) const
{
   switch (capability)
   {
   case RemoteReleaseControl::capChangeShootingParameter:
      // TODO check
      return false;

   case RemoteReleaseControl::capChangeShootingMode:
      // TODO check
      return false;

   case RemoteReleaseControl::capZoomControl:
      // TODO check
      return false;

   case RemoteReleaseControl::capViewfinder:
      // TODO check
      return false;

   case RemoteReleaseControl::capReleaseWhileViewfinder:
      // TODO check
      return false;

   case RemoteReleaseControl::capAFLock:
      // TODO check
      return false;

   case RemoteReleaseControl::capBulbMode:
      // bulb mode generally not supported by WIA
      return false;

   case RemoteReleaseControl::capUILock:
      return false;

   default:
      ATLASSERT(false);
      break;
   }

   return false;
}

void RemoteReleaseControlImpl::SetReleaseSettings(const ShutterReleaseSettings& settings)
{
   LightweightMutex::LockType lock(m_mtxShutterReleaseSettings);
   m_shutterReleaseSettings = settings;
}

unsigned int RemoteReleaseControlImpl::MapImagePropertyTypeToId(T_enImagePropertyType imagePropertyType) const
{
   // TODO implement
   UNUSED(imagePropertyType);
   return 0;
}

ImageProperty RemoteReleaseControlImpl::MapShootingModeToImagePropertyValue(RemoteReleaseControl::T_enShootingMode shootingMode) const
{
   // TODO implement
   UNUSED(shootingMode);
   return GetImageProperty(0);
}

ImageProperty RemoteReleaseControlImpl::GetImageProperty(unsigned int imagePropertyID) const
{
   // TODO implement
   return ImageProperty(variantWia, imagePropertyID, Variant(), true);
}

void RemoteReleaseControlImpl::SetImageProperty(const ImageProperty& imageProperty)
{
   // TODO implement
   UNUSED(imageProperty);
}

void RemoteReleaseControlImpl::EnumImagePropertyValues(unsigned int imagePropertyID, std::vector<ImageProperty>& valuesList) const
{
   // TODO implement
   UNUSED(imagePropertyID);
   UNUSED(valuesList);
}

std::vector<unsigned int> RemoteReleaseControlImpl::EnumImageProperties() const
{
   // TODO implement
   return std::vector<unsigned int>();
}

std::shared_ptr<Viewfinder> RemoteReleaseControlImpl::StartViewfinder() const
{
   throw CameraException(_T("WIA::RemoteReleaseControl::StartViewfinder"), _T("Not supported"),
      0, __FILE__, __LINE__);
}

unsigned int RemoteReleaseControlImpl::NumAvailableShots() const
{
   // TODO implement
   return 0;
}

void RemoteReleaseControlImpl::SendCommand(RemoteReleaseControl::T_enCameraCommand cameraCommand)
{
   // TODO implement
   UNUSED(cameraCommand);
}

void RemoteReleaseControlImpl::Release()
{
   // TODO implement
   throw CameraException(_T("WIA::RemoteReleaseControl::Release"), _T("Not supported"),
      0, __FILE__, __LINE__);
}

std::shared_ptr<BulbReleaseControl> RemoteReleaseControlImpl::StartBulb()
{
   // bulb not supported by WIA
   throw CameraException(_T("WIA::RemoteReleaseControl::StartBulb"), _T("Not supported"),
      0, __FILE__, __LINE__);
}

void RemoteReleaseControlImpl::Close()
{
   m_ref.reset();
}
