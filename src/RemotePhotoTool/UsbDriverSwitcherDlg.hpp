//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file UsbDriverSwitcherDlg.hpp USB driver switcher dialog
//
#pragma once

#include <libwdi.h>
#include <atlddx.h>

/// \brief USB driver switcher dialog
/// \details Shows a dialog that allows the user to switch the USB driver for USB devices from
/// the normal driver to WinUsb that is compatible with gPhoto2.
class UsbDriverSwitcherDlg :
   public CDialogImpl<UsbDriverSwitcherDlg>,
   public CWinDataExchange< UsbDriverSwitcherDlg>,
   public CDialogResize<UsbDriverSwitcherDlg>
{
public:
   /// dialog ID
   enum { IDD = IDD_USB_DRIVER_SWITCHER };

private:
   friend CDialogResize<UsbDriverSwitcherDlg>;

   BEGIN_DDX_MAP(UsbDriverSwitcherDlg)
      DDX_CONTROL_HANDLE(IDC_LIST_USB_DEVICES, m_listUSBDevices)
   END_DDX_MAP()

   BEGIN_DLGRESIZE_MAP(UsbDriverSwitcherDlg)
      DLGRESIZE_CONTROL(IDC_STATIC_USB_DRIVER_NOTE, DLSZ_SIZE_X)
      DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X)
      DLGRESIZE_CONTROL(IDC_LIST_USB_DEVICES, DLSZ_SIZE_X | DLSZ_SIZE_Y)
      DLGRESIZE_CONTROL(IDC_BUTTON_USB_DEVICES_REFRESH, DLSZ_MOVE_X)
      DLGRESIZE_CONTROL(IDC_BUTTON_USB_SWITCH_DRIVER, DLSZ_MOVE_X)
   END_DLGRESIZE_MAP()

   BEGIN_MSG_MAP(UsbDriverSwitcherDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_TIMER, OnTimer)
      COMMAND_ID_HANDLER(IDOK, OnOKOrCancel)
      COMMAND_ID_HANDLER(IDCANCEL, OnOKOrCancel)
      COMMAND_ID_HANDLER(IDC_BUTTON_USB_DEVICES_REFRESH, OnButtonUsbDevicesRefresh)
      COMMAND_ID_HANDLER(IDC_BUTTON_USB_SWITCH_DRIVER, OnButtonUsbSwitchDriver)
      CHAIN_MSG_MAP(CDialogResize<UsbDriverSwitcherDlg>)
   END_MSG_MAP()

   /// initializes dialog
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when dialog is destroyed
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when a timer has elapsed
   LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when user clicks on the Close button or on the X dialog icon
   LRESULT OnOKOrCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      EndDialog(wID);
      return 0;
   }

   /// called when the user clicked on the "refresh" button
   LRESULT OnButtonUsbDevicesRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      RefreshList();
      return 0;
   }

   /// called when the user clicked on the "switch driver" button
   LRESULT OnButtonUsbSwitchDriver(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// sets up list of USB devices
   void SetupList();

   /// refreshes list of USB devices
   void RefreshList();

private:
   // UI

   /// list of all USB devices with their current driver state
   CListViewCtrl m_listUSBDevices;

   // Model

   /// current libwdi device info list
   std::shared_ptr<struct wdi_device_info> m_deviceInfoList;
};
