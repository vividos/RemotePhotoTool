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
#include "BlankView.hpp"
#include "StandardPhotoModeView.hpp"
#include "HDRPhotoModeView.hpp"
#include "PanoramaPhotoModeView.hpp"
#include "HDRPanoramaPhotoModeView.hpp"
#include "TimeLapsePhotoModeView.hpp"
#include "PhotoStackingPhotoModeView.hpp"
#include "ScriptingPhotoModeView.hpp"
#include "DevicePropertyView.hpp"
#include "ImagePropertyView.hpp"
#include "PreviousImagesView.hpp"
#include "CameraFileSystemView.hpp"

std::unique_ptr<IPhotoModeView> ViewManager::CreateView(IPhotoModeViewHost& host, T_enViewType enViewType)
{
   switch (enViewType)
   {
   case viewBlank:      return std::unique_ptr<IPhotoModeView>(new BlankView);
   case viewStandard:   return std::unique_ptr<IPhotoModeView>(new StandardPhotoModeView(host));
   case viewHDR:        return std::unique_ptr<IPhotoModeView>(new HDRPhotoModeView(host));
   case viewPanorama:   return std::unique_ptr<IPhotoModeView>(new PanoramaPhotoModeView(host));
   case viewHDRPanorama:   return std::unique_ptr<IPhotoModeView>(new HDRPanoramaPhotoModeView(host));
   case viewTimeLapse:     return std::unique_ptr<IPhotoModeView>(new TimeLapsePhotoModeView(host));
   case viewPhotoStacking:    return std::unique_ptr<IPhotoModeView>(new PhotoStackingPhotoModeView(host));
   case viewScripting:        return std::unique_ptr<IPhotoModeView>(new ScriptingPhotoModeView(host));
   case viewDeviceProperties: return std::unique_ptr<IPhotoModeView>(new DevicePropertyView(host));
   case viewImageProperties:  return std::unique_ptr<IPhotoModeView>(new ImagePropertyView(host));
   case viewPreviousImages:   return std::unique_ptr<IPhotoModeView>(new PreviousImagesView(host));
   case viewCameraFileSystem: return std::unique_ptr<IPhotoModeView>(new CameraFileSystemView(host));

   default:
      ATLASSERT(false);
      break;
   }

   return std::unique_ptr<IPhotoModeView>();
}
