//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file TimelapseVideoOptionsDlg.hpp Settings dialog
//
#pragma once

/// \brief timelapse video options dialog
/// \details shows some common ffmpeg options for selection and returns the
/// resulting command line string
class TimelapseVideoOptionsDlg :
   public CDialogImpl<TimelapseVideoOptionsDlg>,
   public CWinDataExchange<TimelapseVideoOptionsDlg>
{
public:
   /// ctor
   TimelapseVideoOptionsDlg(const CString& commandLine);

   /// dialog id
   enum { IDD = IDD_TIMELAPSE_VIDEO_OPTIONS };

   /// returns complete command line text that was configured using this dialog
   CString GetCommandLine() const;

private:
   BEGIN_DDX_MAP(TimelapseVideoOptionsDlg)
      DDX_INT(IDC_EDIT_TIMELAPSE_VIDEO_OPTIONS_FRAMERATE, m_framerate)
      DDX_CONTROL_HANDLE(IDC_EDIT_TIMELAPSE_VIDEO_OPTIONS_FRAMERATE, m_editFramerate)
      DDX_CONTROL_HANDLE(IDC_COMBO_TIMELAPSE_VIDEO_OPTIONS_FORMAT, m_comboVideoEncoding)
      DDX_CONTROL_HANDLE(IDC_COMBO_TIMELAPSE_VIDEO_OPTIONS_PRESET, m_comboVideoPreset)
      DDX_COMBO_INDEX(IDC_COMBO_TIMELAPSE_VIDEO_OPTIONS_FORMAT, m_indexVideoEncoding)
      DDX_COMBO_INDEX(IDC_COMBO_TIMELAPSE_VIDEO_OPTIONS_PRESET, m_indexVideoPreset)
      DDX_CONTROL_HANDLE(IDC_EDIT_TIMELAPSE_VIDEO_OPTIONS_EXTRA_FFMPEG_CMDLINE, m_editExtraOptions)
      DDX_TEXT(IDC_EDIT_TIMELAPSE_VIDEO_OPTIONS_EXTRA_FFMPEG_CMDLINE, m_extraFfmpegCmdline)
      DDX_TEXT(IDC_EDIT_TIMELAPSE_VIDEO_OPTIONS_COMPLETE_FFMPEG_CMDLINE, m_commandLine)
   END_DDX_MAP()

   BEGIN_MSG_MAP(TimelapseVideoOptionsDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
      COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
      COMMAND_HANDLER(IDC_EDIT_TIMELAPSE_VIDEO_OPTIONS_FRAMERATE, EN_CHANGE, OnInputControlChanged)
      COMMAND_HANDLER(IDC_EDIT_TIMELAPSE_VIDEO_OPTIONS_EXTRA_FFMPEG_CMDLINE, EN_CHANGE, OnInputControlChanged)
      COMMAND_HANDLER(IDC_COMBO_TIMELAPSE_VIDEO_OPTIONS_FORMAT, CBN_SELCHANGE, OnInputControlChanged)
      COMMAND_HANDLER(IDC_COMBO_TIMELAPSE_VIDEO_OPTIONS_PRESET, CBN_SELCHANGE, OnInputControlChanged)
   END_MSG_MAP()

   /// called when dialog is being shown
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when OK or Cancel button is pressed
   LRESULT OnCloseCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when an input control has changed its contents
   LRESULT OnInputControlChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// returns a complete command line, from reading the current UI control contents
   CString GetCommandLineFromControls();

   /// updates command line from UI controls
   void UpdateCommandLineFromControls();

   /// sets UI controls from given command line
   void SetControlsFromCommandLine(const CString& commandLine);

private:
   // model

   /// current command line string
   CString m_commandLine;

   /// contains selected framerate
   int m_framerate;

   /// contains index of currently selected video encoding option
   int m_indexVideoEncoding;

   /// contains index of currently selected video preset option
   int m_indexVideoPreset;

   /// contains extra options for the ffmpeg command line
   CString m_extraFfmpegCmdline;

   // UI

   /// indicates if the "change update" handler is currently running
   bool m_inChangeHandler;

   /// edit control containing the framerate
   CEdit m_editFramerate;

   /// combobox for selecting video encoding
   CComboBox m_comboVideoEncoding;

   /// combobox for selecting video preset
   CComboBox m_comboVideoPreset;

   /// edit control containing the extra options
   CEdit m_editExtraOptions;
};
