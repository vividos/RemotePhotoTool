//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ViewManager.hpp View manager
//
#pragma once

// includes
#include "IPhotoModeView.hpp"
#include <vector>

// forward references
class IPhotoModeViewHost;

/// view types
enum T_enViewType
{
   viewBlank = 0,
   viewStandard = 1,
   viewHDR = 2,
   viewPanorama = 3,
   viewHDRPanorama = 4,
   viewTimeLapse = 5,
   viewPhotoStacking = 6,
   viewScripting = 7,
   viewDeviceProperties = 8,
   viewImageProperties = 9,
   viewPreviousImages = 10,
};

/// view manager
class ViewManager
{
public:
   /// returns view by view index
   static std::unique_ptr<IPhotoModeView> CreateView(IPhotoModeViewHost& host, T_enViewType enViewType);

private:
   /// ctor
   ViewManager();
};
