//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.RemoteReleaseControl.cpp RemoteReleaseControl wrapper class
//

// includes
#include "stdafx.h"
#include "RemotePhotoTool.CameraControl.RemoteReleaseControl.hpp"
#include "RemotePhotoTool.CameraControl.ShutterReleaseSettings.hpp"
#include "RemotePhotoTool.CameraControl.ImagePropertyType.hpp"
#include "RemotePhotoTool.CameraControl.ImageProperty.hpp"
#include "RemotePhotoTool.CameraControl.Viewfinder.hpp"
#include "RemotePhotoTool.CameraControl.BulbReleaseControl.hpp"
#include "RemoteReleaseControl.hpp"

bool RemotePhotoTool::CameraControl::RemoteReleaseControl::GetCapability(RemoteCapability remoteCapability)
{
   return m_remoteReleaseControl->get()->GetCapability(
      static_cast<::RemoteReleaseControl::T_enRemoteCapability>(remoteCapability));
}

void RemotePhotoTool::CameraControl::RemoteReleaseControl::SetReleaseSettings(ShutterReleaseSettings^ settings)
{
   ::ShutterReleaseSettings* releaseSettings = settings->GetRaw();

   m_remoteReleaseControl->get()->SetReleaseSettings(*releaseSettings);
}

RemotePhotoTool::CameraControl::ImageProperty^
RemotePhotoTool::CameraControl::RemoteReleaseControl::GetImagePropertyByType(ImagePropertyType imagePropertyType)
{
   T_enImagePropertyType enImagePropertyType =
      static_cast<T_enImagePropertyType>(imagePropertyType);

   unsigned int imagePropertyId = m_remoteReleaseControl->get()->MapImagePropertyTypeToId(enImagePropertyType);

   ::ImageProperty imageProperty = m_remoteReleaseControl->get()->GetImageProperty(imagePropertyId);

   return gcnew RemotePhotoTool::CameraControl::ImageProperty(imageProperty);
}

RemotePhotoTool::CameraControl::ImageProperty^
RemotePhotoTool::CameraControl::RemoteReleaseControl::MapShootingModeToImagePropertyValue(ShootingMode shootingMode)
{
   ::ImageProperty imageProperty = m_remoteReleaseControl->get()->MapShootingModeToImagePropertyValue(
      static_cast<::RemoteReleaseControl::T_enShootingMode>(shootingMode));

   return gcnew RemotePhotoTool::CameraControl::ImageProperty(imageProperty);
}

System::Collections::Generic::List<RemotePhotoTool::CameraControl::ImageProperty^>^
RemotePhotoTool::CameraControl::RemoteReleaseControl::EnumImageProperties()
{
   std::vector<unsigned int> imagePropertyList = m_remoteReleaseControl->get()->EnumImageProperties();

   auto imagePropertyCollection = gcnew System::Collections::Generic::List<RemotePhotoTool::CameraControl::ImageProperty^>();

   for (size_t index = 0, max = imagePropertyList.size(); index < max; index++)
   {
      unsigned int imagePropertyId = imagePropertyList[index];

      ::ImageProperty imageProperty = m_remoteReleaseControl->get()->GetImageProperty(imagePropertyId);

      imagePropertyCollection->Add(gcnew RemotePhotoTool::CameraControl::ImageProperty(imageProperty));
   }

   return imagePropertyCollection;
}

void RemotePhotoTool::CameraControl::RemoteReleaseControl::SetImageProperty(ImageProperty^ imageProperty)
{
   m_remoteReleaseControl->get()->SetImageProperty(
      *imageProperty->GetRaw());
}

RemotePhotoTool::CameraControl::Viewfinder^
RemotePhotoTool::CameraControl::RemoteReleaseControl::StartViewfinder()
{
   std::shared_ptr<::Viewfinder> spViewfinder =
      m_remoteReleaseControl->get()->StartViewfinder();

   return gcnew RemotePhotoTool::CameraControl::Viewfinder(spViewfinder);
}

unsigned int RemotePhotoTool::CameraControl::RemoteReleaseControl::NumAvailableShots::get()
{
   return m_remoteReleaseControl->get()->NumAvailableShots();
}

void RemotePhotoTool::CameraControl::RemoteReleaseControl::SendCommand(CameraCommand cameraCommand)
{
   m_remoteReleaseControl->get()->SendCommand(
      static_cast<::RemoteReleaseControl::T_enCameraCommand>(cameraCommand));
}

void RemotePhotoTool::CameraControl::RemoteReleaseControl::Release()
{
   m_remoteReleaseControl->get()->Release();
}

RemotePhotoTool::CameraControl::BulbReleaseControl^
RemotePhotoTool::CameraControl::RemoteReleaseControl::StartBulb()
{
   std::shared_ptr<::BulbReleaseControl> spBulbReleaseControl =
      m_remoteReleaseControl->get()->StartBulb();

   return gcnew RemotePhotoTool::CameraControl::BulbReleaseControl(spBulbReleaseControl);
}

void RemotePhotoTool::CameraControl::RemoteReleaseControl::Close()
{
   m_remoteReleaseControl->get()->Close();
   m_remoteReleaseControl->reset();
}

/// cleans up unmanaged resources
RemotePhotoTool::CameraControl::RemoteReleaseControl::!RemoteReleaseControl()
{
   if (m_remoteReleaseControl != nullptr)
   {
      m_remoteReleaseControl->get()->RemovePropertyEventHandler(m_propertyEventHandlerId);
      m_remoteReleaseControl->get()->RemoveStateEventHandler(m_stateEventHandlerId);
      m_remoteReleaseControl->get()->RemoveDownloadEventHandler(m_downloadEventHandlerId);
   }

   delete m_remoteReleaseControl;
   m_remoteReleaseControl = nullptr;
}

/// cleans up managed resources
RemotePhotoTool::CameraControl::RemoteReleaseControl::~RemoteReleaseControl()
{
}

RemotePhotoTool::CameraControl::RemoteReleaseControl::RemoteReleaseControl(
   std::shared_ptr<::RemoteReleaseControl> remoteReleaseControl)
   :m_remoteReleaseControl(new std::shared_ptr<::RemoteReleaseControl>(remoteReleaseControl)),
   m_propertyEventHandlerId(-1),
   m_stateEventHandlerId(-1),
   m_downloadEventHandlerId(-1)
{
}

void OnPropertyChanged(
   msclr::gcroot<RemotePhotoTool::CameraControl::RemoteReleaseControl::PropertyChangedHandler^> handler,
   ::RemoteReleaseControl::T_enPropertyEvent propertyEvent,
   unsigned int propertyId)
{
   RemotePhotoTool::CameraControl::RemoteReleaseControl::PropertyChangedHandler^ callback = handler;

   callback(
      static_cast<RemotePhotoTool::CameraControl::RemoteReleaseControl::PropertyEvent>(propertyEvent),
      propertyId);
}

void RemotePhotoTool::CameraControl::RemoteReleaseControl::OnPropertyChanged(PropertyEvent propertyEvent,
   unsigned int propertyId)
{
   PropertyChanged(propertyEvent, propertyId);
}

void OnStateChanged(
   msclr::gcroot<RemotePhotoTool::CameraControl::RemoteReleaseControl::StateChangedHandler^> handler,
   ::RemoteReleaseControl::T_enStateEvent StateEvent,
   unsigned int stateId)
{
   RemotePhotoTool::CameraControl::RemoteReleaseControl::StateChangedHandler^ callback = handler;

   callback(
      static_cast<RemotePhotoTool::CameraControl::RemoteReleaseControl::StateEvent>(StateEvent),
      stateId);
}

void RemotePhotoTool::CameraControl::RemoteReleaseControl::OnStateChanged(StateEvent StateEvent,
   unsigned int stateId)
{
   StateChanged(StateEvent, stateId);
}

void OnDownloadChanged(
   msclr::gcroot<RemotePhotoTool::CameraControl::RemoteReleaseControl::DownloadChangedHandler^> handler,
   ::RemoteReleaseControl::T_enDownloadEvent DownloadEvent,
   unsigned int progressPercent)
{
   RemotePhotoTool::CameraControl::RemoteReleaseControl::DownloadChangedHandler^ callback = handler;

   callback(
      static_cast<RemotePhotoTool::CameraControl::RemoteReleaseControl::DownloadEvent>(DownloadEvent),
      progressPercent);
}

void RemotePhotoTool::CameraControl::RemoteReleaseControl::OnDownloadChanged(DownloadEvent DownloadEvent,
   unsigned int progressPercent)
{
   DownloadChanged(DownloadEvent, progressPercent);
}

void RemotePhotoTool::CameraControl::RemoteReleaseControl::InitEventHandler()
{
   // property events

   PropertyChangedHandler^ propertyChangedHandler =
      gcnew PropertyChangedHandler(this, &RemotePhotoTool::CameraControl::RemoteReleaseControl::OnPropertyChanged);

   msclr::gcroot<PropertyChangedHandler^> propertyChangedCallback(propertyChangedHandler);

   m_propertyEventHandlerId =
      m_remoteReleaseControl->get()->AddPropertyEventHandler(
         std::bind(&::OnPropertyChanged, propertyChangedCallback, std::placeholders::_1, std::placeholders::_2));

   // state events

   StateChangedHandler^ stateChangedHandler =
      gcnew StateChangedHandler(this, &RemotePhotoTool::CameraControl::RemoteReleaseControl::OnStateChanged);

   msclr::gcroot<StateChangedHandler^> stateChangedCallback(stateChangedHandler);

   m_stateEventHandlerId =
      m_remoteReleaseControl->get()->AddStateEventHandler(
         std::bind(&::OnStateChanged, stateChangedCallback, std::placeholders::_1, std::placeholders::_2));

   // download events

   DownloadChangedHandler^ downloadChangedHandler =
      gcnew DownloadChangedHandler(this, &RemotePhotoTool::CameraControl::RemoteReleaseControl::OnDownloadChanged);

   msclr::gcroot<DownloadChangedHandler^> downloadChangedCallback(downloadChangedHandler);

   m_downloadEventHandlerId =
      m_remoteReleaseControl->get()->AddDownloadEventHandler(
         std::bind(&::OnDownloadChanged, downloadChangedCallback, std::placeholders::_1, std::placeholders::_2));
}
