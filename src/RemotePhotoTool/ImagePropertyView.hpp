//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file ImagePropertyView.hpp View for image properties
//
#pragma once

#include "IPhotoModeView.hpp"
#include "RemoteReleaseControl.hpp"

class IPhotoModeViewHost;
class SourceDevice;

/// win traits for device property view
typedef CWinTraitsOR<LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE, CControlWinTraits>
ImagePropertyViewWinTraits;

/// image property view
class ImagePropertyView :
   public CWindowImpl<ImagePropertyView, CListViewCtrl, ImagePropertyViewWinTraits>,
   public IPhotoModeView
{
   /// base class type
   typedef CWindowImpl<ImagePropertyView, CListViewCtrl, ImagePropertyViewWinTraits> BaseClass;

public:
   /// ctor
   ImagePropertyView(IPhotoModeViewHost& host)
      :m_host(host),
      m_propertyEventId(-1)
   {
   }
   /// dtor
   virtual ~ImagePropertyView() {}

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
   void OnPropertyChanged(RemoteReleaseControl::T_enPropertyEvent propertyEvent, unsigned int propertyId);

   /// updates single property
   void UpdateProperty(unsigned int imagePropertyId);

   /// refreshes property list
   void RefreshList();

   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hwndParent) override
   {
      HWND hwnd = BaseClass::Create(hwndParent, rcDefault);

      if (!Init())
         return nullptr;

      return hwnd;
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
   // LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   // LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)

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
   int m_propertyEventId;

   /// remote release control
   std::shared_ptr<RemoteReleaseControl> m_remoteReleaseControl;
};
