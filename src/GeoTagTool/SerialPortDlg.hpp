//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file GeoTagTool\SerialPortDlg.hpp About dialog for GeoTagTool
//
#pragma once

#include "Serial\SerialPortInfo.hpp"
#include "resource.h"

/// serial port dialog
class SerialPortDlg :
   public CDialogImpl<SerialPortDlg>,
   public CWinDataExchange<SerialPortDlg>
{
public:
   /// dialog ID
   enum { IDD = IDD_SERIAL_PORT };

   /// returns selected serial port device name
   CString GetSerialPortDeviceName() { return m_serialPortDeviceName; }

private:
   BEGIN_DDX_MAP(SerialPortDlg)
      DDX_CONTROL_HANDLE(IDC_COMBO_SERIALPORT, m_comboSerialPort)
   END_DDX_MAP()

   BEGIN_MSG_MAP(SerialPortDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
      COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   // LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   /// called when dialog is initialized
   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

   /// called when dialog is closed
   LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
   // UI

   /// serial port comobobox
   CComboBox m_comboSerialPort;

   // model

   /// device name of selected serial port
   CString m_serialPortDeviceName;

   /// list of serial port infos
   std::vector<Serial::SerialPortInfo> m_serialPortInfoList;
};
