//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.ShutterReleaseSettings.cpp ShutterReleaseSettings wrapper class
//

// includes
#include "stdafx.h"
#include "RemotePhotoTool.CameraControl.ShutterReleaseSettings.hpp"
#include "ShutterReleaseSettings.hpp"


System::String^ RemotePhotoTool::CameraControl::ShutterReleaseSettings::Filename::get()
{
   return gcnew System::String(m_shutterReleaseSettings->get()->Filename());
}

void RemotePhotoTool::CameraControl::ShutterReleaseSettings::Filename::set(System::String^ value)
{
   m_shutterReleaseSettings->get()->Filename(CString(value));
}

RemotePhotoTool::CameraControl::ShutterReleaseSettings::SaveTarget
RemotePhotoTool::CameraControl::ShutterReleaseSettings::Target::get()
{
   return static_cast<RemotePhotoTool::CameraControl::ShutterReleaseSettings::SaveTarget>(
      m_shutterReleaseSettings->get()->SaveTarget());
}

void RemotePhotoTool::CameraControl::ShutterReleaseSettings::Target::set(
   RemotePhotoTool::CameraControl::ShutterReleaseSettings::SaveTarget value)
{
   m_shutterReleaseSettings->get()->SaveTarget(
      static_cast<::ShutterReleaseSettings::T_enSaveTarget>(value));
}

RemotePhotoTool::CameraControl::ShutterReleaseSettings::FinishedTransferHandler^
RemotePhotoTool::CameraControl::ShutterReleaseSettings::HandlerOnFinishedTransfer::get()
{
   // TODO implement
   return nullptr;
}

static void OnFinishedTransfer(
   msclr::gcroot<RemotePhotoTool::CameraControl::ShutterReleaseSettings::FinishedTransferHandler^> handler,
   const ::ShutterReleaseSettings& settings)
{
   RemotePhotoTool::CameraControl::ShutterReleaseSettings::FinishedTransferHandler^ callback = handler;
   callback(gcnew RemotePhotoTool::CameraControl::ShutterReleaseSettings(settings));
}

void RemotePhotoTool::CameraControl::ShutterReleaseSettings::HandlerOnFinishedTransfer::set(
   FinishedTransferHandler^ value)
{
   msclr::gcroot<FinishedTransferHandler^> callback(value);

   ::ShutterReleaseSettings::T_fnOnFinishedTransfer fn =
      std::bind(&OnFinishedTransfer, callback, std::placeholders::_1);

   m_shutterReleaseSettings->get()->HandlerOnFinishedTransfer(fn);
}

RemotePhotoTool::CameraControl::ShutterReleaseSettings::ShutterReleaseSettings(
   RemotePhotoTool::CameraControl::ShutterReleaseSettings::SaveTarget saveTarget,
   FinishedTransferHandler^ finishedTransferHandler)
:m_shutterReleaseSettings(new std::shared_ptr<::ShutterReleaseSettings>(
   new ::ShutterReleaseSettings))
{
   Target = saveTarget;
   HandlerOnFinishedTransfer = finishedTransferHandler;
}

/// cleans up unmanaged resources
RemotePhotoTool::CameraControl::ShutterReleaseSettings::!ShutterReleaseSettings()
{
   delete m_shutterReleaseSettings;
   m_shutterReleaseSettings = nullptr;
}

/// cleans up managed resources
RemotePhotoTool::CameraControl::ShutterReleaseSettings::~ShutterReleaseSettings()
{
}

::ShutterReleaseSettings* RemotePhotoTool::CameraControl::ShutterReleaseSettings::GetRaw()
{
   return m_shutterReleaseSettings->get();
}

RemotePhotoTool::CameraControl::ShutterReleaseSettings::ShutterReleaseSettings(
   const ::ShutterReleaseSettings& ShutterReleaseSettings)
:m_shutterReleaseSettings(new std::shared_ptr<::ShutterReleaseSettings>(new ::ShutterReleaseSettings(ShutterReleaseSettings)))
{
}
