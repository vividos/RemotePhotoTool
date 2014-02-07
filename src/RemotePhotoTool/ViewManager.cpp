//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ViewManager.cpp View manager
//

// includes
#include "StdAfx.h"
#include "ViewManager.hpp"
#include "resource.h"
#include "StandardPhotoModeView.hpp"
#include "HDRPhotoModeView.hpp"
#include "PanoramaPhotoModeView.hpp"
#include "DevicePropertyView.hpp"
#include "ImagePropertyView.hpp"

std::unique_ptr<IPhotoModeView> ViewManager::CreateView(IPhotoModeViewHost& host, T_enViewType enViewType)
{
   switch (enViewType)
   {
   case viewStandard:   return std::unique_ptr<IPhotoModeView>(new StandardPhotoModeView(host));
   case viewHDR:        return std::unique_ptr<IPhotoModeView>(new HDRPhotoModeView(host));
   case viewPanorama:   return std::unique_ptr<IPhotoModeView>(new PanoramaPhotoModeView(host));
   case viewDeviceProperties: return std::unique_ptr<IPhotoModeView>(new DevicePropertyView(host));
   case viewImageProperties:  return std::unique_ptr<IPhotoModeView>(new ImagePropertyView(host));

   default:
      ATLASSERT(false);
      break;
   }

   return std::unique_ptr<IPhotoModeView>();
}
