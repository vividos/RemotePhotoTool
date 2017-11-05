//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file HuginInterface.hpp Interface to Hugin panorama stitching software
//
#pragma once

// includes
#include <vector>
#include "ExternalApplicationInterface.hpp"

/// interface to hugin application
class HuginInterface : private ExternalApplicationInterface
{
public:
   /// ctor
   HuginInterface(const CString& cszInstallPath);

   // properties

   /// install path; ends with a trailing backslash
   CString InstallPath() const { return m_cszInstallPath; }

   // methods

   /// returns if hugin is installed on this machine
   bool IsInstalled() const { return !m_cszInstallPath.IsEmpty(); }

   /// runs hugin UI with given images
   void RunUI(const std::vector<CString>& imageFilenamesList);

   /// runs stitcher with given pto script and output file
   void RunStitcher(const CString& cszPtoScript, const CString& cszOutputFile);

private:
   /// detects hugin install path
   void Detect();

private:
   /// hugin install path
   CString m_cszInstallPath;
};
