//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ImagePropertyView.hpp View for image properties
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
   ImagePropertyViewWinTraits;

/// devie property view
class ImagePropertyView :
   public CWindowImpl<ImagePropertyView, CListViewCtrl, ImagePropertyViewWinTraits>,
   public IPhotoModeView
{
   /// base class type
   typedef CWindowImpl<ImagePropertyView, CListViewCtrl, ImagePropertyViewWinTraits> BaseClass;

public:
   /// ctor
   ImagePropertyView(IPhotoModeViewHost& host) throw()
      :m_host(host),
       m_iPropertyEventId(-1)
   {
   }
   /// dtor
   virtual ~ImagePropertyView() throw() {}

   DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

private:
   /// column indices
   enum T_enImagePropertyViewColumns
   {
      columnName = 0,
      columnValue = 1,
      columnType = 2,
      columnReadOnly = 3,
      columnId = 4,
      columnRaw = 5,
   };

private:
   /// inits image property list
   bool Init();

   /// called when property changed
   void OnPropertyChanged(RemoteReleaseControl::T_enPropertyEvent enPropertyEvent, unsigned int uiPropertyId);

   /// updates single property
   void UpdateProperty(unsigned int uiImagePropertyId);

   /// refreshes property list
   void RefreshList();

   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hWndParent) override
   {
      HWND hwnd = BaseClass::Create(hWndParent, rcDefault/*, NULL, WS_VISIBLE | LVS_REPORT*/);

      if (!Init())
         return nullptr;

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

   virtual void DestroyView() override;

private:
   BEGIN_MSG_MAP(ImagePropertyView)
      MESSAGE_HANDLER(WM_RELEASECONTROL_PROPERTY_CHANGED, OnReleaseControlPropertyChanged)
      MESSAGE_HANDLER(WM_RELEASECONTROL_ALL_PROPERTIES_CHANGED, OnReleaseControlAllPropertiesChanged)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   /// called when release property has changed
   LRESULT OnReleaseControlPropertyChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      UpdateProperty(wParam);
      return 0;
   }

   /// called when all release properties have changed
   LRESULT OnReleaseControlAllPropertiesChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      RefreshList();
      return 0;
   }

private:
   /// host access
   IPhotoModeViewHost& m_host;

   /// handler id for property changes
   int m_iPropertyEventId;

   /// source device
   std::shared_ptr<SourceDevice> m_spSourceDevice;

   /// remote release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;
};
