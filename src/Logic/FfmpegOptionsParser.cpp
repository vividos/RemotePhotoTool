//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file FfmpegOptionsParser.cpp Parser for ffmpeg options
//
#include "stdafx.h"
#include "FfmpegOptionsParser.hpp"
#include <ulib/CommandLineParser.hpp>

FfmpegOptionsParser::FfmpegOptionsParser()
   :m_framerate(30)
{
}

void FfmpegOptionsParser::Parse(const CString& commandLine)
{
   CommandLineParser parser(commandLine);

   CString nextParam;
   while (parser.GetNext(nextParam))
   {
      if (nextParam == _T("-r"))
      {
         if (parser.GetNext(nextParam))
         {
            m_framerate = static_cast<unsigned int>(_ttoi(nextParam));
         }
      }
      else if (nextParam == _T("-c:v"))
      {
         if (parser.GetNext(nextParam))
         {
            m_videoEncoding = nextParam;
         }
      }
      else if (nextParam == _T("-pix_fmt"))
      {
         if (parser.GetNext(nextParam))
         {
            if (nextParam == _T("yuv420p"))
            {
               m_videoEncoding += _T(" -pix_fmt yuv420p");
            }
         }
      }
      else if (nextParam == _T("-preset"))
      {
         if (parser.GetNext(nextParam))
         {
            m_videoPreset = nextParam;
         }
      }
      else
      {
         m_extraOptions += nextParam;
         m_extraOptions += _T(" ");
      }
   }

   m_extraOptions.Replace(_T("  "), _T(" "));
   m_extraOptions.TrimRight(_T(' '));
}

CString FfmpegOptionsParser::Format() const
{
   CString commandLine;

   commandLine.Format(_T("-r %u "), m_framerate);
   commandLine.AppendFormat(_T("-c:v %s "), m_videoEncoding.GetString());
   commandLine.AppendFormat(_T("-preset %s "), m_videoPreset.GetString());
   commandLine.Append(m_extraOptions);

   return commandLine;
}
