//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file HDRPhotoModeView.hpp View for taking HDR photos
//
#pragma once

// includes
#include "IPhotoModeView.hpp"
#include "CameraException.hpp"
#include "ImagePropertyCombobox.hpp"
#include "PhotoModeManager.hpp"

// forward references
class IPhotoModeViewHost;

/// view for HDR photo mode
class HDRPhotoModeView :
   public CDialogImpl<HDRPhotoModeView>,
   public CDialogResize<HDRPhotoModeView>,
   public CWinDataExchange<HDRPhotoModeView>,
   public IPhotoModeView
{
public:
   /// ctor
   HDRPhotoModeView(IPhotoModeViewHost& host) throw();
   /// dtor
   virtual ~HDRPhotoModeView() throw() {}

   enum { IDD = IDD_PHOTOMODE_HDR_FORM };

private:
   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hWndParent) override
   {
      return CDialogImpl<HDRPhotoModeView>::Create(hWndParent);
   }

   virtual BOOL PreTranslateMessage(MSG* pMsg) override { return CWindow::IsDialogMessage(pMsg); }

   virtual void DestroyView() override
   {
      BOOL bRet = DestroyWindow();
      ATLASSERT(TRUE == bRet); bRet;
   }

private:
   // ddx map
   BEGIN_DDX_MAP(HDRPhotoModeView)
      DDX_CONTROL_HANDLE(IDC_COMBO_AEB_BRACKET_SHOTS, m_cbAEBBracketedShots)
      DDX_CONTROL_HANDLE(IDC_LIST_AEB_SHUTTER_SPEED_VALUES, m_lcAEBShutterSpeedValues)
      DDX_CONTROL(IDC_COMBO_SHUTTER_SPEED, m_cbShutterSpeed)
   END_DDX_MAP()

   // message map
   BEGIN_MSG_MAP(HDRPhotoModeView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      COMMAND_HANDLER(IDC_COMBO_SHUTTER_SPEED, CBN_SELCHANGE, OnComboShutterSpeedSelChange)
      COMMAND_HANDLER(IDC_COMBO_AEB_BRACKET_SHOTS, CBN_SELCHANGE, OnComboAEBBracketShotsSelChange)
      COMMAND_ID_HANDLER(IDC_BUTTON_AEB, OnButtonAEB)
      COMMAND_ID_HANDLER(ID_CAMERA_RELEASE, OnButtonAEB)
      MESSAGE_HANDLER(WM_HDR_AEB_NEXT, OnMessageHDRAEBNext)
      MESSAGE_HANDLER(WM_HDR_AEB_LAST, OnMessageHDRAEBLast)
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
   /// called when shutter speed selection in combobox changes
   LRESULT OnComboShutterSpeedSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when selected number of AEB bracket shots changes
   LRESULT OnComboAEBBracketShotsSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when AEB button is pressed
   LRESULT OnButtonAEB(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when custom message "AEB next" is received
   LRESULT OnMessageHDRAEBNext(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   /// called when custom message "AEB last" is received
   LRESULT OnMessageHDRAEBLast(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// called when property has changed
   void OnUpdatedProperty(RemoteReleaseControl::T_enPropertyEvent enPropertyEvent, unsigned int uiValue);

   /// updates shutter speed list
   void UpdateAEBShutterSpeedList();

private:
   /// host access
   IPhotoModeViewHost& m_host;

   // UI

   /// shutter speed combobox
   ImagePropertyCombobox m_cbShutterSpeed;

   /// combobox with number of bracket shots
   CComboBox m_cbAEBBracketedShots;

   /// list of AEB shutter speed values
   CListViewCtrl m_lcAEBShutterSpeedValues;

   /// handler id for property callback
   int m_iPropertyHandlerId;

   // model

   /// photo mode manager
   HDRPhotoModeManager m_manager;

   /// remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;
};
