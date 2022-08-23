//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file ConnectCameraDlg.cpp Camera connect dialog
//

// includes
#include "StdAfx.h"
#include "resource.h"
#include "ConnectCameraDlg.hpp"
#include "CameraInfoDlg.hpp"
#include "CameraErrorDlg.hpp"
#include "Icons.hpp"
#include "Instance.hpp"
#include "CameraException.hpp"
#include "SourceInfo.hpp"
#include "UsbDriverSwitcherDlg.hpp"

ConnectCameraDlg::ConnectCameraDlg()
:m_iSelectedSourceDeviceIndex(-1),
 m_instance(Instance::Get())
{
}

std::shared_ptr<SourceDevice> ConnectCameraDlg::GetSourceDevice() const
{
   ATLASSERT(m_iSelectedSourceDeviceIndex != -1);

   size_t sourceDeviceIndex = static_cast<size_t>(m_iSelectedSourceDeviceIndex);

   try
   {
      std::vector<std::shared_ptr<SourceInfo>> vecSourceDevices;
      m_instance.EnumerateDevices(vecSourceDevices);

      if (!vecSourceDevices.empty() &&
         sourceDeviceIndex < vecSourceDevices.size())
      {
         std::shared_ptr<SourceInfo> spSourceInfo = vecSourceDevices[sourceDeviceIndex];
         return spSourceInfo->Open();
      }
   }
   catch (const CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't open camera device"), ex);
      dlg.DoModal(m_hWnd);
   }

   return std::shared_ptr<SourceDevice>();
}

LRESULT ConnectCameraDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   CenterWindow(GetParent());

   DlgResize_Init(false, false);

   // load image lists
   {
      m_ilIcons.Create(16,15, ILC_MASK | ILC_COLOR32, 0, 0);
      CBitmap bmpIcons;
      bmpIcons.LoadBitmap(IDB_ICONS);
      m_ilIcons.Add(bmpIcons, RGB(255, 0, 255));
   }

   // set up cameras list
   {
      m_lcCameras.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
      m_lcCameras.SetImageList(m_ilIcons, LVSIL_SMALL);

      CRect rcCamera;
      m_lcCameras.GetClientRect(&rcCamera);

      m_lcCameras.InsertColumn(0, _T("Camera"), LVCFMT_LEFT, rcCamera.Width());

      RefreshCameraList();
   }

   if (m_lcCameras.GetItemCount() > 0)
      m_lcCameras.SelectItem(0);

   m_instance.AsyncWaitForCamera(std::bind(&ConnectCameraDlg::OnCameraAdded, this));

   // hide the "config usb" button, as switching doesn't work
   ::ShowWindow(GetDlgItem(IDC_BUTTON_CONFIG_USB), SW_HIDE);

   return TRUE;
}

LRESULT ConnectCameraDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (wID == IDOK)
   {
      // get device index of selected device
      int iItem = m_lcCameras.GetSelectedIndex();
      m_iSelectedSourceDeviceIndex = m_lcCameras.GetItemData(iItem);
      ATLASSERT(m_iSelectedSourceDeviceIndex != -1);
   }

   // unregister handler
   m_instance.AsyncWaitForCamera();

   EndDialog(wID);
   return 0;
}

LRESULT ConnectCameraDlg::OnBtnRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   int iItemCountBeforeRefresh = m_lcCameras.GetItemCount();

   RefreshCameraList();

   int iItemCount = m_lcCameras.GetItemCount();

   // select first item when list has updated
   if (iItemCount > 0 &&
       iItemCountBeforeRefresh != iItemCount &&
       m_lcCameras.GetSelectedIndex() == -1)
      m_lcCameras.SelectItem(0);

   return 0;
}

LRESULT ConnectCameraDlg::OnBtnInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   // unregister handler
   m_instance.AsyncWaitForCamera();

   // get device index of selected device
   int iItem = m_lcCameras.GetSelectedIndex();
   m_iSelectedSourceDeviceIndex = static_cast<int>(m_lcCameras.GetItemData(iItem));
   ATLASSERT(m_iSelectedSourceDeviceIndex != -1);

   // now that currently selected device is stored in m_iSelectedSourceDeviceIndex, we can call GetSourceDevice()
   try
   {
      std::shared_ptr<SourceDevice> spSourceDevice = GetSourceDevice();

      if (spSourceDevice != nullptr)
      {
         CameraInfoDlg dlg(*spSourceDevice);
         dlg.DoModal(m_hWnd);
      }
   }
   catch (const CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't get camera info"), ex);
      dlg.DoModal(m_hWnd);
   }

   // re-register handler
   m_instance.AsyncWaitForCamera(std::bind(&ConnectCameraDlg::OnCameraAdded, this));

   return 0;
}

LRESULT ConnectCameraDlg::OnButtonConfigUsb(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   UsbDriverSwitcherDlg dlg;
   dlg.DoModal();

   return 0;
}

LRESULT ConnectCameraDlg::OnListCameraItemChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   NMLISTVIEW* pnmv = (LPNMLISTVIEW)pnmh;

   int iItem = pnmv->iItem;

   if ((pnmv->uNewState & LVIS_SELECTED) == 0)
   {
      // selection was removed; check if an item is still selected
      int iSelectedItem = m_lcCameras.GetSelectedIndex();
      if (iSelectedItem == -1)
         iItem = -1;
      else
         return 0;
   }

   bool bEnableBtns = iItem != -1;

   if (iItem != -1)
      m_iSelectedSourceDeviceIndex = m_lcCameras.GetItemData(iItem);
   else
      m_iSelectedSourceDeviceIndex = -1;

   GetDlgItem(IDC_BUTTON_INFO).EnableWindow(bEnableBtns ? TRUE : FALSE);
   GetDlgItem(IDOK).EnableWindow(bEnableBtns ? TRUE : FALSE);

   return 0;
}

void ConnectCameraDlg::OnCameraAdded()
{
   // note: this is called from another thread, so just send a refresh button press message
   PostMessage(WM_COMMAND,
      MAKEWPARAM(IDC_BUTTON_REFRESH, BN_CLICKED),
      reinterpret_cast<LPARAM>(GetDlgItem(IDC_BUTTON_REFRESH).m_hWnd));
}

void ConnectCameraDlg::RefreshCameraList()
{
   m_lcCameras.SetRedraw(FALSE);
   m_lcCameras.DeleteAllItems();

   int iLastSelectedItem = m_lcCameras.GetSelectedIndex();
   CString cszLastSelectedCamera;
   m_lcCameras.GetItemText(iLastSelectedItem, 0, cszLastSelectedCamera);

   int iNextSelectedItem = -1;
   try
   {
      std::vector<std::shared_ptr<SourceInfo>> vecSourceDevices;
      m_instance.EnumerateDevices(vecSourceDevices);

      for (size_t i=0, iMax=vecSourceDevices.size(); i<iMax; i++)
      {
         std::shared_ptr<SourceInfo> spSourceInfo = vecSourceDevices[i];

         int iItem = m_lcCameras.InsertItem(m_lcCameras.GetItemCount(), spSourceInfo->Name(), iconSmallCamera);
         m_lcCameras.SetItemData(iItem, i);

         if (cszLastSelectedCamera == spSourceInfo->Name())
            iNextSelectedItem = iItem;
      }

      if (vecSourceDevices.empty())
      {
         GetDlgItem(IDC_BUTTON_INFO).EnableWindow(FALSE);
         GetDlgItem(IDOK).EnableWindow(FALSE);
      }
   }
   catch (const CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't refresh camera list"), ex);
      dlg.DoModal(m_hWnd);
   }

   if (iNextSelectedItem != -1)
      m_lcCameras.SelectItem(iNextSelectedItem);

   m_lcCameras.SetRedraw(TRUE);
}
