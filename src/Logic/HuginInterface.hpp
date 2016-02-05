//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file HuginInterface.hpp Interface to Hugin panorama stitching software
//
#pragma once

// includes
#include <vector>

/// interface to hugin application
class HuginInterface
{
public:
   /// ctor
   HuginInterface(const CString& cszInstallPath);

   // properties

   /// install path; ends with a trailing backslash
   CString InstallPath() const throw() { return m_cszInstallPath; }

   // methods

   /// returns if hugin is installed on this machine
   bool IsInstalled() const throw() { return !m_cszInstallPath.IsEmpty(); }

   /// runs hugin UI with given images
   void RunUI(const std::vector<CString>& vecImageFilenames);

   /// runs stitcher with given pto script and output file
   void RunStitcher(const CString& cszPtoScript, const CString& cszOutputFile);

private:
   /// detects hugin install path
   void Detect();

private:
   /// hugin install path
   CString m_cszInstallPath;
};
