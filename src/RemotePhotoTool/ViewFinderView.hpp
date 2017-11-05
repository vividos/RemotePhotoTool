//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file ViewFinderView.hpp View for viewfinder image
//
#pragma once

// includes
#include "ViewFinderImageWindow.hpp"
#include "ImageProperty.hpp"
#include "Viewfinder.hpp"

// forward references
class IPhotoModeViewHost;
class RemoteReleaseControl;

/// view class for viewfinder
class ViewFinderView :
   public CDialogImpl<ViewFinderView>,
   public CDialogResize<ViewFinderView>,
   public CWinDataExchange<ViewFinderView>
{
public:
   /// ctor
   ViewFinderView(IPhotoModeViewHost& host, std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);
   /// dtor
   ~ViewFinderView() {}

   /// dialog id
   enum { IDD = IDD_VIEWFINDER_FORM };

   /// sets viewfinder
   void SetViewfinder(std::shared_ptr<Viewfinder> spViewfinder);

   /// enables or disables updates to the viewfinder window
   void EnableUpdate(bool bEnable);

   /// performs auto-focus action
   void AutoFocus();

   /// performs auto white balance action
   void AutoWhiteBalance();

   /// sets new lines mode
   void SetLinesMode(ViewFinderImageWindow::T_enLinesMode enLinesMode);

   /// sets new viewfinder output type
   void SetOutputType(Viewfinder::T_enOutputType enOutputType);

private:
   friend CDialogResize<ViewFinderView>;

   // ddx map
   BEGIN_DDX_MAP(ViewFinderView)
      DDX_CONTROL_HANDLE(IDC_SLIDER_ZOOM, m_tbZoom)
   END_DDX_MAP()

   // resize map
   BEGIN_DLGRESIZE_MAP(ViewFinderView)
      DLGRESIZE_CONTROL(IDC_STATIC_VIEWFINDER_FRAME, DLSZ_SIZE_X | DLSZ_SIZE_Y)
   END_DLGRESIZE_MAP()

   // message map
   BEGIN_MSG_MAP(ViewFinderView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
      COMMAND_HANDLER(ID_VIEWFINDER_AUTO_FOCUS, BN_CLICKED, OnViewfinderAutoFocus)
      COMMAND_HANDLER(ID_VIEWFINDER_AUTO_WB, BN_CLICKED, OnViewfinderAutoWhiteBalance)
      COMMAND_HANDLER(ID_VIEWFINDER_ZOOM_OUT, BN_CLICKED, OnViewfinderZoomOut)
      COMMAND_HANDLER(ID_VIEWFINDER_ZOOM_IN, BN_CLICKED, OnViewfinderZoomIn)
      COMMAND_HANDLER(ID_VIEWFINDER_HISTOGRAM, BN_CLICKED, OnViewfinderHistogram)
      COMMAND_HANDLER(ID_VIEWFINDER_SHOW_OVEREXPOSED, BN_CLICKED, OnViewfinderShowOverexposed)
      CHAIN_MSG_MAP(CDialogResize<ViewFinderView>)
      REFLECT_NOTIFICATIONS() // to make sure superclassed controls get notification messages
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
// LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)

   /// called when view is being shown
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called at destruction of view
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called when a horizontal scroll bar (the zoom trackbar) has been changed
   LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called when button Auto Focus is pressed
   LRESULT OnViewfinderAutoFocus(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when button Auto White Balance is pressed
   LRESULT OnViewfinderAutoWhiteBalance(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when zoom out button is pressed
   LRESULT OnViewfinderZoomOut(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when zoom in button is pressed
   LRESULT OnViewfinderZoomIn(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when button Histogram is pressed
   LRESULT OnViewfinderHistogram(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when "show overexposed areas" button-checkbox is changed
   LRESULT OnViewfinderShowOverexposed(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// sets up viewfinder window
   void SetupViewfinderWindow();

   /// sets up zoom controls
   void SetupZoomControls();

   /// sets new zoom position
   void SetZoomPos(unsigned int uiPos);

   /// updates UI elements for new zoom position
   void UpdateZoomPosUI(unsigned int uiPos);

private:
   // UI

   /// host access
   IPhotoModeViewHost& m_host;

   /// zoom trackbar
   CTrackBarCtrl m_tbZoom;

   /// viewfinder window
   std::unique_ptr<ViewFinderImageWindow> m_upViewFinderWindow;

   // Model

   /// indicates if zebra pattern is shown
   bool m_bShowZebraPattern;

   /// indicates if histogram is shown
   bool m_bShowHistogram;

   /// list of all possible zoom values
   std::vector<ImageProperty> m_vecAllZoomValues;

   /// release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;
};
