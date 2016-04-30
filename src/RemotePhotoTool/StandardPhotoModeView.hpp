//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file StandardPhotoModeView.hpp View for taking standard photos
//
#pragma once

// includes
#include "IPhotoModeView.hpp"
#include "CameraException.hpp"
#include "ImagePropertyValueManager.hpp"
#include "ImagePropertyCombobox.hpp"

// forward references
class IPhotoModeViewHost;

/// view for a default and simple standard photo mode
class StandardPhotoModeView :
   public CDialogImpl<StandardPhotoModeView>,
   public CDialogResize<StandardPhotoModeView>,
   public CWinDataExchange<StandardPhotoModeView>,
   public IPhotoModeView
{
public:
   /// ctor
   StandardPhotoModeView(IPhotoModeViewHost& host) throw();
   /// dtor
   virtual ~StandardPhotoModeView() throw() {}

   /// dialog id
   enum { IDD = IDD_PHOTOMODE_STANDARD_FORM };

private:
   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hWndParent) override
   {
      return CDialogImpl<StandardPhotoModeView>::Create(hWndParent);
   }

   virtual BOOL PreTranslateMessage(MSG* pMsg) override { return CWindow::IsDialogMessage(pMsg); }

   virtual void DestroyView() override
   {
      BOOL bRet = DestroyWindow();
      ATLASSERT(TRUE == bRet); bRet;
   }

private:
   // ddx map
   BEGIN_DDX_MAP(StandardPhotoModeView)
      DDX_CONTROL(IDC_COMBO_SHOOTING_MODE, m_cbShootingMode)
      DDX_CONTROL(IDC_COMBO_APERTURE, m_cbAperture)
      DDX_CONTROL(IDC_COMBO_SHUTTER_SPEED, m_cbShutterSpeed)
      DDX_CONTROL(IDC_COMBO_EXPOSURE_COMP, m_cbExposureComp)
      DDX_CONTROL(IDC_COMBO_WHITE_BALANCE, m_cbWhiteBalance)
   END_DDX_MAP()

   // message map
   BEGIN_MSG_MAP(StandardPhotoModeView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      COMMAND_HANDLER(IDC_COMBO_SHOOTING_MODE, CBN_SELCHANGE, OnShootingModeSelChange)
      COMMAND_ID_HANDLER(IDC_BUTTON_RELEASE, OnButtonRelease)
      COMMAND_ID_HANDLER(ID_CAMERA_RELEASE, OnButtonRelease)
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
   /// called when selected shooting mode is being changed
   LRESULT OnShootingModeSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   /// called when Release button is pressed
   LRESULT OnButtonRelease(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when property has changed
   void OnUpdatedProperty(RemoteReleaseControl::T_enPropertyEvent enPropertyEvent, unsigned int uiValue);

   /// called when image has finished transfer
   void OnFinishedTransfer(const ShutterReleaseSettings& settings);

   /// sets up ImagePropertyValueManager
   void SetupImagePropertyManager();

   /// updates values that depend on shooting mode changes
   void UpdateShootingModeDependentValues();

private:
   /// host access
   IPhotoModeViewHost& m_host;

   // model

   /// remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;

   // UI

   /// image property value manager
   std::unique_ptr<ImagePropertyValueManager> m_upImagePropertyValueManager;

   /// shooting mode
   ImagePropertyCombobox m_cbShootingMode;

   /// aperture
   ImagePropertyCombobox m_cbAperture;

   /// shutter speed
   ImagePropertyCombobox m_cbShutterSpeed;

   /// exposure compensation
   ImagePropertyCombobox m_cbExposureComp;

   /// white balance
   ImagePropertyCombobox m_cbWhiteBalance;

   /// id for property handler
   int m_iPropertyHandlerId;
};
