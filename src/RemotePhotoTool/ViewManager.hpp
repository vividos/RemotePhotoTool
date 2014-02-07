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
#include <memory> // for std::auto_ptr

// forward references
class IPhotoModeViewHost;

/// view types
enum T_enViewType
{
   viewStandard = 0,
   viewHDR = 1,
   viewPanorama = 2,
   viewHDRPanorama = 3,
   viewTimeLapse = 4,
   viewPhotoStacking = 5,
   viewScripting = 6,
   viewDeviceProperties = 7,
   viewImageProperties = 8,
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
