//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file SourceInfo.hpp Canon control - Source info
//
#pragma once

// includes

// forward references
class SourceDevice;

/// infos about a source camera
class SourceInfo
{
public:
   /// dtor
   virtual ~SourceInfo() throw() {}

   /// returns name of source camera
   virtual CString Name() const = 0;

   /// opens source device for use
   virtual std::shared_ptr<SourceDevice> Open() = 0;
};
