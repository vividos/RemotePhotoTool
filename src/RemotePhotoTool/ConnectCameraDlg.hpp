//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file ConnectCameraDlg.hpp Camera connect dialog
//
#pragma once

// includes
#include "ViewManager.hpp"
#include "Instance.hpp"
#include "SourceDevice.hpp"

/// connect to camera dialog
class ConnectCameraDlg :
   public CDialogImpl<ConnectCameraDlg>,
   public CDialogResize<ConnectCameraDlg>,
   public CWinDataExchange<ConnectCameraDlg>
{
public:
   /// dtor
   ConnectCameraDlg();

   /// dialog id
   enum { IDD = IDD_CONNECT_CAMERA };

   /// returns source device of connected camera
   std::shared_ptr<SourceDevice> GetSourceDevice() const;

private:
   BEGIN_MSG_MAP(ConnectCameraDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
      COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
      COMMAND_ID_HANDLER(IDC_BUTTON_REFRESH, OnBtnRefresh)
      COMMAND_ID_HANDLER(IDC_BUTTON_INFO, OnBtnInfo)
      NOTIFY_HANDLER(IDC_LIST_CAMERA, LVN_ITEMCHANGED, OnListCameraItemChanged)
      CHAIN_MSG_MAP(CDialogResize<ConnectCameraDlg>)
   END_MSG_MAP()

   BEGIN_DLGRESIZE_MAP(ConnectCameraDlg)
      DLGRESIZE_CONTROL(IDC_LIST_CAMERA, DLSZ_SIZE_X | DLSZ_SIZE_Y)
      DLGRESIZE_CONTROL(IDC_BUTTON_REFRESH, DLSZ_MOVE_X)
      DLGRESIZE_CONTROL(IDC_BUTTON_INFO, DLSZ_MOVE_X)
      DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
      DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
   END_DLGRESIZE_MAP()

   BEGIN_DDX_MAP(ConnectCameraDlg)
      DDX_CONTROL_HANDLE(IDC_LIST_CAMERA, m_lcCameras)
   END_DDX_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
// LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)

   /// called to init dialog
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called when closing dialog
   LRESULT OnCloseCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when refresh button is pressed
   LRESULT OnBtnRefresh(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when info button is pressed
   LRESULT OnBtnInfo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when selected camera in list has changed
   LRESULT OnListCameraItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled);

   /// called when camera is added
   void OnCameraAdded();

   /// refreshes camera list
   void RefreshCameraList();

private:
   /// CameraControl instance
   Instance m_instance;

   /// camera list
   CListViewCtrl m_lcCameras;

   /// icons for camera list
   CImageList m_ilIcons;

   /// index of selected source device
   int m_iSelectedSourceDeviceIndex;
};
