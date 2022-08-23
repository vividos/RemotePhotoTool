//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2RemoteReleaseControlImpl.cpp gPhoto2 - Remote release control impl
//
#include "stdafx.h"
#include "GPhoto2RemoteReleaseControlImpl.hpp"
#include "GPhoto2PropertyAccess.hpp"
#include "CameraException.hpp"
#include "SingleThreadExecutor.hpp"
#include "GPhoto2ViewfinderImpl.hpp"
#include "GPhoto2BulbReleaseControlImpl.hpp"
#include "GPhoto2Include.hpp"

using GPhoto2::RemoteReleaseControlImpl;

RemoteReleaseControlImpl::RemoteReleaseControlImpl(RefSp ref,
   std::shared_ptr<_Camera> camera,
   std::shared_ptr<PropertyAccess> properties)
   :m_ref(ref),
   m_camera(camera),
   m_properties(properties),
   m_releaseThread(std::make_unique<SingleThreadExecutor>())
{
   Variant value;
   value.Set(true);
   value.SetType(Variant::typeBool);

   properties->SetPropertyByName(_T("capture"), value);

   properties->Refresh();

   // TODO start event listener
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
   switch (remoteCapability)
   {
   case RemoteReleaseControl::capChangeShootingParameter:
   case RemoteReleaseControl::capChangeShootingMode:
   case RemoteReleaseControl::capZoomControl:
      return m_properties->GetCameraOperationAbility(GP_OPERATION_CONFIG);

   case RemoteReleaseControl::capViewfinder:
   case RemoteReleaseControl::capReleaseWhileViewfinder:
      return m_properties->GetCameraOperationAbility(GP_OPERATION_CAPTURE_PREVIEW);

   case RemoteReleaseControl::capAFLock:
   case RemoteReleaseControl::capBulbMode: // TODO check for bulb shooting mode property?
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
   return m_properties->MapImagePropertyTypeToId(imagePropertyType);
}

ImageProperty RemoteReleaseControlImpl::MapShootingModeToImagePropertyValue(T_enShootingMode shootingMode) const
{
   return m_properties->MapShootingModeToImagePropertyValue(shootingMode);
}

std::vector<unsigned int> RemoteReleaseControlImpl::EnumImageProperties() const
{
   return m_properties->EnumImageProperties();
}

ImageProperty RemoteReleaseControlImpl::GetImageProperty(unsigned int imagePropertyId) const
{
   return m_properties->GetImageProperty(imagePropertyId);
}

void RemoteReleaseControlImpl::SetImageProperty(const ImageProperty& imageProperty)
{
   m_properties->SetPropertyById(imageProperty.Id(), imageProperty.Value());
}

void RemoteReleaseControlImpl::EnumImagePropertyValues(unsigned int imagePropertyId, std::vector<ImageProperty>& valuesList) const
{
   valuesList = m_properties->EnumImagePropertyValues(imagePropertyId);
}

std::shared_ptr<Viewfinder> RemoteReleaseControlImpl::StartViewfinder() const
{
   if (!GetCapability(RemoteReleaseControl::capViewfinder))
      throw CameraException(_T("gPhoto2::RemoteReleaseControl::StartViewfinder"),
         _T("Not supported"), 0, __FILE__, __LINE__);

   return std::make_shared<ViewfinderImpl>(m_ref, m_camera, m_properties, *m_releaseThread);
}

unsigned int RemoteReleaseControlImpl::NumAvailableShots() const
{
   unsigned int propertyId = MapImagePropertyTypeToId(T_enImagePropertyType::propAvailableShots);

   if (propertyId == static_cast<unsigned int>(-1))
      return 0;

   ImageProperty prop = m_properties->GetImageProperty(propertyId);
   return prop.Value().Get<int>();
}

void RemoteReleaseControlImpl::SendCommand(T_enCameraCommand cameraCommand)
{
   // TODO implement
   UNUSED(cameraCommand);
}

void RemoteReleaseControlImpl::Release()
{
   m_releaseThread->Schedule(std::bind(&RemoteReleaseControlImpl::AsyncRelease, this));
}

void RemoteReleaseControlImpl::AsyncRelease()
{
   int ret = gp_camera_trigger_capture(m_camera.get(), m_ref->GetContext().get());
   CheckError(_T("gp_camera_trigger_capture"), ret, __FILE__, __LINE__);
}

std::shared_ptr<BulbReleaseControl> RemoteReleaseControlImpl::StartBulb()
{
   if (!GetCapability(T_enRemoteCapability::capBulbMode))
      throw CameraException(_T("gPhoto2::RemoteReleaseControl::StartBulb"),
         _T("Not supported"), 0, __FILE__, __LINE__);

   return std::make_shared<BulbReleaseControlImpl>(m_ref, m_properties);
}

void RemoteReleaseControlImpl::Close()
{
   Variant value;
   value.Set(false);
   value.SetType(Variant::typeBool);

   m_properties->SetPropertyByName(_T("capture"), value);
}
