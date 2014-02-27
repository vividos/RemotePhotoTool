//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ViewFinderView.hpp View for viewfinder image
//
#pragma once

// includes
#include "ViewFinderImageWindow.hpp"
#include "ImageProperty.hpp"

// forward references
class RemoteReleaseControl;

/// view class for viewfinder
class ViewFinderView :
   public CDialogImpl<ViewFinderView>,
   public CDialogResize<ViewFinderView>,
   public CWinDataExchange<ViewFinderView>
{
public:
   ViewFinderView(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);
   ~ViewFinderView() throw() {}

   enum { IDD = IDD_VIEWFINDER_FORM };

   void SetViewfinder(std::shared_ptr<Viewfinder> spViewfinder);

   /// enables or disables updates to the viewfinder window
   void EnableUpdate(bool bEnable);

private:
   friend CDialogResize<ViewFinderView>;

   // ddx map
   BEGIN_DDX_MAP(ViewFinderView)
      DDX_CONTROL_HANDLE(IDC_SLIDER_ZOOM, m_tbZoom)
      DDX_CONTROL_HANDLE(IDC_COMBO_VIEWFINDER_LINESMODE, m_cbLinesMode)
   END_DDX_MAP()

   // resize map
   BEGIN_DLGRESIZE_MAP(ViewFinderView)
      DLGRESIZE_CONTROL(IDC_STATIC_VIEWFINDER_FRAME, DLSZ_SIZE_X | DLSZ_SIZE_Y)
   END_DLGRESIZE_MAP()

   // message map
   BEGIN_MSG_MAP(ViewFinderView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      NOTIFY_HANDLER(IDC_COMBO_VIEWFINDER_LINESMODE, CBN_SELCHANGE, OnComboLinesModeSelChange)
      MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
      COMMAND_HANDLER(IDC_BUTTON_VIEWFINDER_AUTOFOCUS, BN_CLICKED, OnBnClickedAutoFocus)
      COMMAND_HANDLER(IDC_BUTTON_VIEWFINDER_AUTOWHITEBALANCE, BN_CLICKED, OnBnClickedAutoWhiteBalance)
      COMMAND_HANDLER(IDC_BUTTON_ZOOM_OUT, BN_CLICKED, OnBnClickedZoomOut)
      COMMAND_HANDLER(IDC_BUTTON_ZOOM_IN, BN_CLICKED, OnBnClickedZoomIn)
      COMMAND_HANDLER(IDC_BUTTON_VIEWFINDER_HISTOGRAM, BN_CLICKED, OnBnClickedHistogram)
      COMMAND_HANDLER(IDC_BUTTON_PREV_IMAGEVIEWER, BN_CLICKED, OnBnClickedPreviousImageViewer)
      CHAIN_MSG_MAP(CDialogResize<ViewFinderView>)
      REFLECT_NOTIFICATIONS() // to make sure superclassed controls get notification messages
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnComboLinesModeSelChange(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
   LRESULT OnHScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnBnClickedAutoFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnBnClickedAutoWhiteBalance(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnBnClickedZoomOut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnBnClickedZoomIn(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnBnClickedHistogram(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnBnClickedPreviousImageViewer(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   void SetupLinesModeCombobox();
   void SetupViewfinderWindow();
   void SetupZoomControls();

   void SetZoomPos(unsigned int uiPos);

private:
   // UI

   /// zoom trackbar
   CTrackBarCtrl m_tbZoom;

   /// lines mode combobox
   CComboBox m_cbLinesMode;

   /// viewfinder window
   std::unique_ptr<ViewFinderImageWindow> m_upViewFinderWindow;

   // Model

   /// indicates if histogram is shown
   bool m_bShowHistogram;

   /// list of all possible zoom values
   std::vector<ImageProperty> m_vecAllZoomValues;

   /// release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;
};
