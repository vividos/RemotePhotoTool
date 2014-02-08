//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file HDRPhotoModeView.hpp View for taking HDR photos
//
#pragma once

// includes
#include "IPhotoModeView.hpp"
#include "CameraException.hpp"
#include "ImagePropertyCombobox.hpp"

// forward references
class IPhotoModeViewHost;

/// message sent when next HDR image can be captured
#define WM_HDR_AEB_NEXT (WM_USER+1)

/// message sent when last HDR image was captured
#define WM_HDR_AEB_LAST (WM_USER+2)

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

   virtual void SetSourceDevice(std::shared_ptr<SourceDevice> spSourceDevice) override
   {
      m_spSourceDevice = spSourceDevice;
   }

   virtual BOOL PreTranslateMessage(MSG* pMsg) override { return CWindow::IsDialogMessage(pMsg); }

   virtual void DestroyView() override
   {
      ATLVERIFY(TRUE == DestroyWindow());
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
      MESSAGE_HANDLER(WM_HDR_AEB_NEXT, OnMessageHDRAEBNext)
      MESSAGE_HANDLER(WM_HDR_AEB_LAST, OnMessageHDRAEBLast)
      REFLECT_NOTIFICATIONS() // to make sure superclassed controls get notification messages
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnComboShutterSpeedSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnComboAEBBracketShotsSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnButtonAEB(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnMessageHDRAEBNext(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnMessageHDRAEBLast(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

   /// called when property has changed
   void OnUpdatedProperty(RemoteReleaseControl::T_enPropertyEvent enPropertyEvent, unsigned int uiValue);

   /// recalculates AEB shutter speed list
   void RecalcAEBShutterSpeedList();

   /// updates shutter speed list
   void UpdateAEBShutterSpeedList();

   /// releases remote control for first AEB image
   void ReleaseAEBFirst();

   /// releases remote control for next AEB image
   void ReleaseAEBNext();

   /// called when next AEB image has finished transfer
   void OnFinishedTransferNextAEB(const ShutterReleaseSettings& settings);

   /// called when last AEB image has finished transfer
   void OnFinishedTransferLastAEB(const ShutterReleaseSettings& settings);

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

   /// source device
   std::shared_ptr<SourceDevice> m_spSourceDevice;

   /// remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;

   /// viewfinder
   std::shared_ptr<Viewfinder> m_spViewfinder;

   /// indicates if AEB release is in progress
   bool m_bAEBInProgress;

   /// indicates if viewfinder was active before start
   bool m_bViewfinderActiveBeforeStart;

   /// indicates index of current shutter speed, from m_vecAEBShutterSpeedValues
   size_t m_uiCurrentAEBShutterSpeed;

   /// shutter speed values for AEB shots
   std::vector<ImageProperty> m_vecAEBShutterSpeedValues;

   /// filenames of bracketed shots
   std::vector<CString> m_vecAEBFilenameList;
};
