//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file GeoTagTool\SerialPortDlg.hpp About dialog for GeoTagTool
//
#include "stdafx.h"
#include "SerialPortDlg.hpp"
#include "Serial\SerialPort.hpp"

LRESULT SerialPortDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   CenterWindow(GetParent());

   m_serialPortInfoList = Serial::SerialPort::EnumSerialPorts();

   // fill combobox
   for (size_t index = 0, maxIndex = m_serialPortInfoList.size(); index < maxIndex; index++)
   {
      const Serial::SerialPortInfo& info = m_serialPortInfoList[index];

      int itemIndex = m_comboSerialPort.AddString(info.DisplayName());
      m_comboSerialPort.SetItemData(itemIndex, index);
   }

   m_comboSerialPort.SetCurSel(1);

   return TRUE;
}

LRESULT SerialPortDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   EndDialog(wID);

   if (wID == IDOK)
   {
      int itemIndex = m_comboSerialPort.GetCurSel();
      size_t index = m_comboSerialPort.GetItemData(itemIndex);

      m_serialPortDeviceName = m_serialPortInfoList[index].DeviceName();
   }

   return 0;
}
