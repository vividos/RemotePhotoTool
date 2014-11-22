//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ScriptingPhotoModeView.hpp  Scripting photo mode view
//
#pragma once

// includes
#include "IPhotoModeView.hpp"

// forward references
class IPhotoModeViewHost;

/// view for scripting photo mode view
class ScriptingPhotoModeView :
   public CDialogImpl<ScriptingPhotoModeView>,
   public CDialogResize<ScriptingPhotoModeView>,
   public CWinDataExchange<ScriptingPhotoModeView>,
   public IPhotoModeView
{
public:
   /// ctor
   ScriptingPhotoModeView(IPhotoModeViewHost& host) throw();
   /// dtor
   virtual ~ScriptingPhotoModeView() throw() {}

   /// dialog id
   enum { IDD = IDD_PHOTOMODE_STANDARD_FORM };

private:
   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hWndParent) override
   {
      return CDialogImpl<ScriptingPhotoModeView>::Create(hWndParent);
   }

   virtual void SetSourceDevice(std::shared_ptr<SourceDevice> spSourceDevice) override
   {
      m_spSourceDevice = spSourceDevice;
   }

   virtual BOOL PreTranslateMessage(MSG* pMsg) override { return CWindow::IsDialogMessage(pMsg); }

   virtual void DestroyView() override
   {
      BOOL bRet = DestroyWindow();
      ATLASSERT(TRUE == bRet); bRet;
   }

private:
   // ddx map
   BEGIN_DDX_MAP(ScriptingPhotoModeView)
   END_DDX_MAP()

   // message map
   BEGIN_MSG_MAP(ScriptingPhotoModeView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      REFLECT_NOTIFICATIONS() // to make sure superclassed controls get notification messages
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   /// called when dialog is being shown
   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   /// called at destruction of dialog
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);


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
