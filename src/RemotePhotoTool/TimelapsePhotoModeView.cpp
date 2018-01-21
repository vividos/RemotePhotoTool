//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file TimeLapsePhotoModeView.cpp Timelapse photo mode view
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "TimeLapsePhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"

TimeLapsePhotoModeView::TimeLapsePhotoModeView(IPhotoModeViewHost& host)
   :m_host(host),
   m_manager(host, m_hWnd),
   m_releaseTriggerRadio(0),
   m_scheduleIsCheckedStartTime(false),
   m_scheduleIsCheckedEndTime(false)
{
}

bool TimeLapsePhotoModeView::CanClose() const
{
   if (!m_manager.IsStarted())
      return true;

   int iRet = AtlMessageBox(m_hWnd, _T("A time lapse shooting is in progress. Do you want to abort the time lapse?"),
      IDR_MAINFRAME, MB_YESNO | MB_ICONQUESTION);

   if (iRet == IDNO)
      return false;

   return true;
}

LRESULT TimeLapsePhotoModeView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   m_spRemoteReleaseControl = m_host.GetRemoteReleaseControl();

   if (!m_manager.Init(m_spRemoteReleaseControl))
      return FALSE;

   m_btnStop.EnableWindow(FALSE);
   EnableControls(true);

   return TRUE;
}

LRESULT TimeLapsePhotoModeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // note: don't reset default release settings; must be done in new view

   return 0;
}

LRESULT TimeLapsePhotoModeView::OnButtonStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_SAVE);

   m_btnStart.EnableWindow(false);
   m_btnStop.EnableWindow(true);
   EnableControls(false);

   return 0;
}

LRESULT TimeLapsePhotoModeView::OnButtonStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_btnStart.EnableWindow(true);
   m_btnStop.EnableWindow(false);
   EnableControls(true);

   return 0;
}

LRESULT TimeLapsePhotoModeView::OnCheckboxScheduleStartTime(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_SAVE, IDC_CHECKBOX_TIMELAPSE_SCHEDULE_STARTTIME);

   m_timePickerScheduleStartDate.EnableWindow(m_scheduleIsCheckedStartTime);
   m_timePickerScheduleStartTime.EnableWindow(m_scheduleIsCheckedStartTime);

   return 0;
}

LRESULT TimeLapsePhotoModeView::OnCheckboxScheduleEndTime(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_SAVE, IDC_CHECKBOX_TIMELAPSE_SCHEDULE_ENDTIME);

   m_timePickerScheduleEndDate.EnableWindow(m_scheduleIsCheckedEndTime);
   m_timePickerScheduleEndTime.EnableWindow(m_scheduleIsCheckedEndTime);

   return 0;
}

void TimeLapsePhotoModeView::EnableControls(bool enable)
{
   DoDataExchange(DDX_SAVE, IDC_CHECKBOX_TIMELAPSE_SCHEDULE_ENDTIME);
   DoDataExchange(DDX_SAVE, IDC_CHECKBOX_TIMELAPSE_SCHEDULE_STARTTIME);

   m_timePickerReleaseTriggerInterval.EnableWindow(enable);

   GetDlgItem(IDC_RADIO_TIMELAPSE_RELEASE_TRIGGER_INTERVAL).EnableWindow(enable);
   GetDlgItem(IDC_RADIO_TIMELAPSE_RELEASE_TRIGGER_POST_TRANSFER).EnableWindow(enable);
   GetDlgItem(IDC_RADIO_TIMELAPSE_RELEASE_TRIGGER_MANUAL).EnableWindow(enable);

   GetDlgItem(IDC_CHECKBOX_TIMELAPSE_SCHEDULE_STARTTIME).EnableWindow(enable);
   m_timePickerScheduleStartDate.EnableWindow(m_scheduleIsCheckedStartTime && enable);
   m_timePickerScheduleStartTime.EnableWindow(m_scheduleIsCheckedStartTime && enable);

   GetDlgItem(IDC_CHECKBOX_TIMELAPSE_SCHEDULE_ENDTIME).EnableWindow(enable);
   m_timePickerScheduleEndDate.EnableWindow(m_scheduleIsCheckedEndTime && enable);
   m_timePickerScheduleEndTime.EnableWindow(m_scheduleIsCheckedEndTime && enable);
}
