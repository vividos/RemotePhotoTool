//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file BlankView.hpp Blank view
//
#pragma once

// includes
#include "IPhotoModeView.hpp"

/// blank view, used when no camera is connected
class BlankView : public CWindowImpl<BlankView>, public IPhotoModeView
{
public:
   /// dtor
   virtual ~BlankView() throw() {}

   DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW | CS_VREDRAW, COLOR_APPWORKSPACE)

   virtual HWND CreateView(HWND hWndParent) override
   {
      return CWindowImpl<BlankView>::Create(hWndParent, rcDefault, NULL,
         WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
   }

   virtual void SetSourceDevice(std::shared_ptr<SourceDevice> spSourceDevice) override
   {
      spSourceDevice;
   }

   virtual BOOL PreTranslateMessage(MSG* pMsg) override
   {
      pMsg;
      return FALSE;
   }

   virtual void DestroyView() override
   {
      ATLVERIFY(TRUE == DestroyWindow());
   }

private:
   BEGIN_MSG_MAP(BlankView)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   /// called to paint view
   LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      CPaintDC dc(m_hWnd);
      return 0;
   }
};
