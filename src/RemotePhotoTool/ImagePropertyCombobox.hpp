//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file ImagePropertyCombobox.hpp Image property combobox
//
#pragma once

// includes
#include "IImagePropertyControl.hpp"
#include "RemoteReleaseControl.hpp"

/// combobox containing image property (and possible values to choose from)
class ImagePropertyCombobox :
   public CWindowImpl<ImagePropertyCombobox, CComboBox>,
   public IImagePropertyControl
{
public:
   /// ctor
   ImagePropertyCombobox(T_enImagePropertyType enImagePropertyType)
      :m_enImagePropertyType(enImagePropertyType),
       m_uiPropertyId(0)
   {
   }

   DECLARE_WND_SUPERCLASS(NULL, WC_COMBOBOX)

   /// sets remote release control
   void SetRemoteReleaseControl(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   // virtual methods from IImagePropertyControl
   
   /// returns property id of image property managed by control
   virtual unsigned int GetPropertyId() override { return m_uiPropertyId; }

   /// updates list of values
   virtual void UpdateValuesList() override;

   /// updates current value
   virtual void UpdateValue() override;

private:
   BEGIN_MSG_MAP(ImagePropertyCombobox)
      COMMAND_CODE_HANDLER(CBN_SELCHANGE, OnSelChange)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   /// called when combobox selection changes; note: sent as command notification
   LRESULT OnSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
   /// image property type
   T_enImagePropertyType m_enImagePropertyType;

   /// release control access
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;

   /// property id
   unsigned int m_uiPropertyId;

   /// all currently possible values for this property
   std::vector<ImageProperty> m_vecValues;

   /// mapping from value array to combobox index
   std::map<size_t, int> m_mapValueIndex;
};
