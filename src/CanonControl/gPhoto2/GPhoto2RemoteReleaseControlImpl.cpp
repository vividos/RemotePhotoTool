//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file GPhoto2RemoteReleaseControlImpl.cpp gPhoto2 - Remote release control impl
//

// includes
#include "stdafx.h"
#include "GPhoto2RemoteReleaseControlImpl.hpp"
#include "CameraException.hpp"
#include "AsyncReleaseControlThread.hpp"

using GPhoto2::RemoteReleaseControlImpl;

RemoteReleaseControlImpl::RemoteReleaseControlImpl(std::shared_ptr<_GPContext> spContext, std::shared_ptr<_Camera> spCamera)
   :m_spContext(spContext),
   m_spCamera(spCamera)
{
}

RemoteReleaseControlImpl::~RemoteReleaseControlImpl() throw()
{
   try
   {
      Close();
   }
   catch (...)
   {
   }
}

bool RemoteReleaseControlImpl::GetCapability(T_enRemoteCapability enRemoteCapability) const throw()
{
   // TODO implement
   switch (enRemoteCapability)
   {
   case RemoteReleaseControl::capChangeShootingParameter:
   case RemoteReleaseControl::capChangeShootingMode:
   case RemoteReleaseControl::capZoomControl:
   case RemoteReleaseControl::capViewfinder:
   case RemoteReleaseControl::capReleaseWhileViewfinder:
   case RemoteReleaseControl::capAFLock:
   case RemoteReleaseControl::capBulbMode:
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

unsigned int RemoteReleaseControlImpl::MapImagePropertyTypeToId(T_enImagePropertyType enImagePropertyType) const
{
   // TODO implement
   UNUSED(enImagePropertyType);
   return 0;
}

ImageProperty RemoteReleaseControlImpl::MapShootingModeToImagePropertyValue(T_enShootingMode enShootingMode) const
{
   // TODO implement
   UNUSED(enShootingMode);

   Variant value;
   value.Set<unsigned char>(42);
   value.SetType(Variant::typeUInt8);

   return ImageProperty(T_enSDKVariant::variantGphoto2, 0, value, true);
}

std::vector<unsigned int> RemoteReleaseControlImpl::EnumImageProperties() const
{
   // TODO implement
   return std::vector<unsigned int>();
}

ImageProperty RemoteReleaseControlImpl::GetImageProperty(unsigned int uiImagePropertyId) const
{
   // TODO implement
   Variant value;
   value.Set<unsigned char>(42);
   value.SetType(Variant::typeUInt8);

   return ImageProperty(T_enSDKVariant::variantGphoto2, uiImagePropertyId, value, true);
}

void RemoteReleaseControlImpl::SetImageProperty(const ImageProperty& imageProperty)
{
   // TODO implement
   UNUSED(imageProperty);
}

void RemoteReleaseControlImpl::EnumImagePropertyValues(unsigned int uiImagePropertyId, std::vector<ImageProperty>& vecValues) const
{
   // TODO implement
   UNUSED(uiImagePropertyId);
   vecValues.clear();
}

std::shared_ptr<Viewfinder> RemoteReleaseControlImpl::StartViewfinder() const
{
   throw CameraException(_T("gPhoto2::RemoteReleaseControl::StartViewfinder"),
      _T("Not supported"), 0, __FILE__, __LINE__);
}

unsigned int RemoteReleaseControlImpl::NumAvailableShots() const
{
   // TODO implement
   return 0;
}

void RemoteReleaseControlImpl::SendCommand(T_enCameraCommand enCameraCommand)
{
   // TODO implement
   UNUSED(enCameraCommand);
}

void RemoteReleaseControlImpl::Release()
{
   m_upReleaseThread->Post(std::bind(&RemoteReleaseControlImpl::AsyncRelease, this));
}

void RemoteReleaseControlImpl::AsyncRelease()
{
   // TODO implement
}

std::shared_ptr<BulbReleaseControl> RemoteReleaseControlImpl::StartBulb()
{
   // bulb not supported by gPhoto2
   throw CameraException(_T("gPhoto2::RemoteReleaseControl::StartBulb"),
      _T("Not supported"), 0, __FILE__, __LINE__);
}

void RemoteReleaseControlImpl::Close()
{
   // TODO implement
}
