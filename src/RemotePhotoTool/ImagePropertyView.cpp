//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ImagePropertyView.cpp View for image properties
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "ImagePropertyView.hpp"
#include "IPhotoModeViewHost.hpp"
#include "ImageProperty.hpp"
#include "ViewFinderView.hpp"

bool ImagePropertyView::Init()
{
   m_spRemoteReleaseControl = m_host.StartRemoteReleaseControl(true);
   if (m_spRemoteReleaseControl == nullptr)
   {
      AtlMessageBox(m_hWnd, _T("Couldn't start remote release control."), IDR_MAINFRAME, MB_OK);
      DestroyWindow();
      return false;
   }

   m_iPropertyEventId = m_spRemoteReleaseControl->AddPropertyEventHandler(
      std::bind(&ImagePropertyView::OnPropertyChanged, this, std::placeholders::_1, std::placeholders::_2));

   InsertColumn(columnName, _T("Name"), LVCFMT_LEFT, 300);
   InsertColumn(columnValue, _T("Value"), LVCFMT_LEFT, 300);
   InsertColumn(columnType, _T("Type"), LVCFMT_LEFT, 100);
   InsertColumn(columnReadOnly, _T("Read-only"), LVCFMT_LEFT, 80);
   InsertColumn(columnId, _T("Id"), LVCFMT_LEFT, 80);
   InsertColumn(columnRaw, _T("Raw"), LVCFMT_LEFT, 80);

   DWORD dwExStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
   SetExtendedListViewStyle(dwExStyle, dwExStyle);

   RefreshList();

   return true;
}

void ImagePropertyView::DestroyView()
{
   if (m_iPropertyEventId != -1 && m_spRemoteReleaseControl != nullptr)
      m_spRemoteReleaseControl->RemovePropertyEventHandler(m_iPropertyEventId);

   ATLVERIFY(TRUE == DestroyWindow());
}

void ImagePropertyView::OnPropertyChanged(RemoteReleaseControl::T_enPropertyEvent /*enPropertyEvent*/, unsigned int uiPropertyId)
{
   if (uiPropertyId == 0)
      PostMessage(WM_RELEASECONTROL_ALL_PROPERTIES_CHANGED);
   else
      PostMessage(WM_RELEASECONTROL_PROPERTY_CHANGED, uiPropertyId);
}

void ImagePropertyView::UpdateProperty(unsigned int uiImagePropertyId)
{
   ATLASSERT(m_spRemoteReleaseControl != nullptr);
   RemoteReleaseControl& rrc = *m_spRemoteReleaseControl;

   SetRedraw(FALSE);

   // find property by item data
   for (int iIndex=0, iMax = GetItemCount(); iIndex<iMax; iIndex++)
   {
      unsigned int uiPropertyId = GetItemData(iIndex);
      if (uiPropertyId != uiImagePropertyId)
         continue;

      ImageProperty ip = rrc.GetImageProperty(uiPropertyId);

      SetItemText(iIndex, columnValue, ip.AsString());
      SetItemText(iIndex, columnReadOnly, ip.IsReadOnly() ? _T("yes") : _T("no"));
      SetItemText(iIndex, columnRaw, ip.Value().ToString());
   }

   SetRedraw(TRUE);
}

void ImagePropertyView::RefreshList()
{
   ATLASSERT(m_spRemoteReleaseControl != nullptr);
   RemoteReleaseControl& rrc = *m_spRemoteReleaseControl;

   SetRedraw(FALSE);

   DeleteAllItems();

   // disable viewfinder while refreshing list
   ViewFinderView* pViewfinder = m_host.GetViewFinderView();

   if (pViewfinder != NULL)
      pViewfinder->EnableUpdate(false);

   std::vector<unsigned int> vecImagePropertyIds;
   vecImagePropertyIds = rrc.EnumImageProperties();

   for (size_t i=0, iMax = vecImagePropertyIds.size(); i<iMax; i++)
   {
      unsigned int uiPropertyId = vecImagePropertyIds[i];
      ImageProperty ip = rrc.GetImageProperty(uiPropertyId);

      int iIndex = InsertItem(GetItemCount(), ip.Name());
      SetItemData(iIndex, uiPropertyId);

      SetItemText(iIndex, columnValue, ip.AsString());
      SetItemText(iIndex, columnType, Variant::TypeAsString(ip.Value().Type()));
      SetItemText(iIndex, columnReadOnly, ip.IsReadOnly() ? _T("yes") : _T("no"));

      CString cszId;
      cszId.Format(_T("0x%04x"), uiPropertyId);
      SetItemText(iIndex, columnId, cszId);

      SetItemText(iIndex, columnRaw, ip.Value().ToString());
   }

   if (pViewfinder != NULL)
      pViewfinder->EnableUpdate(true);

   SetRedraw(TRUE);
}
