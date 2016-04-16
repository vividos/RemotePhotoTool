//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file GPhoto2Common.cpp gPhoto2 - Common functions
//

// includes
#include "stdafx.h"
#include "GPhoto2Common.hpp"
#include "GPhoto2SourceInfoImpl.hpp"
#include "GPhoto2Include.hpp"

void GPhoto2::CheckError(const CString& cszFunction, int err, LPCSTR pszFile, UINT uiLine)
{
   if (err >= GP_OK)
      return;

   const char* errorText = gp_result_as_string(err);

   LOG_TRACE(_T("gPhoto2: error in function %s: %hs, return code %i\n"), cszFunction.GetString(), errorText, err);

   throw CameraException(cszFunction, CString(errorText), static_cast<unsigned int>(err), pszFile, uiLine);
}

/// context error handler function
static void ctx_error_func(GPContext* context, const char* str, void* /*data*/)
{
   LOG_TRACE(_T("gPhoto2 error: ctx=%p, text=%hs\n"), context, str);
}

/// context status handler function
static void ctx_status_func(GPContext* context, const char* str, void* /*data*/)
{
   LOG_TRACE(_T("gPhoto2 status: ctx=%p, text=%hs\n"), context, str);
}

/// context message handler function
static void ctx_message_func(GPContext* context, const char* str, void* /*data*/)
{
   LOG_TRACE(_T("gPhoto2 message: ctx=%p, text=%hs\n"), context, str);
}

GPhoto2::Ref::Ref()
{
   GPContext* context = gp_context_new();
   m_spContext.reset(context, gp_context_unref);

   gp_context_set_error_func(context, ctx_error_func, nullptr);
   gp_context_set_status_func(context, ctx_status_func, nullptr);
   gp_context_set_message_func(context, ctx_message_func, nullptr);
}

GPhoto2::Ref::~Ref() throw()
{
}

void GPhoto2::Ref::AddVersionText(CString& cszVersionText) const
{
   const char** ppVersion = gp_library_version(GP_VERSION_SHORT);

   cszVersionText.AppendFormat(_T("gPhoto2 %hs\n\n"), ppVersion[0]);
}

void GPhoto2::Ref::EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& vecSourceDevices) const
{
   CameraList* cameraList = nullptr;

   int ret = gp_list_new(&cameraList);
   if (ret < GP_OK)
      return;

   std::shared_ptr<CameraList> spAutoFreeCameraList(cameraList, gp_list_free);

   ret = gp_camera_autodetect(cameraList, m_spContext.get());
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

      vecSourceDevices.push_back(std::make_shared<GPhoto2::SourceInfoImpl>(m_spContext, name, port));
   }
}
