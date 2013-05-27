//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file IPhotoModeView.hpp Interface for photo mode views
//
#pragma once

// includes
#include "SourceDevice.hpp"

/// Interface for photo mode views
class IPhotoModeView
{
public:
   /// dtor
   virtual ~IPhotoModeView() throw() {}

   /// creates view
   virtual HWND CreateView(HWND hWndParent) = 0;

   /// sets source device
   virtual void SetSourceDevice(std::shared_ptr<SourceDevice> spSourceDevice) = 0;

   /// translates message
   virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;

   /// destroys view
   virtual void DestroyView() = 0;
};
