//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file IPhotoModeView.hpp Interface for photo mode views
//
#pragma once

// includes
#include "SourceDevice.hpp"

/// Interface for photo mode views
class IPhotoModeView : public boost::noncopyable
{
public:
   /// dtor
   virtual ~IPhotoModeView() {}

   /// creates view
   virtual HWND CreateView(HWND hWndParent) = 0;

   /// translates message
   virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;

   /// checks if a view can be closed
   virtual bool CanClose() const { return true; }

   /// destroys view
   virtual void DestroyView() = 0;
};
