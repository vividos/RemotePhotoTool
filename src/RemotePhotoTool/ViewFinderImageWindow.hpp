//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file ViewFinderImageWindow.hpp Viewfinder image window
//
#pragma once

// includes
#include "LightweightMutex.hpp"

// forward references
class Viewfinder;

/// image control for viewfinder image
class ViewFinderImageWindow: public CWindowImpl<ViewFinderImageWindow>
{
public:
   /// ctor
   ViewFinderImageWindow();

   DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW | CS_VREDRAW, COLOR_APPWORKSPACE)

private:
   BEGIN_MSG_MAP(ViewFinderImageWindow)
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

   /// sets new bitmap
   void SetBitmap(CBitmapHandle bmpViewfinder);

private:
// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

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
};
