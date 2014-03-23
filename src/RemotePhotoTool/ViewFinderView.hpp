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
   /// ctor
   ViewFinderView(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);
   /// dtor
   ~ViewFinderView() throw() {}

   /// dialog id
   enum { IDD = IDD_VIEWFINDER_FORM };

   /// sets viewfinder
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
      COMMAND_HANDLER(IDC_COMBO_VIEWFINDER_LINESMODE, CBN_SELENDOK, OnComboLinesModeSelEndOk)
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

   /// called when view is being shown
   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   /// called at destruction of view
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   /// called when lines mode combobox is closed with a new selection
   LRESULT OnComboLinesModeSelEndOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when a horizontal scroll bar (the zoom trackbar) has been changed
   LRESULT OnHScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   /// called when button Auto Focus is pressed
   LRESULT OnBnClickedAutoFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when button Auto White Balance is pressed
   LRESULT OnBnClickedAutoWhiteBalance(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when zoom out button is pressed
   LRESULT OnBnClickedZoomOut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when zoom in button is pressed
   LRESULT OnBnClickedZoomIn(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when button Histogram is pressed
   LRESULT OnBnClickedHistogram(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   /// called when button for previous images is pressed
   LRESULT OnBnClickedPreviousImageViewer(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   /// sets up lines mode combobox
   void SetupLinesModeCombobox();

   /// sets up viewfinder window
   void SetupViewfinderWindow();

   /// sets up zoom controls
   void SetupZoomControls();

   /// sets new zoom position
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
