//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file TimelapseVideoOptionsDlg.cpp Settings dialog
//
#include "stdafx.h"
#include "resource.h"
#include "TimelapseVideoOptionsDlg.hpp"
#include "FfmpegOptionsParser.hpp"

/// video encoding options for command line
static LPCTSTR c_videoEncodingOptions[] =
{
   _T("libx264 -pix_fmt yuv420p"),
};

/// display texts for video encoding options
static LPCTSTR c_videoEncodingDisplayText[] =
{
   _T("MP4 (H.264 Codec + YUV 4:2:0)"),
};

static_assert(
   sizeof(c_videoEncodingOptions) / sizeof(*c_videoEncodingOptions) ==
   sizeof(c_videoEncodingDisplayText) / sizeof(*c_videoEncodingDisplayText),
   "length of video encoding options and display text arrays must match");

/// video preset options for command line
static LPCTSTR c_videoPresetOptions[] =
{
   _T("ultrafast"),
   _T("medium"),
   _T("slow"),
   _T("veryslow"),
};

/// display texts for video preset options
static LPCTSTR c_videoPresetDisplayText[] =
{
   _T("Preset ultrafast (worst quality)"),
   _T("Preset medium (default)"),
   _T("Preset slow (good quality)"),
   _T("Preset veryslow (best quality)"),
};

static_assert(
   sizeof(c_videoPresetOptions) / sizeof(*c_videoPresetOptions) ==
   sizeof(c_videoPresetDisplayText) / sizeof(*c_videoPresetDisplayText),
   "length of video preset options and display text arrays must match");

TimelapseVideoOptionsDlg::TimelapseVideoOptionsDlg(const CString& commandLine)
   :m_commandLine(commandLine),
   m_framerate(30),
   m_indexVideoEncoding(0),
   m_indexVideoPreset(2),
   m_inChangeHandler(false)
{
}

CString TimelapseVideoOptionsDlg::GetCommandLine() const
{
   return m_commandLine;
}

LRESULT TimelapseVideoOptionsDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_inChangeHandler = true;

   DoDataExchange(DDX_LOAD);

   m_inChangeHandler = false;

   for (size_t index = 0; index < sizeof(c_videoEncodingDisplayText) / sizeof(*c_videoEncodingOptions); index++)
   {
      m_comboVideoEncoding.AddString(c_videoEncodingDisplayText[index]);
   }

   for (size_t index = 0; index < sizeof(c_videoPresetDisplayText) / sizeof(*c_videoPresetDisplayText); index++)
   {
      m_comboVideoPreset.AddString(c_videoPresetDisplayText[index]);
   }

   m_inChangeHandler = true;

   SetControlsFromCommandLine(m_commandLine);
   DoDataExchange(DDX_LOAD);

   m_inChangeHandler = false;

   return TRUE;
}

LRESULT TimelapseVideoOptionsDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (wID == IDOK)
   {
      m_commandLine = GetCommandLineFromControls();
   }

   EndDialog(wID);
   return 0;
}

LRESULT TimelapseVideoOptionsDlg::OnInputControlChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_inChangeHandler)
      return 0;

   m_inChangeHandler = true;

   UpdateCommandLineFromControls();

   m_inChangeHandler = false;

   return 0;
}

CString TimelapseVideoOptionsDlg::GetCommandLineFromControls()
{
   ATLASSERT(m_indexVideoEncoding >= 0);
   ATLASSERT(m_indexVideoPreset >= 0);

   FfmpegOptionsParser parser;
   parser.Framerate(m_framerate);
   parser.VideoEncoding(c_videoEncodingOptions[m_indexVideoEncoding]);
   parser.VideoPreset(c_videoPresetOptions[m_indexVideoPreset]);
   parser.ExtraOptions(m_extraFfmpegCmdline);

   return parser.Format();
}

void TimelapseVideoOptionsDlg::UpdateCommandLineFromControls()
{
   DWORD framerateSelection = m_editFramerate.GetSel();
   DWORD extraOptionsSelection = m_editExtraOptions.GetSel();

   DoDataExchange(DDX_SAVE);

   CString commandLine = GetCommandLineFromControls();

   SetControlsFromCommandLine(commandLine);

   m_commandLine = GetCommandLineFromControls();

   DoDataExchange(DDX_LOAD);

   m_editFramerate.SetSel(framerateSelection, TRUE);
   m_editExtraOptions.SetSel(extraOptionsSelection, TRUE);
}

void TimelapseVideoOptionsDlg::SetControlsFromCommandLine(const CString& commandLine)
{
   FfmpegOptionsParser parser;
   parser.Parse(commandLine);

   m_framerate = parser.Framerate();
   m_extraFfmpegCmdline = parser.ExtraOptions();

   for (size_t index = 0; index < sizeof(c_videoEncodingOptions) / sizeof(*c_videoEncodingOptions); index++)
   {
      if (c_videoEncodingOptions[index] == parser.VideoEncoding())
      {
         m_indexVideoEncoding = static_cast<int>(index);
         break;
      }
   }

   for (size_t index = 0; index < sizeof(c_videoPresetOptions) / sizeof(*c_videoPresetOptions); index++)
   {
      if (c_videoPresetOptions[index] == parser.VideoPreset())
      {
         m_indexVideoPreset = static_cast<int>(index);
         break;
      }
   }
}
