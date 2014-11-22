//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PreviousImagesView.hpp Previous images view
//
#pragma once

// includes
#include "IPhotoModeView.hpp"
#include "RemoteReleaseControl.hpp"

// forward references
class IPhotoModeViewHost;
class SourceDevice;

/// win traits for device property view
typedef CWinTraitsOR<LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE, CControlWinTraits>
   PreviousImagesViewWinTraits;

/// devie property view
class PreviousImagesView :
   public CWindowImpl<PreviousImagesView, CListViewCtrl, PreviousImagesViewWinTraits>,
   public IPhotoModeView
{
   /// base class type
   typedef CWindowImpl<PreviousImagesView, CListViewCtrl, PreviousImagesViewWinTraits> BaseClass;

public:
   /// ctor
   PreviousImagesView(IPhotoModeViewHost& host) throw()
      :m_host(host)
   {
   }
   /// dtor
   virtual ~PreviousImagesView() throw() {}

   DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

private:
   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hWndParent) override
   {
      HWND hwnd = BaseClass::Create(hWndParent, rcDefault);

      return hwnd;
   }

   virtual void SetSourceDevice(std::shared_ptr<SourceDevice> /*spSourceDevice*/) override
   {
   }

   virtual BOOL PreTranslateMessage(MSG* /*pMsg*/) override
   {
      return FALSE;
   }

   virtual void DestroyView() override;

private:
   BEGIN_MSG_MAP(PreviousImagesView)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

private:
   // UI

   /// host access
   IPhotoModeViewHost& m_host;

   // Model

};
