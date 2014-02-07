//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file DevicePropertyView.hpp View for device properties
//
#pragma once

// includes
#include "IPhotoModeView.hpp"

// forward references
class IPhotoModeViewHost;
class SourceDevice;

/// win traits for device property view
typedef CWinTraitsOR<LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE, CControlWinTraits>
   DevicePropertyViewWinTraits;

/// devie property view
class DevicePropertyView :
   public CWindowImpl<DevicePropertyView, CListViewCtrl, DevicePropertyViewWinTraits>,
   public IPhotoModeView
{
   /// base class type
   typedef CWindowImpl<DevicePropertyView, CListViewCtrl, DevicePropertyViewWinTraits> BaseClass;

public:
   /// ctor
   DevicePropertyView(IPhotoModeViewHost& host) throw()
      :m_host(host) {}
   /// dtor
   virtual ~DevicePropertyView() throw() {}

   DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

private:
   /// column indices
   enum T_enDevicePropertyViewColumns
   {
      columnName = 0,
      columnValue = 1,
      columnType = 2,
      columnReadOnly = 3,
      columnId = 4,
   };

private:
   /// inits view
   void Init();

   /// refreshes property list
   void RefreshList();

   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hWndParent) override
   {
      HWND hwnd = BaseClass::Create(hWndParent, rcDefault/*, NULL, WS_VISIBLE | LVS_REPORT*/);

      Init();

      return hwnd;
   }

   virtual void SetSourceDevice(std::shared_ptr<SourceDevice> spSourceDevice) override
   {
      m_spSourceDevice = spSourceDevice;
   }

   virtual BOOL PreTranslateMessage(MSG* /*pMsg*/) override
   {
      return FALSE;
   }

   virtual void DestroyView() override
   {
      ATLVERIFY(TRUE == DestroyWindow());
   }

private:
   BEGIN_MSG_MAP(DevicePropertyView)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

private:
   /// host access
   IPhotoModeViewHost& m_host;

   /// source device
   std::shared_ptr<SourceDevice> m_spSourceDevice;
};
