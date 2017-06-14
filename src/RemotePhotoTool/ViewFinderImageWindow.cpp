//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ViewFinderImageWindow.cpp Viewfinder image window
//

// includes
#include "stdafx.h"
#include "ViewFinderImageWindow.hpp"
#include "Viewfinder.hpp"
#include "JpegMemoryReader.hpp"
#include "Logging.hpp"

/// ratio to draw lines for "golden ratio" mode
const double c_dGoldenRatio = 0.618;

/// number of milliseconds until zebra pattern is moved to the right
const unsigned int c_uiZebraPatternMovementInMs = 300;

ViewFinderImageWindow::ViewFinderImageWindow()
:m_uiResX(0),
 m_uiResY(0),
 m_enLinesMode(linesModeNoLines),
 m_bShowZebraPattern(false),
 m_bShowHistogram(false)
{
   SetupZebraBrush();
}

ViewFinderImageWindow::~ViewFinderImageWindow() throw()
{
   m_brushZebraPattern.DeleteObject();
}

void ViewFinderImageWindow::SetOutputType(Viewfinder::T_enOutputType enOutputType)
{
   m_spViewfinder->SetOutputType(enOutputType);
}

void ViewFinderImageWindow::EnableUpdate(bool bEnable)
{
   if (bEnable)
   {
      if (m_spViewfinder != nullptr)
         m_spViewfinder->SetAvailImageHandler(
            std::bind(&ViewFinderImageWindow::OnAvailViewfinderImage, this, std::placeholders::_1));
   }
   else
   {
      m_spViewfinder->SetAvailImageHandler();
   }
}

void ViewFinderImageWindow::SetViewfinder(std::shared_ptr<Viewfinder> spViewfinder)
{
   if (spViewfinder == nullptr && m_spViewfinder != nullptr)
   {
      m_spViewfinder->SetAvailImageHandler();
      SetBitmap(NULL);
   }

   m_spViewfinder = spViewfinder;

   if (spViewfinder != nullptr)
      EnableUpdate(true);
}

void ViewFinderImageWindow::OnAvailViewfinderImage(const std::vector<BYTE>& vecImage)
{
   if (m_spViewfinder == nullptr)
      return;

   if (vecImage.empty())
      return;

   DecodeJpegImage(vecImage);

   if (IsWindow())
      PostMessage(WM_VIEWFINDER_AVAIL_IMAGE);
}

LRESULT ViewFinderImageWindow::OnMessageViewfinderAvailImage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CBitmapHandle bmp;

   ReadBitmap(bmp);
   SetBitmap(bmp);

   // invalidate control to force redraw
   Invalidate();

   TraceViewfinderFps();

   return 0;
}

void ViewFinderImageWindow::SetupZebraBrush()
{
   const WORD c_bitsZebraPatternBrush[8] = { 0x9f, 0x3f, 0x7e, 0xfc, 0xf9, 0xf3, 0xe7, 0xcf };

   CBitmap bmZebraPattern;
   bmZebraPattern.CreateBitmap(8, 8, 1, 1, c_bitsZebraPatternBrush);

   LOGBRUSH logBrush = { 0 };
   logBrush.lbStyle = BS_PATTERN;
   logBrush.lbHatch = (ULONG_PTR)bmZebraPattern.m_hBitmap;
   logBrush.lbColor = RGB(192, 192, 192);

   m_brushZebraPattern.CreateBrushIndirect(&logBrush);
}

void ViewFinderImageWindow::DecodeJpegImage(const std::vector<BYTE>& vecImage)
{
   //DWORD dwStart = GetTickCount();
   JpegMemoryReader jpegReader(vecImage);

   try
   {
      jpegReader.Read();
   }
   catch (...)
   {
      static bool s_bWarnedAboutJPEG = false;
      if (!s_bWarnedAboutJPEG)
      {
         LOG_TRACE(_T("DecodeJpegImage: failed loading JPEG!\n"));
         s_bWarnedAboutJPEG = true;
      }
      return;
   }

   //DWORD dwEnd = GetTickCount();
   //ATLTRACE(_T("decoding jpg took %u ms\n"), dwEnd - dwStart);

   std::vector<BYTE>& vecBitmapData = jpegReader.BitmapData();

   JpegImageInfo imageInfo = jpegReader.ImageInfo();

   {
      LightweightMutex::LockType lock(m_mtxViewfinderData);
      std::swap(m_vecCurrentViewfinderData, vecBitmapData);

      m_uiResX = imageInfo.Width();
      m_uiResY = imageInfo.Height();
   }
}

void ViewFinderImageWindow::MakeOverexposedTransparent(std::vector<BYTE>& vecBitmapData)
{
   ATLASSERT((vecBitmapData.size() % 3) == 0);

   size_t uiValues = vecBitmapData.size() / 3;

   // 0xff means fully opaque
   std::vector<BYTE> vecTransparentBitmapData(uiValues * 4, 0xff);

   for (size_t ui = 0; ui < uiValues; ui++)
   {
      memcpy(&vecTransparentBitmapData[ui * 4], &vecBitmapData[ui * 3], 3);

      if (vecBitmapData[ui * 3 + 0] == 0xff ||
          vecBitmapData[ui * 3 + 1] == 0xff ||
          vecBitmapData[ui * 3 + 2] == 0xff)
      {
         vecTransparentBitmapData[ui * 4 + 3] = 0; // make transparent
      }
   }

   vecBitmapData.swap(vecTransparentBitmapData);
}

void ViewFinderImageWindow::ReadBitmap(CBitmapHandle& bmp)
{
   BITMAPINFO bi = {0};

   BITMAPINFOHEADER& bih = bi.bmiHeader;
   bih.biSize = sizeof(bih);
   bih.biBitCount = 24;
   bih.biClrUsed = 0;
   bih.biCompression = BI_RGB;
   bih.biPlanes = 1;

   bih.biHeight = -static_cast<LONG>(m_uiResY); // negative, since bytes represent a top-bottom DIB
   bih.biWidth = m_uiResX;

   BITMAPINFOHEADER* lpbmih = &bih;
   BITMAPINFO* lpbmi = &bi;

   {
      LightweightMutex::LockType lock(m_mtxViewfinderData);

      LPCVOID lpDIBBits = m_vecCurrentViewfinderData.data();

      CClientDC dc(m_hWnd);
      bmp.CreateDIBitmap(dc, lpbmih, CBM_INIT, lpDIBBits, lpbmi, DIB_RGB_COLORS);
   }
}

void ViewFinderImageWindow::SetBitmap(CBitmapHandle bmpViewfinder)
{
   m_bmpViewfinder = bmpViewfinder;
}

void ViewFinderImageWindow::TraceViewfinderFps()
{
   static unsigned int s_uiViewfinderImageCount = 0;
   static DWORD s_dwLastFpsTime = GetTickCount();

   s_uiViewfinderImageCount++;

   DWORD dwNow = GetTickCount();

   DWORD dwElapsed = dwNow - s_dwLastFpsTime;
   if (dwElapsed > 500)
   {
      CString cszText;
      cszText.Format(_T("Viewfinder: %lu fps"), s_uiViewfinderImageCount * 1000 / dwElapsed);
      ATLTRACE(_T("%s\n"), cszText.GetString());

      s_uiViewfinderImageCount = 0;
      s_dwLastFpsTime = dwNow;
   }
}

void ViewFinderImageWindow::ScaleBitmapSize(const BITMAP& bm, int& iWidth, int& iHeight)
{
   CRect rcWindow;
   GetClientRect(rcWindow);

   double dRatioWindow = rcWindow.Height() == 0 ? 1.0 : (double(rcWindow.Width()) / rcWindow.Height());
   double dRatioBitmap = bm.bmHeight == 0 ? 1.0 : (double(bm.bmWidth) / bm.bmHeight);

   if (dRatioBitmap < dRatioWindow)
      iWidth = int(iHeight * dRatioBitmap);
   else
      iHeight = int(iWidth / dRatioBitmap);
}

void ViewFinderImageWindow::DrawLines(CDC& dc, int iWidth, int iHeight)
{
   // pen color is the inverse of the background color
   int iLastDrawMode = dc.SetROP2(R2_NOTXORPEN);

   CPen pen;
   pen.CreatePen(PS_DOT, 3, RGB(0, 0, 0));
   CPenHandle oldPen = dc.SelectPen(pen);

   double dRatio1 = 1.0 / 3.0;
   if (m_enLinesMode == linesModeGoldenRatio)
      dRatio1 = c_dGoldenRatio;

   double dRatio2 = 1.0 - dRatio1;

   // horizontal lines
   dc.MoveTo(0, int(iHeight * dRatio1));
   dc.LineTo(iWidth-1, int(iHeight * dRatio1));

   dc.MoveTo(0, int(iHeight * dRatio2));
   dc.LineTo(iWidth-1,int(iHeight * dRatio2));

   // vertical lines
   dc.MoveTo(int(iWidth * dRatio1), 0);
   dc.LineTo(int(iWidth * dRatio1), iHeight-1);

   dc.MoveTo(int(iWidth * dRatio2), 0);
   dc.LineTo(int(iWidth * dRatio2), iHeight-1);

   dc.SelectPen(oldPen);

   dc.SetROP2(iLastDrawMode);
}

void ViewFinderImageWindow::DrawZebraPattern(CDC& dc, int iWidth, int iHeight)
{
   // draw zebra pattern
   HBRUSH hOldBrush = dc.SelectBrush(m_brushZebraPattern);

   dc.SetBkMode(TRANSPARENT);
   dc.SetBkColor(RGB(255, 255, 255));

   // in order to move zebra pattern, set pattern brush origin
   DWORD dwNow = GetTickCount();
   int iOffset = (dwNow / c_uiZebraPatternMovementInMs) % 8;
   dc.SetBrushOrg(iOffset, 0);

   CRect rcImage(CPoint(0, 0), CSize(iWidth, iHeight));
   dc.Rectangle(&rcImage);

   dc.SelectBrush(hOldBrush);
}

LRESULT ViewFinderImageWindow::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // disable erasing
   return FALSE;
}

LRESULT ViewFinderImageWindow::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CPaintDC dc(m_hWnd);

   if (m_bmpViewfinder.IsNull())
   {
      dc.FillSolidRect(&dc.m_ps.rcPaint, RGB(0,0,0));
      return 0;
   }

   // calculate bitmap size
   BITMAP bm = {0};
   GetObject(m_bmpViewfinder, sizeof(bm), &bm);

   CRect rcPaint(dc.m_ps.rcPaint);

   int iWidth = rcPaint.Width();
   int iHeight = rcPaint.Height();

   ScaleBitmapSize(bm, iWidth, iHeight);

   rcPaint = CRect(0, 0, iWidth, iHeight);

   // select bitmap into bitmap DC
   CDC bmpDC;
   bmpDC.CreateCompatibleDC();

   HBITMAP hbmT = bmpDC.SelectBitmap(m_bmpViewfinder);

   // draw to memory DC
   CMemoryDC memDC(dc, dc.m_ps.rcPaint);
   memDC.FillSolidRect(&dc.m_ps.rcPaint, ::GetSysColor(COLOR_3DFACE));

   // blit bitmap
   if (m_bShowZebraPattern)
   {
      DrawZebraPattern(memDC, iWidth, iHeight);

      // draw viewfinder bitmap with transparency
      memDC.TransparentBlt(0, 0, iWidth, iHeight, bmpDC, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255,255,255));
   }
   else
   {
      memDC.SetStretchBltMode(COLORONCOLOR);
      memDC.StretchBlt(0, 0, iWidth, iHeight, bmpDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
   }

   bmpDC.SelectBitmap(hbmT);

   // draw lines
   if (m_enLinesMode != linesModeNoLines)
      DrawLines(memDC, iWidth, iHeight);

   return 0;
}

LRESULT ViewFinderImageWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if (m_spViewfinder != nullptr)
   {
      m_spViewfinder->SetAvailImageHandler();
      m_spViewfinder.reset();
   }

   return 0;
}
