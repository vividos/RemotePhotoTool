//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file CameraInfoDlg.hpp Camera info dialog
//
#pragma once

// forward references
class SourceDevice;
class ImageProperty;
class RemoteReleaseControl;

/// camera info dialog
class CameraInfoDlg :
   public CDialogImpl<CameraInfoDlg>,
   public CDialogResize<CameraInfoDlg>,
   public CWinDataExchange<CameraInfoDlg>
{
public:
   /// ctor
   CameraInfoDlg(SourceDevice& sourceDevice)
      :m_sourceDevice(sourceDevice)
   {
   }

   /// dialog id
   enum { IDD = IDD_CAMERAINFO };

private:
   BEGIN_MSG_MAP(CameraInfoDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
      COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
      COMMAND_ID_HANDLER(IDC_BUTTON_COPY, OnBtnCopy)
      CHAIN_MSG_MAP(CDialogResize<CameraInfoDlg>)
   END_MSG_MAP()

   BEGIN_DLGRESIZE_MAP(CameraInfoDlg)
      DLGRESIZE_CONTROL(IDC_EDIT_INFO, DLSZ_SIZE_X | DLSZ_SIZE_Y)
      DLGRESIZE_CONTROL(IDC_BUTTON_COPY, DLSZ_MOVE_X | DLSZ_MOVE_Y)
      DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
   END_DLGRESIZE_MAP()

   BEGIN_DDX_MAP(CameraInfoDlg)
      DDX_CONTROL_HANDLE(IDC_EDIT_INFO, m_ecCameraInfo)
   END_DDX_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
// LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)

   /// called at start of dialog
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called on closing dialog
   LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      EndDialog(wID);
      return 0;
   }

   /// called when copy button is pressed
   LRESULT OnBtnCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      int iStart = 0, iEnd = 0;
      m_ecCameraInfo.GetSel(iStart, iEnd);
      m_ecCameraInfo.SetSel(0, -1);
      m_ecCameraInfo.Copy();
      m_ecCameraInfo.SetSel(iStart, iEnd);
      return 0;
   }

   /// collects camera infos
   CString CollectCameraInfo();

   /// collects device properties
   void CollectDeviceProperties(CString& cszText);

   /// collects capabilities
   void CollectCapabilities(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, CString& cszText);

   /// collects image properties
   void CollectImageProperties(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, CString& cszText);

   /// collects shooting mode infos
   void CollectShootingModeInfos(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, CString& cszText);

   /// collects details about shooting mode
   void CollectShootingModeDetails(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      const ImageProperty& shootingMode, CString& cszText);

private:
   /// source device
   SourceDevice& m_sourceDevice;

   /// camera info edit control
   CEdit m_ecCameraInfo;
};
