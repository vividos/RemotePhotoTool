//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file ImagePropertyRibbonCombobox.hpp Image property ribbon combobox
//
#pragma once

// includes
#include "ImageProperty.hpp"
#include "IImagePropertyControl.hpp"
#include "RemoteReleaseControl.hpp"
#include "CameraException.hpp"
#include "CameraErrorDlg.hpp"
#include "LightweightMutex.hpp"

// forward references
class MainFrame;

/// interface to combobox placed on a ribbon
class IRibbonCombobox
{
public:
   /// dtor
   virtual ~IRibbonCombobox() throw() {}

   /// called to query category text for category number
   virtual LPCWSTR OnRibbonQueryCategoryText(UINT32 uCat) = 0;

   /// called to query item category for item
   virtual UINT32 OnRibbonQueryItemCategory(UINT32 uItem) = 0;

   /// called to query item text for item
   virtual LPCWSTR OnRibbonQueryItemText(UINT32 uItem) = 0;

   /// called to query currently selected item
   virtual bool OnRibbonQuerySelectedItem(UINT32& uSel) = 0;

   /// called to query item image for item
   virtual HBITMAP OnRibbonQueryItemImage(UINT32 uItem) = 0;
};

/// macro used in message map to specify handler for image property combobox
#define RIBBON_COMBOBOX_IMAGE_PROPERTY(wID, control) \
   RIBBON_COMBO_CONTROL_HANDLER(wID, ##control.OnRibbonComboSelChanged)

/// ribbon combobox containing image property (and possible values to choose from)
template <UINT wID, T_enImagePropertyType enImagePropertyType>
class ImagePropertyRibbonCombobox :
   public IRibbonCombobox,
   public RibbonUI::CollectionCtrlImpl<MainFrame, wID,
      RibbonUI::ComboCollectionImpl<ImagePropertyRibbonCombobox<wID, enImagePropertyType>, 500, 0>>,
   public IImagePropertyControl
{
   /// ribbon base class
   typedef RibbonUI::CollectionCtrlImpl<MainFrame, wID,
      RibbonUI::ComboCollectionImpl<ImagePropertyRibbonCombobox<wID, enImagePropertyType>, 500, 0 >> RibbonBaseClass;
public:
   /// ctor
   ImagePropertyRibbonCombobox()
      :m_enImagePropertyType(enImagePropertyType),
      m_uiPropertyId(0)
   {
   }

   /// sets remote release control
   void SetRemoteReleaseControl(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
   {
      ATLASSERT(spRemoteReleaseControl != nullptr);

      m_spRemoteReleaseControl = spRemoteReleaseControl;

      m_uiPropertyId = m_spRemoteReleaseControl->MapImagePropertyTypeToId(m_enImagePropertyType);
   }

   // virtual methods from IImagePropertyControl

   /// returns property id of image property managed by control
   virtual unsigned int GetPropertyId() override { return m_uiPropertyId; }

   /// updates list of values
   virtual void UpdateValuesList() override
   {
      ATLASSERT(m_spRemoteReleaseControl != nullptr);

      LightweightMutex::LockType lock(m_mtxValues);

      m_vecValues.clear();

      try
      {
         m_spRemoteReleaseControl->EnumImagePropertyValues(m_uiPropertyId, m_vecValues);
      }
      catch (CameraException& ex)
      {
         CameraErrorDlg dlg(_T("Couldn't enumerate values for image property"), ex);
         dlg.DoModal(GetWndRibbon());
      }

      m_vecValueTexts.clear();
      m_vecValueTexts.resize(m_vecValues.size());

      Resize(m_vecValues.size(), true);
   }

   /// updates current value
   virtual void UpdateValue() override
   {
      ATLASSERT(m_spRemoteReleaseControl != nullptr);

      CString cszPropertyName;

      try
      {
         LightweightMutex::LockType lock(m_mtxValues);

         ImageProperty val = m_spRemoteReleaseControl->GetImageProperty(m_uiPropertyId);
         cszPropertyName = val.Name();

         // no descriptions available?
         if (m_vecValues.empty())
         {
            m_vecValueTexts.resize(1);
            m_vecValueTexts[0] = val.AsString();

            Resize(m_vecValueTexts.size(), true);
            SetItemText(0, m_vecValueTexts[0], true);

            Select(0, true);
            return;
         }

         // search item in value list
         std::vector<ImageProperty>::iterator iterValues = std::find(m_vecValues.begin(), m_vecValues.end(), val);
         if (iterValues == m_vecValues.end())
            return;

         ptrdiff_t iPos = std::distance(m_vecValues.begin(), iterValues);
         ATLASSERT(iPos >= 0);

         unsigned int uiIndex = static_cast<unsigned int>(iPos);
         m_vecValueTexts[uiIndex] = val.AsString();

         SetItemText(uiIndex, m_vecValueTexts[uiIndex], true);

         Select(uiIndex, true);
      }
      catch (CameraException& ex)
      {
         CString cszText(_T("Couldn't get value for image property"));

         if (!cszPropertyName.IsEmpty())
            cszText.AppendFormat(_T(": %s"), cszPropertyName.GetString());

         CameraErrorDlg dlg(cszText, ex);
         dlg.DoModal(GetWndRibbon());
      }
   }

   // virtual methods from IRibbonCombobox

   /// called to query category text for category number
   virtual LPCWSTR OnRibbonQueryCategoryText(UINT32 /*uCat*/) override
   {
      return L"";
   }

   /// called to query item category for item
   virtual UINT32 OnRibbonQueryItemCategory(UINT32 /*uItem*/) override
   {
      return 0; // no categories
   }

   /// called to query item text for item
   virtual LPCWSTR OnRibbonQueryItemText(UINT32 uItem) override
   {
      LightweightMutex::LockType lock(m_mtxValues);

      ATLASSERT(uItem < m_vecValueTexts.size());

      if (uItem >= m_vecValueTexts.size())
         return L"";

      if (m_vecValueTexts[uItem].IsEmpty() && uItem < m_vecValues.size())
         m_vecValueTexts[uItem] = m_vecValues[uItem].AsString();

      return m_vecValueTexts[uItem];
   }

   /// called to query currently selected item
   virtual bool OnRibbonQuerySelectedItem(UINT32& uSel) override
   {
      uSel = GetSelected();
      return true;
   }

   /// called to query item image for item
   virtual HBITMAP OnRibbonQueryItemImage(UINT32 /*uItem*/) override
   {
      return nullptr;
   }

   /// called when selection has changed
   LRESULT OnRibbonComboSelChanged(UI_EXECUTIONVERB verb, WORD /*wID*/, UINT uSel, BOOL& /*bHandled*/)
   {
      if (verb == UI_EXECUTIONVERB_EXECUTE &&
         uSel != UI_COLLECTION_INVALIDINDEX &&
         m_spRemoteReleaseControl != nullptr)
      {
         LightweightMutex::LockType lock(m_mtxValues);

         if (m_vecValues.empty())
            return 0; // read-only value

         ATLASSERT(uSel < m_vecValues.size());

         ImageProperty& imageProperty = m_vecValues[uSel];

         SetImageProperty(imageProperty);
      }

      return 0;
   }

private:
   /// sets new image property
   void SetImageProperty(ImageProperty& imageProperty)
   {
      ATLASSERT(m_spRemoteReleaseControl != nullptr);

      try
      {
         m_spRemoteReleaseControl->SetImageProperty(imageProperty);
      }
      catch (CameraException& ex)
      {
         CameraErrorDlg dlg(_T("Couldn't set image property"), ex);
         dlg.DoModal(GetWndRibbon());
      }
   }

private:
   /// image property type
   T_enImagePropertyType m_enImagePropertyType;

   /// release control access
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;

   /// property id
   unsigned int m_uiPropertyId;

   /// mutex to protect m_vecValues and m_vecValueTexts
   LightweightMutex m_mtxValues;

   /// all currently possible values for this property
   std::vector<ImageProperty> m_vecValues;

   /// all texts for currently possible values
   std::vector<CString> m_vecValueTexts;
};
