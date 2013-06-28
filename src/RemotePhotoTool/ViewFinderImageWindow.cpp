//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file ViewFinderImageWindow.cpp Viewfinder image window
//

// includes
#include "stdafx.h"
#include "ViewFinderImageWindow.hpp"
#include "Viewfinder.hpp"
#include "JpegMemoryReader.hpp"
#include <boost/bind.hpp>

ViewFinderImageWindow::ViewFinderImageWindow()
:m_uiResX(0),
 m_uiResY(0)
{
}

void ViewFinderImageWindow::SetViewfinder(std::shared_ptr<Viewfinder> spViewfinder)
{
   if (spViewfinder == nullptr && m_spViewfinder != nullptr)
   {
      m_spViewfinder->SetAvailImageHandler();
      SetBitmap(NULL);
   }

   m_spViewfinder = spViewfinder;

   if (m_spViewfinder != nullptr)
      m_spViewfinder->SetAvailImageHandler(
         boost::bind(&ViewFinderImageWindow::OnAvailViewfinderImage, this, _1));
}

void ViewFinderImageWindow::OnAvailViewfinderImage(const std::vector<BYTE>& vecImage)
{
   if (m_spViewfinder == nullptr)
      return;

   if (vecImage.empty())
      return;

   //{
   //   static DWORD s_dwLastCall = GetTickCount();
   //   DWORD dwNow = GetTickCount();
   //   ATLTRACE(_T("last view finder image was %u ms ago\n"), dwNow - s_dwLastCall);
   //
   //   s_dwLastCall = dwNow;
   //}

   // decode jpeg image
   {
      //DWORD dwStart = GetTickCount();
      JpegMemoryReader jpegReader(vecImage);

      try
      {
         jpegReader.Read();
      }
      catch(...)
      {
         ATLTRACE(_T("!!! failed loading JPEG!\n"));
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

   if (IsWindow())
      PostMessage(WM_VIEWFINDER_AVAIL_IMAGE);
}

LRESULT ViewFinderImageWindow::OnMessageViewfinderAvailImage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CBitmapHandle bmp;

   CreateBitmap(bmp);
   SetBitmap(bmp);

   // invalidate control to force redraw
   Invalidate();

   TraceViewfinderFps();

   return 0;
}

void ViewFinderImageWindow::CreateBitmap(CBitmapHandle& bmp)
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

      CDC dc = GetWindowDC();
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
      cszText.Format(_T("Viewfinder: %u fps"), s_uiViewfinderImageCount * 1000 / dwElapsed);
      ATLTRACE(_T("%s\n"), cszText);

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

LRESULT ViewFinderImageWindow::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CPaintDC dc(m_hWnd);

   if (m_bmpViewfinder.IsNull())
   {
      dc.FillSolidRect(&dc.m_ps.rcPaint, RGB(0,0,0));
      return 0;
   }

   // select bitmap into memory DC
   CDC memDC;
   memDC.CreateCompatibleDC();

   HBITMAP hbmT = memDC.SelectBitmap(m_bmpViewfinder);

   BITMAP bm = {0};
   GetObject(m_bmpViewfinder, sizeof(bm), &bm);

   CRect rcPaint(dc.m_ps.rcPaint);

   int iWidth = rcPaint.Width();
   int iHeight = rcPaint.Height();

   ScaleBitmapSize(bm, iWidth, iHeight);

   // blit to actual paint DC
   dc.StretchBlt(0, 0, iWidth, iHeight, memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

   memDC.SelectBitmap(hbmT);

   return 0;
}

LRESULT ViewFinderImageWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_spViewfinder.reset();
   return 0;
}
