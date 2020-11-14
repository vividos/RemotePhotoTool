//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file UsbDriverSwitcherDlg.cpp USB driver switcher dialog
//
#include "stdafx.h"
#include "resource.h"
#include "UsbDriverSwitcherDlg.hpp"
#include <ulib/Path.hpp>

/// timer ID for refreshing USB devices list
const UINT_PTR c_timerRefreshList = 1;

/// refresh cycle time for USB devices list
const UINT c_refreshCycleTime = 1000;

LRESULT UsbDriverSwitcherDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DlgResize_Init();
   DoDataExchange(DDX_LOAD);

   CenterWindow();

   SetupList();
   RefreshList();

   SetTimer(c_timerRefreshList, c_refreshCycleTime);

   return TRUE;
}

LRESULT UsbDriverSwitcherDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   KillTimer(c_timerRefreshList);

   return 0;
}

LRESULT UsbDriverSwitcherDlg::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if (wParam == c_timerRefreshList)
   {
      KillTimer(c_timerRefreshList);

      RefreshList();

      SetTimer(c_timerRefreshList, c_refreshCycleTime);
   }

   return 0;
}

LRESULT UsbDriverSwitcherDlg::OnButtonUsbSwitchDriver(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   int selectedIndex = m_listUSBDevices.GetSelectedIndex();
   if (selectedIndex != -1)
   {
      struct wdi_device_info* selectedDeviceInfo =
         (struct wdi_device_info*)m_listUSBDevices.GetItemData(selectedIndex);

      if (selectedDeviceInfo != nullptr)
      {
         // stop timer or else it will mess up the wdi_device_info struct
         KillTimer(c_timerRefreshList);

         SwitchDriver(selectedDeviceInfo);

         SetTimer(c_timerRefreshList, c_refreshCycleTime);
      }
   }

   return 0;
}

LRESULT UsbDriverSwitcherDlg::OnButtonUsbOpenDeviceManager(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   ShellExecute(m_hWnd, _T("open"), _T("devmgmt.msc"), nullptr, nullptr, SW_SHOWNORMAL);
   return 0;
}

void UsbDriverSwitcherDlg::SetupList()
{
   m_listUSBDevices.AddColumn(_T("USB Device"), 0);
   m_listUSBDevices.AddColumn(_T("Driver status"), 1);

   m_listUSBDevices.SetColumnWidth(0, 300);
   m_listUSBDevices.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

   m_listUSBDevices.SetExtendedListViewStyle(
      LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES,
      LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void UsbDriverSwitcherDlg::RefreshList()
{
   CString selectedDeviceId;
   int selectedIndex = m_listUSBDevices.GetSelectedIndex();
   if (selectedIndex != -1)
   {
      struct wdi_device_info* selectedDeviceInfo =
         (struct wdi_device_info*)m_listUSBDevices.GetItemData(selectedIndex);

      if (selectedDeviceInfo != nullptr)
      {
         selectedDeviceId = selectedDeviceInfo->device_id;
      }
   }

   m_listUSBDevices.SetRedraw(FALSE);

   m_listUSBDevices.DeleteAllItems();

   struct wdi_options_create_list cl_options = { 0 };
   cl_options.list_all = TRUE;
   cl_options.list_hubs = FALSE;
   cl_options.trim_whitespaces = TRUE;

   int newSelectedIndex = -1;
   struct wdi_device_info* rawDeviceInfo = nullptr;
   int ret = wdi_create_list(&rawDeviceInfo, &cl_options);
   if (ret == WDI_SUCCESS && rawDeviceInfo != nullptr)
   {
      m_deviceInfoList.reset(rawDeviceInfo, wdi_destroy_list);

      for (struct wdi_device_info* deviceInfo = m_deviceInfoList.get(); deviceInfo != nullptr; deviceInfo = deviceInfo->next)
      {
         if (deviceInfo->desc == nullptr)
            continue;

         int itemIndex = m_listUSBDevices.InsertItem(
            m_listUSBDevices.GetItemCount(),
            CString(deviceInfo->desc),
            0);

         bool isWinusbDriver = CStringA(deviceInfo->driver) == "WinUSB";

         m_listUSBDevices.SetItemText(itemIndex, 1, isWinusbDriver ? _T("gPhoto2 compatible") : _T("other driver"));

         m_listUSBDevices.SetItemData(itemIndex, (DWORD_PTR)deviceInfo);

         if (!selectedDeviceId.IsEmpty() &&
            selectedDeviceId == deviceInfo->device_id)
         {
            newSelectedIndex = itemIndex;
         }
      }
   }

   if (newSelectedIndex != -1)
      m_listUSBDevices.SelectItem(newSelectedIndex);

   m_listUSBDevices.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

   m_listUSBDevices.SetRedraw(TRUE);
}

void UsbDriverSwitcherDlg::SwitchDriver(struct wdi_device_info* deviceInfo)
{
   CString folderPath = Path::Combine(Path::TempFolder(), _T("usb_driver"));
   CString infName{ _T("default.inf") };

   Path::CreateDirectoryRecursive(folderPath);

   wdi_options_prepare_driver prepareDriverOptions = { };
   prepareDriverOptions.driver_type = WDI_WINUSB;

   int ret = wdi_prepare_driver(deviceInfo, CStringA(folderPath), CStringA(infName), &prepareDriverOptions);

   if (ret != WDI_SUCCESS)
   {
      CString text;
      text.Format(_T("Error while preparing USB driver: %hs"), wdi_strerror(ret));

      AtlMessageBox(m_hWnd,
         text.GetString(),
         IDR_MAINFRAME,
         MB_OK | MB_ICONERROR);

      return;
   }

   wdi_options_install_driver installDriverOptions = { };
   installDriverOptions.hWnd = m_hWnd;
   installDriverOptions.install_filter_driver = false;

   ret = wdi_install_driver(deviceInfo, CStringA(folderPath), CStringA(infName), &installDriverOptions);

   if (ret != WDI_SUCCESS)
   {
      CString text;
      text.Format(_T("Error while installing USB driver: %hs"), wdi_strerror(ret));

      AtlMessageBox(m_hWnd,
         text.GetString(),
         IDR_MAINFRAME,
         MB_OK | MB_ICONERROR);

      return;
   }

   RefreshList();
}
