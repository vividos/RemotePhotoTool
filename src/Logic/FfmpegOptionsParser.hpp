//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file FfmpegOptionsParser.cpp Parser for ffmpeg options
//
#pragma once

/// parser for ffmpeg options
class FfmpegOptionsParser
{
public:
   /// ctor; sets default parameters
   FfmpegOptionsParser();

   // getter

   unsigned int Framerate() const { return m_framerate; }      ///< returns framerate
   const CString& VideoEncoding() { return m_videoEncoding; }  ///< returns video encoding option
   const CString& VideoPreset() { return m_videoPreset; }      ///< returns video preset
   const CString& ExtraOptions() { return m_extraOptions; }    ///< returns extra options

   // setter

   void Framerate(unsigned int framerate) { m_framerate = framerate; }  ///< sets framerate
   void VideoEncoding(const CString& videoEncoding) { m_videoEncoding = videoEncoding; }  ///< sets video encoding option
   void VideoPreset(const CString& videoPreset) { m_videoPreset = videoPreset; }          ///< sets video preset
   void ExtraOptions(const CString& extraOptions) { m_extraOptions = extraOptions; }      ///< sets extra options

   // actions

   /// parse a command line string into the properties
   void Parse(const CString& commandLine);

   /// formats a command line string from the properties
   CString Format() const;

private:
   unsigned int m_framerate;  ///< framerate in fps
   CString m_videoEncoding;   ///< video encoding option
   CString m_videoPreset;     ///< video preset
   CString m_extraOptions;    ///< extra options
};
