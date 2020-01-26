//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file PreviousImagesView.cpp Previous images view
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "PreviousImagesView.hpp"
#include "IPhotoModeViewHost.hpp"
#include <ulib/Path.hpp>
#include <shellapi.h>

/// padding pixels between image and image info
const unsigned int c_uiPaddingImageInfo = 10;

PreviousImagesView::PreviousImagesView(IPhotoModeViewHost& host)
:m_host(host),
m_manager(host.GetPreviousImagesManager())
{
}

HWND PreviousImagesView::CreateView(HWND hWndParent)
{
   HWND hwnd = BaseClass::Create(hWndParent, rcDefault);

   // add us as idle handler
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != nullptr);
   pLoop->AddIdleHandler(this);

   StartGetImage(PreviousImagesManager::imageTypeLast);

   return hwnd;
}

void PreviousImagesView::DestroyView()
{
   // remove idle handler
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != nullptr);
   pLoop->RemoveIdleHandler(this);

   ATLVERIFY(TRUE == DestroyWindow());
}

BOOL PreviousImagesView::OnIdle()
{
   bool isImageAvail = m_spCurrentImage != nullptr;

   m_host.EnableUI(ID_PREV_IMAGES_OPEN_FOLDER, isImageAvail);
   m_host.EnableUI(ID_PREV_IMAGES_EDIT, isImageAvail);
   m_host.EnableUI(ID_PREV_IMAGES_PREV, isImageAvail && !m_manager.IsFirstImage(m_spCurrentImage));
   m_host.EnableUI(ID_PREV_IMAGES_NEXT, isImageAvail && !m_manager.IsLastImage(m_spCurrentImage));

   return FALSE;
}

LRESULT PreviousImagesView::OnPrevImagesPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   StartGetImage(PreviousImagesManager::imageTypePrevious);
   return 0;
}

LRESULT PreviousImagesView::OnPrevImagesNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   StartGetImage(PreviousImagesManager::imageTypeNext);
   return 0;
}

LRESULT PreviousImagesView::OnPrevImagesOpenFolder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_spCurrentImage != nullptr)
      OpenFolder(m_spCurrentImage->Filename());

   return 0;
}

LRESULT PreviousImagesView::OnPrevImagesEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_spCurrentImage != nullptr)
      EditImage(m_spCurrentImage->Filename());

   return 0;
}

/// \details handles WM_PREV_IMAGES_UPDATE message
LRESULT PreviousImagesView::OnMessageUpdatedCurrentImage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   UpdateCurrentImage();

   return 0;
}

LRESULT PreviousImagesView::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // disable erasing
   return FALSE;
}

LRESULT PreviousImagesView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CPaintDC dc(m_hWnd);

   if (m_bmpPreviousImage.IsNull())
   {
      DrawEmptyView(dc);
      return 0;
   }

   int iWidth = 0;
   DrawCurrentImage(dc, iWidth);
   DrawImageInfos(dc, iWidth);

   return 0;
}

void PreviousImagesView::DrawEmptyView(CPaintDC& dc)
{
   DWORD dwColor = ::GetSysColor(COLOR_APPWORKSPACE);
   dc.FillSolidRect(&dc.m_ps.rcPaint, dwColor);

   // output "no image" string
   CRect rcClient;
   GetClientRect(&rcClient);

   HFONT oldFont = dc.SelectFont(GetFont());

   dc.SetBkColor(dwColor);
   dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

   CString cszText = m_manager.ImagesAvail() ? _T("<Loading image...>") : _T("<No images available>");

   // add DT_SINGLELINE to center both vertically and horizontally
   dc.DrawText(cszText, cszText.GetLength(), rcClient, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

   dc.SelectFont(oldFont);
}

void PreviousImagesView::DrawCurrentImage(CPaintDC& dc, int& iWidth)
{
   // calculate bitmap size
   CRect rcPaint;
   GetClientRect(&rcPaint);

   BITMAP bm = { 0 };
   GetObject(m_bmpPreviousImage, sizeof(bm), &bm);

   iWidth = rcPaint.Width();
   int iHeight = rcPaint.Height();

   ScaleBitmapSize(bm, iWidth, iHeight);

   // select bitmap into bitmap DC
   CDC bmpDC;
   bmpDC.CreateCompatibleDC(dc);

   HBITMAP hbmT = bmpDC.SelectBitmap(m_bmpPreviousImage);

   // draw to memory DC
   CMemoryDC memDC(dc, dc.m_ps.rcPaint);
   memDC.FillSolidRect(&dc.m_ps.rcPaint, ::GetSysColor(COLOR_3DFACE));

   // blit bitmap
   memDC.SetStretchBltMode(COLORONCOLOR);
   memDC.StretchBlt(0, 0, iWidth, iHeight, bmpDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

   bmpDC.SelectBitmap(hbmT);
}

void PreviousImagesView::DrawImageInfos(CPaintDC& dc, int iWidth)
{
   std::shared_ptr<PreviousImageInfo> spCurrentImage = m_spCurrentImage;

   if (spCurrentImage == nullptr)
      return;

   CRect rcPaint;
   GetClientRect(&rcPaint);
   rcPaint.left = iWidth + c_uiPaddingImageInfo;
   rcPaint.top += c_uiPaddingImageInfo;

   HFONT oldFont = dc.SelectFont(GetFont());

   dc.SetBkMode(TRANSPARENT);
   dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

   CString cszText;
   cszText.Format(
      _T("Filename: %s\n")
      _T("Av: %s\n")
      _T("Tv: %s\n")
      _T("ISO: %s\n")
      _T("Zoom: %s\n")
      _T("Date: %s\n")
      _T("%s"),
      spCurrentImage->Filename().GetString(),
      spCurrentImage->InfoText(PreviousImageInfo::typeAperture).GetString(),
      spCurrentImage->InfoText(PreviousImageInfo::typeShutterSpeed).GetString(),
      spCurrentImage->InfoText(PreviousImageInfo::typeIsoSetting).GetString(),
      spCurrentImage->InfoText(PreviousImageInfo::typeFocalLength).GetString(),
      spCurrentImage->InfoText(PreviousImageInfo::typeDateTime).GetString(),
      spCurrentImage->InfoText(PreviousImageInfo::typeFlashFired).GetString());

   dc.DrawText(cszText, cszText.GetLength(), rcPaint, DT_LEFT | DT_TOP);

   dc.SelectFont(oldFont);
}

void PreviousImagesView::StartGetImage(PreviousImagesManager::T_enRequestImageType enRequestImageType)
{
   m_manager.AsyncGetImage(
      enRequestImageType,
      m_spCurrentImage, // ignored for imageTypeLast
      std::bind(&PreviousImagesView::OnUpdatedCurrentImage, this, std::placeholders::_1));
}

/// \note: This may not run in UI thread, so this just posts a message to the UI thread
void PreviousImagesView::OnUpdatedCurrentImage(std::shared_ptr<PreviousImageInfo> spImageInfo)
{
   m_spCurrentImage = spImageInfo;

   PostMessage(WM_PREV_IMAGES_UPDATE);
}

void PreviousImagesView::UpdateCurrentImage()
{
   std::shared_ptr<PreviousImageInfo> spCurrentImage = m_spCurrentImage;

   if (!m_bmpPreviousImage.IsNull())
      m_bmpPreviousImage.DeleteObject();

   BITMAPINFO bi = { 0 };

   BITMAPINFOHEADER& bih = bi.bmiHeader;
   bih.biSize = sizeof(bih);
   bih.biBitCount = 24;
   bih.biClrUsed = 0;
   bih.biCompression = BI_RGB;
   bih.biPlanes = 1;

   bih.biHeight = -static_cast<LONG>(spCurrentImage->Height()); // negative, since bytes represent a top-bottom DIB
   bih.biWidth = spCurrentImage->Width();

   BITMAPINFOHEADER* lpbmih = &bih;
   BITMAPINFO* lpbmi = &bi;

   LPCVOID lpDIBBits = spCurrentImage->BitmapData().data();

   CClientDC dc(m_hWnd);
   m_bmpPreviousImage.CreateDIBitmap(dc, lpbmih, CBM_INIT, lpDIBBits, lpbmi, DIB_RGB_COLORS);

   Invalidate(true);
}

void PreviousImagesView::ScaleBitmapSize(const BITMAP& bm, int& iWidth, int& iHeight)
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

/// \see http://stackoverflow.com/questions/3010305/programmatically-selecting-file-in-explorer
void PreviousImagesView::OpenFolder(const CString& cszImageFilename)
{
   ITEMIDLIST* pidl = ILCreateFromPath(cszImageFilename);
   if (pidl != nullptr)
   {
      SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
      ILFree(pidl);

      return;
   }

   // workaround for systems where ILCreateFromPath() fails; opens a new explorer
   // window each time it is called.
   CString cszFolderName = Path::FolderName(cszImageFilename);

   CString cszArgs;
   cszArgs.Format(_T("/select, \"%s\""), cszImageFilename.GetString());
   ::ShellExecute(m_hWnd, _T("open"), _T("explorer.exe"), cszArgs, cszFolderName, SW_SHOWNORMAL);
}

void PreviousImagesView::EditImage(const CString& cszImageFilename)
{
   CString cszFolderName = Path::FolderName(cszImageFilename);

   ::ShellExecute(m_hWnd, _T("edit"), cszImageFilename, NULL, cszFolderName, SW_SHOWNORMAL);
}
