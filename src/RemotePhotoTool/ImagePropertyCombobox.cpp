//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ImagePropertyCombobox.cpp Image property combobox
//

// includes
#include "stdafx.h"
#include "ImagePropertyCombobox.hpp"
#include "CameraException.hpp"
#include "CameraErrorDlg.hpp"

void ImagePropertyCombobox::SetRemoteReleaseControl(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
{
   ATLASSERT(spRemoteReleaseControl != nullptr);

   m_spRemoteReleaseControl = spRemoteReleaseControl;

   m_uiPropertyId = m_spRemoteReleaseControl->MapImagePropertyTypeToId(m_enImagePropertyType);
}

void ImagePropertyCombobox::UpdateValuesList()
{
   ATLASSERT(m_spRemoteReleaseControl != nullptr);

   SetRedraw(FALSE);
   ResetContent();

   m_mapValueIndex.clear();
   m_vecValues.clear();


   try
   {
      m_spRemoteReleaseControl->EnumImagePropertyValues(m_uiPropertyId, m_vecValues);
   }
   catch (const CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't enumerate values for image property"), ex);
      dlg.DoModal(m_hWnd);
   }

   bool bReadOnly = false;
   int iSelected = -1;

   try
   {
      ImageProperty currentVal = m_spRemoteReleaseControl->GetImageProperty(m_uiPropertyId);

      if (m_vecValues.empty())
      {
         // when empty, treat as read-only
         bReadOnly = true;
      }
      else
      {
         // check if the property is currently read only
         bReadOnly = currentVal.IsReadOnly();
      }

      for (size_t i=0, iMax=m_vecValues.size(); i<iMax; i++)
      {
         const ImageProperty& val = m_vecValues[i];
         int iItem = AddString(val.AsString());
         SetItemData(iItem, i);

         if (val == currentVal)
            iSelected = iItem;

         m_mapValueIndex.insert(std::make_pair(i, iItem));
      }
   }
   catch (const CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't get current value for image property"), ex);
      dlg.DoModal(m_hWnd);
   }

   // select item, in case control doesn't also get a UpdateValue() call
   if (iSelected != -1)
      SetCurSel(iSelected);

   EnableWindow(bReadOnly ? FALSE : TRUE);

   SetRedraw(TRUE);
}

void ImagePropertyCombobox::UpdateValue()
{
   ATLASSERT(m_spRemoteReleaseControl != nullptr);

   CString cszPropertyName;

   try
   {
      ImageProperty val = m_spRemoteReleaseControl->GetImageProperty(m_uiPropertyId);
      cszPropertyName = val.Name();

      // no descriptions available?
      if (m_vecValues.empty())
      {
         ResetContent();
         AddString(val.AsString());
         SetCurSel(0);

         return;
      }

      // search item in value list
      std::vector<ImageProperty>::iterator iterValues = std::find(m_vecValues.begin(), m_vecValues.end(), val);
      if (iterValues == m_vecValues.end())
         return;

      ptrdiff_t iPos = std::distance(m_vecValues.begin(), iterValues);
      ATLASSERT(iPos >= 0);

      // search cb index
      std::map<size_t, int>::const_iterator iterMap = m_mapValueIndex.find(iPos);
      if (iterMap == m_mapValueIndex.end())
         return;

      SetCurSel(iterMap->second);
   }
   catch (const CameraException& ex)
   {
      CString cszText(_T("Couldn't get value for image property"));

      if (!cszPropertyName.IsEmpty())
         cszText.AppendFormat(_T(": %s"), cszPropertyName.GetString());

      CameraErrorDlg dlg(cszText, ex);
      dlg.DoModal(m_hWnd);
   }
}

LRESULT ImagePropertyCombobox::OnSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
   bHandled = false;

   ATLASSERT(m_spRemoteReleaseControl != nullptr);

   // value has changed; update value
   int iItem = CComboBox::GetCurSel();
   if (iItem == CB_ERR)
      return 0;

   size_t iIndex = GetItemData(iItem);

   if (iIndex >= m_vecValues.size())
      return 0; // invalid index

   try
   {
      m_spRemoteReleaseControl->SetImageProperty(m_vecValues[iIndex]);
   }
   catch (const CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Couldn't set image property"), ex);
      dlg.DoModal(m_hWnd);
   }

   return 0;
}
