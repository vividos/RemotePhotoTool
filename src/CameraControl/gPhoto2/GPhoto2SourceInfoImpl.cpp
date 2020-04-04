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

/// taken from gPhoto2 example folder, autodetect.c
static int set_camera_port(Camera* camera, const char* model, const char* port, GPContext* context)
{
   static GPPortInfoList* portinfolist = nullptr;
   static CameraAbilitiesList* abilities = nullptr;

   if (!abilities)
   {
      // Load all the camera drivers we have...
      int ret = gp_abilities_list_new(&abilities);
      if (ret < GP_OK)
         return ret;

      ret = gp_abilities_list_load(abilities, context);
      if (ret < GP_OK)
         return ret;
   }

   // First lookup the model / driver
   int m = gp_abilities_list_lookup_model(abilities, model);
   if (m < GP_OK)
      return m;

   CameraAbilities a;
   {
      int ret = gp_abilities_list_get_abilities(abilities, m, &a);
      if (ret < GP_OK)
         return ret;

      ret = gp_camera_set_abilities(camera, a);
      if (ret < GP_OK)
         return ret;
   }

   if (!portinfolist)
   {
      // Load all the port drivers we have...
      int ret = gp_port_info_list_new(&portinfolist);
      if (ret < GP_OK)
         return ret;

      ret = gp_port_info_list_load(portinfolist);
      if (ret < 0)
         return ret;

      ret = gp_port_info_list_count(portinfolist);
      if (ret < 0)
         return ret;
   }

   // Then associate the camera with the specified port
   int p = gp_port_info_list_lookup_path(portinfolist, port);
   if (p < GP_OK)
      return p;

   GPPortInfo pi;
   int ret = gp_port_info_list_get_info(portinfolist, p, &pi);
   if (ret < GP_OK)
      return ret;

   ret = gp_camera_set_port_info(camera, pi);
   if (ret < GP_OK)
      return ret;

   return GP_OK;
}

std::shared_ptr<SourceDevice> GPhoto2::SourceInfoImpl::Open()
{
   Camera* rawCamera = nullptr;
   gp_camera_new(&rawCamera);

   std::shared_ptr<_Camera> camera;
   camera.reset(rawCamera, gp_camera_free);

   int ret = set_camera_port(camera.get(), CStringA(m_name), CStringA(m_port), m_context.get());
   CheckError(_T("set_camera_port"), ret, __FILE__, __LINE__);

   ret = gp_camera_init(camera.get(), m_context.get());
   CheckError(_T("gp_camera_init"), ret, __FILE__, __LINE__);

   return std::make_shared<SourceDeviceImpl>(m_context, camera);
}
