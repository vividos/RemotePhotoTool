//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file PhotomatixInterface.hpp Interface to Photomatix HDR image creation software
//
#pragma once

// includes
#include <vector>

/// interface to Photomatix application
class PhotomatixInterface
{
public:
   /// ctor
   PhotomatixInterface(const CString& cszInstallPath);

   // properties

   /// install path; ends with a trailing backslash
   CString InstallPath() const throw() { return m_cszInstallPath; }

   // methods

   /// returns if hugin is installed on this machine
   bool IsInstalled() const throw() { return !m_cszInstallPath.IsEmpty(); }

   /// runs hugin UI with given images
   void RunUI(const std::vector<CString>& vecImageFilenames);

private:
   /// detects photomatix install path
   void Detect();

private:
   /// hugin install path
   CString m_cszInstallPath;
};
