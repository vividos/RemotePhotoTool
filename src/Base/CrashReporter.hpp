//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CrashReporter.hpp Application crash reporting
//
#pragma once

// includes

/// crash reporter helper class
class CrashReporter
{
public:
   /// sets up crash reporting
   static void Init(const CString& cszBasePath);
};
