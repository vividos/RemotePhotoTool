//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file FfmpegInterface.hpp Interface to ffmpeg movie creation software
//
#pragma once

#include <vector>

/// interface to ffmpeg application
class FfmpegInterface
{
public:
   /// ctor
   FfmpegInterface(const CString& binPath);

   // properties

   /// bin path; ends with a trailing backslash
   const CString& BinPath() const { return m_binPath; }

   // methods

   /// returns if ffmpeg is installed on this machine
   bool IsInstalled() const;

   /// runs ffmpeg to create a move
   void Run(const std::vector<CString>& imageFilenamesList, const CString& extraCommandLine, const CString& outputFilename);

private:
   /// ffmpeg bin path
   CString m_binPath;
};
