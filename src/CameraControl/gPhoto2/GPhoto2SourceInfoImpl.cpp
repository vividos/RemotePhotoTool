//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2SourceInfoImpl.cpp gPhoto2 - SourceInfo impl
//
#pragma once

#include "stdafx.h"
#include "GPhoto2SourceInfoImpl.hpp"
#include "GPhoto2SourceDeviceImpl.hpp"
#include "GPhoto2PropertyAccess.hpp"
#include "GPhoto2Include.hpp"
#include "CameraException.hpp"

std::shared_ptr<SourceDevice> GPhoto2::SourceInfoImpl::Open()
{
   Camera* rawCamera = nullptr;
   gp_camera_new(&rawCamera);

   std::shared_ptr<_Camera> camera;
   camera.reset(rawCamera, gp_camera_free);

   InitCamera(camera);

   return std::make_shared<SourceDeviceImpl>(m_ref->GetContext(), camera);
}

void GPhoto2::SourceInfoImpl::InitCamera(std::shared_ptr<_Camera> camera)
{
   // First lookup the model / driver
   int modelIndex = gp_abilities_list_lookup_model(m_ref->GetCameraAbilitiesList().get(), CStringA(m_name));
   CheckError(_T("gp_abilities_list_lookup_model"), modelIndex, __FILE__, __LINE__);

   CameraAbilities abilities;
   int ret = gp_abilities_list_get_abilities(m_ref->GetCameraAbilitiesList().get(), modelIndex, &abilities);
   CheckError(_T("gp_abilities_list_get_abilities"), ret, __FILE__, __LINE__);

   ret = gp_camera_set_abilities(camera.get(), abilities);
   CheckError(_T("gp_camera_set_abilities"), ret, __FILE__, __LINE__);

   // Then associate the camera with the specified port
   int portIndex = gp_port_info_list_lookup_path(m_ref->GetPortInfoList().get(), CStringA(m_port));
   CheckError(_T("gp_port_info_list_lookup_path"), portIndex, __FILE__, __LINE__);

   GPPortInfo portInfo;
   ret = gp_port_info_list_get_info(m_ref->GetPortInfoList().get(), portIndex, &portInfo);
   CheckError(_T("gp_port_info_list_get_info"), ret, __FILE__, __LINE__);

   ret = gp_camera_set_port_info(camera.get(), portInfo);
   CheckError(_T("gp_camera_set_port_info"), ret, __FILE__, __LINE__);

   ret = gp_camera_init(camera.get(), m_ref->GetContext().get());
   CheckError(_T("gp_camera_init"), ret, __FILE__, __LINE__);
}
