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

RemoteReleaseControlImpl::RemoteReleaseControlImpl(RefSp ref,
   std::shared_ptr<_Camera> camera)
   :m_ref(ref),
   m_camera(camera)
{
   //result = gp_camera_wait_for_event(gp_params.camera, waittime, type, &data, gp_params.context);
   //case GP_EVENT_CAPTURE_COMPLETE:
}

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

bool RemoteReleaseControlImpl::GetCapability(T_enRemoteCapability remoteCapability) const
{
   //result = gp_camera_get_abilities (gp_params.camera, &a);

   // TODO implement
   switch (remoteCapability)
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
   LightweightMutex::LockType lock(m_mutexShutterReleaseSettings);

   m_shutterReleaseSettings = settings;
}

unsigned int RemoteReleaseControlImpl::MapImagePropertyTypeToId(T_enImagePropertyType imagePropertyType) const
{
   // TODO implement
   UNUSED(imagePropertyType);
   return 0;
}

ImageProperty RemoteReleaseControlImpl::MapShootingModeToImagePropertyValue(T_enShootingMode shootingMode) const
{
   // TODO implement
   UNUSED(shootingMode);

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

ImageProperty RemoteReleaseControlImpl::GetImageProperty(unsigned int imagePropertyId) const
{
   //return m_spProperties->GetImageProperty(uiPropertyId);

   // TODO implement
   Variant value;
   value.Set<unsigned char>(42);
   value.SetType(Variant::typeUInt8);

   return ImageProperty(T_enSDKVariant::variantGphoto2, imagePropertyId, value, true);
}

void RemoteReleaseControlImpl::SetImageProperty(const ImageProperty& imageProperty)
{
   // TODO implement
   UNUSED(imageProperty);
}

void RemoteReleaseControlImpl::EnumImagePropertyValues(unsigned int imagePropertyId, std::vector<ImageProperty>& valuesList) const
{
   // TODO implement
   UNUSED(imagePropertyId);
   valuesList.clear();
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

void RemoteReleaseControlImpl::SendCommand(T_enCameraCommand cameraCommand)
{
   // TODO implement
   UNUSED(cameraCommand);
}

void RemoteReleaseControlImpl::Release()
{
   m_releaseThread->Post(std::bind(&RemoteReleaseControlImpl::AsyncRelease, this));
}

void RemoteReleaseControlImpl::AsyncRelease()
{
   // TODO implement
   //gp_camera_trigger_capture (gp_params.camera, gp_params.context);
}

std::shared_ptr<BulbReleaseControl> RemoteReleaseControlImpl::StartBulb()
{
   // result = set_config_action (&gp_params, "bulb", "1");

   // bulb not supported by gPhoto2
   throw CameraException(_T("gPhoto2::RemoteReleaseControl::StartBulb"),
      _T("Not supported"), 0, __FILE__, __LINE__);
}

void RemoteReleaseControlImpl::Close()
{
   // TODO implement
}
