//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PanoramaPhotoModeView.hpp View for taking panorama photos
//
#pragma once

// includes
#include "IPhotoModeView.hpp"
#include "CameraException.hpp"

// forward references
class IPhotoModeViewHost;
class SourceDevice;
class ShutterReleaseSettings;
class RemoteReleaseControl;

/// view for HDR photo mode
class PanoramaPhotoModeView :
   public CDialogImpl<PanoramaPhotoModeView>,
   public CDialogResize<PanoramaPhotoModeView>,
   public CWinDataExchange<PanoramaPhotoModeView>,
   public IPhotoModeView
{
public:
   /// ctor
   PanoramaPhotoModeView(IPhotoModeViewHost& host) throw();
   /// dtor
   virtual ~PanoramaPhotoModeView() throw() {}

   /// dialog id
   enum { IDD = IDD_PHOTOMODE_PANORAMA_FORM };

private:
   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hWndParent) override
   {
      return CDialogImpl<PanoramaPhotoModeView>::Create(hWndParent);
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
   BEGIN_DDX_MAP(PanoramaPhotoModeView)
      DDX_CONTROL_HANDLE(IDC_BUTTON_PANORAMA_START, m_btnStart)
      DDX_CONTROL_HANDLE(IDC_BUTTON_PANORAMA_STOP, m_btnStop)
      DDX_CONTROL_HANDLE(IDC_BUTTON_PANORAMA_CANCEL, m_btnCancel)
   END_DDX_MAP()

   BEGIN_MSG_MAP(PanoramaPhotoModeView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      COMMAND_ID_HANDLER(IDC_BUTTON_PANORAMA_START, OnButtonStart)
      COMMAND_ID_HANDLER(IDC_BUTTON_PANORAMA_STOP, OnButtonStop)
      COMMAND_ID_HANDLER(IDC_BUTTON_PANORAMA_CANCEL, OnButtonCancel)
      REFLECT_NOTIFICATIONS() // to make sure superclassed controls get notification messages
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnButtonStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnButtonStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnButtonCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   /// starts taking panorama
   void StartPanorama();

   /// stops taking panorama
   void StopPanorama();

   /// starts hugin
   void StartHugin();

   /// does remote release for panorama image
   void ReleasePanorama();

   /// called when image transfer is finished
   void OnFinishedTransfer(const ShutterReleaseSettings& settings);

private:
   /// host access
   IPhotoModeViewHost& m_host;

   // UI

   /// panorama started?
   bool m_bStarted;

   /// start/next button
   CButton m_btnStart;

   /// stop button
   CButton m_btnStop;

   /// cancel button
   CButton m_btnCancel;

   // model

   /// source device
   std::shared_ptr<SourceDevice> m_spSourceDevice;

   /// remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;

   /// filenames of panorama shots
   std::vector<CString> m_vecPanoramaFilenameList;
};
