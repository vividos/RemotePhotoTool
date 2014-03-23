//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ViewFinderImageWindow.hpp Viewfinder image window
//
#pragma once

// includes
#include "LightweightMutex.hpp"
#include "Event.hpp"

// forward references
class Viewfinder;

/// image control for viewfinder image
class ViewFinderImageWindow: public CWindowImpl<ViewFinderImageWindow>
{
public:
   /// ctor
   ViewFinderImageWindow();
   /// dtor
   ~ViewFinderImageWindow() throw();

   /// lines mode types
   enum T_enLinesMode
   {
      linesModeNoLines = 0,      ///< show no lines
      linesModeRuleOfThird = 1,  ///< show lines at 1/3 points
      linesModeGoldenRatio = 2,  ///< show lines that divide image according to golden ratio

      linesModeMax = linesModeGoldenRatio ///< max. mode value
   };

   /// sets lines mode
   void SetLinesMode(T_enLinesMode enLinesMode) throw() { m_enLinesMode = enLinesMode; }

   /// shows zebra pattern for overexposed images
   void ShowZebraPattern(bool bShowZebraPattern) throw() { m_bShowZebraPattern = bShowZebraPattern; }

   /// sets if histogram is shown
   void ShowHistogram(bool bShowHistogram) throw() { m_bShowHistogram = bShowHistogram; }

   /// enables or disables updates to the viewfinder window
   void EnableUpdate(bool bEnable);

   DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW | CS_VREDRAW, COLOR_APPWORKSPACE)

private:
   BEGIN_MSG_MAP(ViewFinderImageWindow)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_VIEWFINDER_AVAIL_IMAGE, OnMessageViewfinderAvailImage)
   END_MSG_MAP()

   /// sets viewfinder object
   void SetViewfinder(std::shared_ptr<Viewfinder> spViewfinder);

private:
   /// called when new viewfinder image is available
   void OnAvailViewfinderImage(const std::vector<BYTE>& vecImage);

   /// message arrived that new viewfinder image is available
   LRESULT OnMessageViewfinderAvailImage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

   /// sets up zebra brush
   void SetupZebraBrush();

   /// decodes raw jpeg data into image and stores it for drawing
   void DecodeJpegImage(const std::vector<BYTE>& vecImage);

   /// sets transparency for overexposed values in bitmap
   void MakeOverexposedTransparent(std::vector<BYTE>& vecBitmapData);

   /// creates bitmap from viewfinder data
   void CreateBitmap(CBitmapHandle& bmp);

   /// sets new bitmap
   void SetBitmap(CBitmapHandle bmpViewfinder);

   /// traces viewfinder fps
   void TraceViewfinderFps();

   /// scales bitmap size, according to window size
   void ScaleBitmapSize(const BITMAP& bm, int& iWidth, int& iHeight);

   /// draws lines into dc
   void DrawLines(CDC& dc, int iWidth, int iHeight);

   /// draws zebra pattern
   void DrawZebraPattern(CDC& dc, int iWidth, int iHeight);

private:
// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   /// erases background
   LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

   /// paints viewfinder image
   LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

   /// destroys window
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
   /// viewfinder
   std::shared_ptr<Viewfinder> m_spViewfinder;

   /// mutex to protect m_vecCurrentViewfinderData, m_uiResX and m_uiResY
   LightweightMutex m_mtxViewfinderData;

   /// newly arrived viewfinder data
   std::vector<BYTE> m_vecCurrentViewfinderData;

   /// x resolution of viewfinder image
   unsigned int m_uiResX;
   /// y resolution of viewfinder image
   unsigned int m_uiResY;

   /// bitmap for viewfinder
   CBitmap m_bmpViewfinder;

   /// lines mode
   T_enLinesMode m_enLinesMode;

   /// indicates if zebra pattern for overexposed areas are drawn
   bool m_bShowZebraPattern;

   /// brush for drawing zebra pattern
   CBrush m_brushZebraPattern;

   /// indicates if histogram is shown
   bool m_bShowHistogram;
};
