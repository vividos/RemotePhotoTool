//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file ImagePropertyView.cpp View for image properties
//
#include "stdafx.h"
#include "resource.h"
#include "ImagePropertyView.hpp"
#include "IPhotoModeViewHost.hpp"
#include "ImageProperty.hpp"
#include "ViewFinderView.hpp"
#include "CameraException.hpp"

bool ImagePropertyView::Init()
{
   m_remoteReleaseControl = m_host.GetRemoteReleaseControl();

   m_propertyEventId = m_remoteReleaseControl->AddPropertyEventHandler(
      std::bind(&ImagePropertyView::OnPropertyChanged, this, std::placeholders::_1, std::placeholders::_2));

   InsertColumn(columnName, _T("Name"), LVCFMT_LEFT, 300);
   InsertColumn(columnValue, _T("Value"), LVCFMT_LEFT, 300);
   InsertColumn(columnType, _T("Type"), LVCFMT_LEFT, 100);
   InsertColumn(columnReadOnly, _T("Read-only"), LVCFMT_LEFT, 80);
   InsertColumn(columnId, _T("Id"), LVCFMT_LEFT, 80);
   InsertColumn(columnRaw, _T("Raw"), LVCFMT_LEFT, 80);

   DWORD exStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
   SetExtendedListViewStyle(exStyle, exStyle);

   RefreshList();

   return true;
}

void ImagePropertyView::DestroyView()
{
   if (m_propertyEventId != -1 && m_remoteReleaseControl != nullptr)
      m_remoteReleaseControl->RemovePropertyEventHandler(m_propertyEventId);

   ATLVERIFY(TRUE == DestroyWindow());
}

void ImagePropertyView::OnPropertyChanged(RemoteReleaseControl::T_enPropertyEvent /*propertyEvent*/, unsigned int propertyId)
{
   if (propertyId == 0)
      PostMessage(WM_RELEASECONTROL_ALL_PROPERTIES_CHANGED);
   else
      PostMessage(WM_RELEASECONTROL_PROPERTY_CHANGED, propertyId);
}

void ImagePropertyView::UpdateProperty(unsigned int imagePropertyId)
{
   ATLASSERT(m_remoteReleaseControl != nullptr);
   RemoteReleaseControl& rrc = *m_remoteReleaseControl;

   SetRedraw(FALSE);

   // find property by item data
   for (int index = 0, maxIndex = GetItemCount(); index < maxIndex; index++)
   {
      unsigned int propertyId = GetItemData(index);
      if (propertyId != imagePropertyId)
         continue;

      try
      {
         ImageProperty ip = rrc.GetImageProperty(propertyId);

         SetItemText(index, columnValue, ip.AsString());
         SetItemText(index, columnReadOnly, ip.IsReadOnly() ? _T("yes") : _T("no"));
         SetItemText(index, columnRaw, ip.Value().ToString());
      }
      catch (const CameraException& ex)
      {
         SetItemText(index, columnValue, ex.Message());
      }
   }

   SetRedraw(TRUE);
}

void ImagePropertyView::RefreshList()
{
   ATLASSERT(m_remoteReleaseControl != nullptr);
   RemoteReleaseControl& rrc = *m_remoteReleaseControl;

   SetRedraw(FALSE);

   DeleteAllItems();

   // disable viewfinder while refreshing list
   ViewFinderView* viewfinder = m_host.GetViewFinderView();

   if (viewfinder != nullptr)
      viewfinder->EnableUpdate(false);

   std::vector<unsigned int> imagePropertyIdList;
   imagePropertyIdList = rrc.EnumImageProperties();

   for (size_t index = 0, maxIndex = imagePropertyIdList.size(); index < maxIndex; index++)
   {
      unsigned int propertyId = imagePropertyIdList[index];

      try
      {
         ImageProperty ip = rrc.GetImageProperty(propertyId);

         int itemIndex = InsertItem(GetItemCount(), ip.Name());
         SetItemData(index, propertyId);

         SetItemText(itemIndex, columnValue, ip.AsString());
         SetItemText(itemIndex, columnType, Variant::TypeAsString(ip.Value().Type()));
         SetItemText(itemIndex, columnReadOnly, ip.IsReadOnly() ? _T("yes") : _T("no"));

         CString textId;
         textId.Format(_T("0x%08x"), propertyId);
         SetItemText(itemIndex, columnId, textId);

         SetItemText(itemIndex, columnRaw, ip.Value().ToString());
      }
      catch (const CameraException& ex)
      {
         int itemIndex = InsertItem(GetItemCount(), _T("???"));

         SetItemText(itemIndex, columnValue, ex.Message());
      }
   }

   if (viewfinder != nullptr)
      viewfinder->EnableUpdate(true);

   SetRedraw(TRUE);
}
