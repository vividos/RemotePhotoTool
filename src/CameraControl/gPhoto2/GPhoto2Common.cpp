//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2Common.cpp gPhoto2 - Common functions
//
#include "stdafx.h"
#include "GPhoto2Common.hpp"
#include "GPhoto2SourceInfoImpl.hpp"
#include "GPhoto2Include.hpp"

void GPhoto2::CheckError(const CString& function, int errorCode, LPCSTR filename, UINT lineNumber)
{
   if (errorCode >= GP_OK)
      return;

   const char* errorText = gp_result_as_string(errorCode);

   LOG_TRACE(_T("gPhoto2: error in function %s: %hs, return code %i\n"), function.GetString(), errorText, errorCode);

   throw CameraException(function, CString(errorText), static_cast<unsigned int>(errorCode), filename, lineNumber);
}

/// context error handler function
static void ctx_error_func(GPContext* context, const char* str, void* data)
{
   LOG_TRACE(_T("gPhoto2 error: ctx=%p, text=%hs, data=%p\n"), context, str, data);
}

/// context status handler function
static void ctx_status_func(GPContext* context, const char* str, void* data)
{
   LOG_TRACE(_T("gPhoto2 status: ctx=%p, text=%hs, data=%p\n"), context, str, data);
}

/// context message handler function
static void ctx_message_func(GPContext* context, const char* str, void* data)
{
   LOG_TRACE(_T("gPhoto2 message: ctx=%p, text=%hs, data=%p\n"), context, str, data);
}

GPhoto2::Ref::Ref()
{
   GPContext* rawContext = gp_context_new();
   m_context.reset(rawContext, gp_context_unref);

   gp_context_set_error_func(rawContext, ctx_error_func, nullptr);
   gp_context_set_status_func(rawContext, ctx_status_func, nullptr);
   gp_context_set_message_func(rawContext, ctx_message_func, nullptr);

   InitCameraAbilitiesList();
   InitPortInfoList();
}

bool GPhoto2::Ref::InitCameraAbilitiesList()
{
   // Load all the camera drivers we have...
   _CameraAbilitiesList* rawAbilities = nullptr;
   int ret = gp_abilities_list_new(&rawAbilities);
   if (ret < GP_OK || rawAbilities == nullptr)
      return false;

   m_abilitiesList.reset(rawAbilities, gp_abilities_list_free);

   ret = gp_abilities_list_load(m_abilitiesList.get(), m_context.get());
   if (ret < GP_OK)
      return false;

   return true;
}

bool GPhoto2::Ref::InitPortInfoList()
{
   // Load all the port drivers we have...
   _GPPortInfoList* rawPortInfoList = nullptr;

   int ret = gp_port_info_list_new(&rawPortInfoList);
   if (ret < GP_OK || rawPortInfoList == nullptr)
      return false;

   m_portInfoList.reset(rawPortInfoList, gp_port_info_list_free);

   ret = gp_port_info_list_load(m_portInfoList.get());
   if (ret < 0)
      return false;

   int count = gp_port_info_list_count(m_portInfoList.get());
   if (count < 0)
      return false;

   return true;
}

GPhoto2::Ref::~Ref()
{
}

void GPhoto2::Ref::AddVersionText(CString& versionText) const
{
   const char** version = gp_library_version(GP_VERSION_SHORT);

   versionText.AppendFormat(_T("gPhoto2 %hs\n\n"), version[0]);
}

void GPhoto2::Ref::EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& sourceDevicesList) const
{
   CameraList* cameraList = nullptr;

   int ret = gp_list_new(&cameraList);
   if (ret < GP_OK)
      return;

   std::shared_ptr<CameraList> spAutoFreeCameraList(cameraList, gp_list_free);

   ret = gp_camera_autodetect(cameraList, m_context.get());
   if (ret < GP_OK)
   {
      const char* errorText = gp_result_as_string(ret);

      LOG_TRACE(_T("gPhoto2: no camera auto detected: %hs\n"), errorText);
      return;
   }

   for (int index = 0, max = ret; index < max; index++)
   {
      const char* name = nullptr;
      const char* port = nullptr;

      gp_list_get_name(cameraList, index, &name);
      gp_list_get_value(cameraList, index, &port);

      LOG_TRACE(_T("gPhoto2 camera #%i: %hs (%hs)\n"), index, name, port);

      RefSp ref = const_cast<Ref*>(this)->shared_from_this();
      sourceDevicesList.push_back(std::make_shared<GPhoto2::SourceInfoImpl>(ref, name, port));
   }
}
