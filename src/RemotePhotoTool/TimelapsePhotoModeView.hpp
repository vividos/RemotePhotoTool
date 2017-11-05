//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file TimeLapsePhotoModeView.hpp Timelapse photo mode view
//
#pragma once

// includes
#include "IPhotoModeView.hpp"

// forward references
class IPhotoModeViewHost;

/// timelapse photo mode view
class TimeLapsePhotoModeView :
   public CDialogImpl<TimeLapsePhotoModeView>,
   public CDialogResize<TimeLapsePhotoModeView>,
   public CWinDataExchange<TimeLapsePhotoModeView>,
   public IPhotoModeView
{
public:
   /// ctor
   TimeLapsePhotoModeView(IPhotoModeViewHost& host);
   /// dtor
   virtual ~TimeLapsePhotoModeView() {}

   /// dialog id
   enum { IDD = IDD_PHOTOMODE_TIMELAPSE_FORM };

private:
   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hWndParent) override
   {
      return CDialogImpl<TimeLapsePhotoModeView>::Create(hWndParent);
   }

   virtual BOOL PreTranslateMessage(MSG* pMsg) override
   {
      return CWindow::IsDialogMessage(pMsg);
   }

   virtual void DestroyView() override
   {
      ATLVERIFY(TRUE == DestroyWindow());
   }

private:
   // ddx map
   BEGIN_DDX_MAP(TimeLapsePhotoModeView)
   END_DDX_MAP()

   // message map
   BEGIN_MSG_MAP(TimeLapsePhotoModeView)
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

   /// source device
   std::shared_ptr<SourceDevice> m_spSourceDevice;

   /// remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;

   // UI

};
