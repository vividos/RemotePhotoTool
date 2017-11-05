//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file ExternalApplicationInterface.hpp Interface to external applications
//
#pragma once

#include <vector>

/// interface to external application; use as base class
class ExternalApplicationInterface
{
public:
   /// runs hugin UI with given images
   void RunApplication(const CString& applicationName, const CString& workingDirectory,
      const std::vector<CString>& imageFilenamesList);
};
