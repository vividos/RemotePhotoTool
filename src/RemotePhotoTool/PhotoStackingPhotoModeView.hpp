//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file PhotoStackingPhotoModeView.hpp Photo stacking photo mode view
//
#pragma once

// includes
#include "IPhotoModeView.hpp"

// forward references
class IPhotoModeViewHost;

/// photo stacking photo mode view
class PhotoStackingPhotoModeView :
   public CDialogImpl<PhotoStackingPhotoModeView>,
   public CDialogResize<PhotoStackingPhotoModeView>,
   public CWinDataExchange<PhotoStackingPhotoModeView>,
   public IPhotoModeView
{
public:
   /// ctor
   PhotoStackingPhotoModeView(IPhotoModeViewHost& host);
   /// dtor
   virtual ~PhotoStackingPhotoModeView() {}

   /// dialog id
   enum { IDD = IDD_PHOTOMODE_PHOTO_STACKING_FORM };

private:
   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hWndParent) override
   {
      return CDialogImpl<PhotoStackingPhotoModeView>::Create(hWndParent);
   }

   virtual BOOL PreTranslateMessage(MSG* pMsg) override { return CWindow::IsDialogMessage(pMsg); }

   virtual void DestroyView() override
   {
      ATLVERIFY(TRUE == DestroyWindow());
   }

private:
   // ddx map
   BEGIN_DDX_MAP(PhotoStackingPhotoModeView)
   END_DDX_MAP()

   // message map
   BEGIN_MSG_MAP(PhotoStackingPhotoModeView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      REFLECT_NOTIFICATIONS() // to make sure superclassed controls get notification messages
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
// LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)

   /// called when dialog is being shown
   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called at destruction of dialog
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
   /// host access
   IPhotoModeViewHost& m_host;

   // model

   /// remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;

   // UI

};
