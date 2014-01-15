//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file DevicePropertyView.cpp View for device properties
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "DevicePropertyView.hpp"
#include "SourceDevice.hpp"
#include "DeviceProperty.hpp"
#include "IPhotoModeViewHost.hpp"
#include "ViewFinderImageWindow.hpp"

void DevicePropertyView::Init()
{
   InsertColumn(columnName, _T("Name"), LVCFMT_LEFT, 300);
   InsertColumn(columnValue, _T("Value"), LVCFMT_LEFT, 300);
   InsertColumn(columnType, _T("Type"), LVCFMT_LEFT, 100);
   InsertColumn(columnReadOnly, _T("Read-only"), LVCFMT_LEFT, 80);
   InsertColumn(columnId, _T("Id"), LVCFMT_LEFT, 80);

   DWORD dwExStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
   SetExtendedListViewStyle(dwExStyle, dwExStyle);

   RefreshList();
}

void DevicePropertyView::RefreshList()
{
   ATLASSERT(m_spSourceDevice != nullptr);
   SourceDevice& sd = *m_spSourceDevice;

   SetRedraw(FALSE);

   DeleteAllItems();

   // disable viewfinder while refreshing list
   ViewFinderImageWindow* pViewfinder = m_host.GetViewfinderWindow();

   if (pViewfinder != NULL)
      pViewfinder->EnableUpdate(false, true); // wait for viewfinder image handler to exit

   std::vector<unsigned int> vecDevicePropertyIds = sd.EnumDeviceProperties();

   for (size_t i=0, iMax = vecDevicePropertyIds.size(); i<iMax; i++)
   {
      unsigned int uiPropertyId = vecDevicePropertyIds[i];
      DeviceProperty dp = sd.GetDeviceProperty(uiPropertyId);

      int iIndex = InsertItem(GetItemCount(), dp.Name());

      SetItemText(iIndex, columnValue, dp.AsString());
      SetItemText(iIndex, columnType, Variant::TypeAsString(dp.Value().Type()));
      SetItemText(iIndex, columnReadOnly, dp.IsReadOnly() ? _T("yes") : _T("no"));

      CString cszId;
      cszId.Format(_T("0x%04x"), uiPropertyId);
      SetItemText(iIndex, columnId, cszId);
   }

   if (pViewfinder != NULL)
      pViewfinder->EnableUpdate(true);

   SetRedraw(TRUE);
}
