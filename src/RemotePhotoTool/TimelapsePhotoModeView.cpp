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

   const_cast<TimeLapsePhotoModeView&>(*this).
      SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_TIMELAPSE_STOP, 0), 0);

   return true;
}

LRESULT TimeLapsePhotoModeView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   m_spRemoteReleaseControl = m_host.GetRemoteReleaseControl();

   auto fnFinished = std::bind(&TimeLapsePhotoModeView::OnTimeLapseFinished, this);

   if (!m_manager.Init(m_spRemoteReleaseControl, fnFinished))
      return FALSE;

   m_btnStop.EnableWindow(FALSE);
   EnableControls(true);

   COleDateTime spanInterval;
   spanInterval.SetTime(0, 1, 0); // 1 minute
   SYSTEMTIME systemTimeInterval = { 0 };
   spanInterval.GetAsSystemTime(systemTimeInterval);
   m_timePickerReleaseTriggerInterval.SetSystemTime(GDT_VALID, &systemTimeInterval);

   return TRUE;
}

LRESULT TimeLapsePhotoModeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // note: don't reset default release settings; must be done in new view

   return 0;
}

/// returns a time span value from a time picker control
static COleDateTimeSpan OleDateTimeSpanFromTimePicker(const CDateTimePickerCtrl& picker)
{
   SYSTEMTIME systemTime = { 0 };
   picker.GetSystemTime(&systemTime);

   COleDateTime dateTime(systemTime);
   return COleDateTimeSpan(0, dateTime.GetHour(), dateTime.GetMinute(), dateTime.GetSecond());
}

/// returns a date and time value from a date picker control and a time picker control
static COleDateTime OleDateTimeFromDateAndTimePicker(const CDateTimePickerCtrl& datePicker, const CDateTimePickerCtrl& timePicker)
{
   SYSTEMTIME systemTime = { 0 };
   datePicker.GetSystemTime(&systemTime);

   COleDateTime dateTime(systemTime);
   dateTime.SetDate(dateTime.GetYear(), dateTime.GetMonth(), dateTime.GetDay());

   return dateTime + OleDateTimeSpanFromTimePicker(timePicker);
}

LRESULT TimeLapsePhotoModeView::OnButtonStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_SAVE);

   TimeLapseOptions& options = m_manager.Options();

   options.m_releaseTrigger = static_cast<TimeLapseOptions::T_enReleaseTrigger>(m_releaseTriggerRadio);

   if (options.m_releaseTrigger == TimeLapseOptions::releaseTriggerInterval)
   {
      options.m_intervalTime = OleDateTimeSpanFromTimePicker(m_timePickerReleaseTriggerInterval);
   }

   options.m_useStartTime = m_scheduleIsCheckedStartTime;
   if (m_scheduleIsCheckedStartTime)
   {
      options.m_startTime = OleDateTimeFromDateAndTimePicker(m_timePickerScheduleStartDate, m_timePickerScheduleStartTime);
   }

   options.m_useEndTime = m_scheduleIsCheckedEndTime;
   if (m_scheduleIsCheckedEndTime)
   {
      options.m_endTime = OleDateTimeFromDateAndTimePicker(m_timePickerScheduleEndDate, m_timePickerScheduleEndTime);
   }

   m_btnStart.EnableWindow(false);
   m_btnStop.EnableWindow(true);
   EnableControls(false);

   m_manager.Start();

   return 0;
}

LRESULT TimeLapsePhotoModeView::OnButtonStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_btnStart.EnableWindow(true);
   m_btnStop.EnableWindow(false);
   EnableControls(true);

   m_manager.Stop();

   return 0;
}

LRESULT TimeLapsePhotoModeView::OnButtonCameraRelease(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
   if (m_manager.IsStarted() &&
      m_manager.Options().m_releaseTrigger == TimeLapseOptions::releaseManually)
   {
      m_manager.ManualRelease();
   }
   else
   {
      bHandled = false;
   }

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

   bool automaticTrigger = m_releaseTriggerRadio != 2;

   GetDlgItem(IDC_CHECKBOX_TIMELAPSE_SCHEDULE_STARTTIME).EnableWindow(automaticTrigger && enable);
   m_timePickerScheduleStartDate.EnableWindow(m_scheduleIsCheckedStartTime && automaticTrigger && enable);
   m_timePickerScheduleStartTime.EnableWindow(m_scheduleIsCheckedStartTime && automaticTrigger && enable);

   GetDlgItem(IDC_CHECKBOX_TIMELAPSE_SCHEDULE_ENDTIME).EnableWindow(automaticTrigger && enable);
   m_timePickerScheduleEndDate.EnableWindow(m_scheduleIsCheckedEndTime && automaticTrigger && enable);
   m_timePickerScheduleEndTime.EnableWindow(m_scheduleIsCheckedEndTime && automaticTrigger && enable);
}

void TimeLapsePhotoModeView::OnTimeLapseFinished()
{
   // use a message, since this call arrives on the timelapse scheduler thread
   PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_TIMELAPSE_STOP, 0), 0);
}
