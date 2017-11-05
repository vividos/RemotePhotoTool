//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file HDRPanoramaPhotoModeView.cpp View for taking HDR Panorama photos
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "HDRPanoramaPhotoModeView.hpp"
#include "IPhotoModeViewHost.hpp"

HDRPanoramaPhotoModeView::HDRPanoramaPhotoModeView(IPhotoModeViewHost& host)
:m_host(host),
 m_manager(host, m_hWnd),
 m_cbShutterSpeed(propTv),
 m_iPropertyHandlerId(-1)
{
}

LRESULT HDRPanoramaPhotoModeView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   m_spRemoteReleaseControl = m_host.GetRemoteReleaseControl();

   if (!m_manager.Init(m_spRemoteReleaseControl))
      return FALSE;

   if (!m_manager.CheckManualMode())
      return 0;

   // init shutter speed combobox
   {
      m_cbShutterSpeed.SetRemoteReleaseControl(m_spRemoteReleaseControl);

      m_cbShutterSpeed.UpdateValuesList();
      m_cbShutterSpeed.UpdateValue();
   }

   // init combobox with number of bracketed shots
   {
      for (size_t i=3; i<13; i+= 2)
      {
         CString cszText;
         cszText.Format(_T("%Iu shots"), i);
         int iItem = m_cbAEBBracketedShots.AddString(cszText);
         m_cbAEBBracketedShots.SetItemData(iItem, i);
      }

      m_cbAEBBracketedShots.SetCurSel(0);

      UpdateAEBShutterSpeedList();
   }

   // init list with shutter speed values
   m_lcAEBShutterSpeedValues.InsertColumn(0, _T("Shutter speed"), LVCFMT_LEFT, 80);

   // add handler for changing shutter speed
   m_iPropertyHandlerId = m_spRemoteReleaseControl->AddPropertyEventHandler(
      std::bind(&HDRPanoramaPhotoModeView::OnUpdatedProperty, this, std::placeholders::_1, std::placeholders::_2));

   return TRUE;
}


LRESULT HDRPanoramaPhotoModeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_spRemoteReleaseControl->RemovePropertyEventHandler(m_iPropertyHandlerId);

   return 0;
}

LRESULT HDRPanoramaPhotoModeView::OnComboShutterSpeedSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   UpdateAEBShutterSpeedList();

   return 0;
}

LRESULT HDRPanoramaPhotoModeView::OnComboAEBBracketShotsSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   UpdateAEBShutterSpeedList();

   return 0;
}

void HDRPanoramaPhotoModeView::OnUpdatedProperty(RemoteReleaseControl::T_enPropertyEvent enPropertyEvent, unsigned int uiValue)
{
   if (m_manager.IsStarted())
      return;

   unsigned int uiShutterSpeedPropertyId =
      m_spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propTv);

   if (uiShutterSpeedPropertyId == uiValue)
   {
      if (enPropertyEvent == RemoteReleaseControl::propEventPropertyDescChanged)
         m_cbShutterSpeed.UpdateValuesList();
      else
         m_cbShutterSpeed.UpdateValue();
   }

   if (enPropertyEvent == RemoteReleaseControl::propEventPropertyChanged &&
       uiShutterSpeedPropertyId == uiValue)
   {
      UpdateAEBShutterSpeedList();
   }
}


LRESULT HDRPanoramaPhotoModeView::OnButtonAEB(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_manager.ShutterSpeedValues().empty())
      return 0;

   if (!m_manager.CheckManualMode())
      return 0;

   m_manager.ReleaseAEBFirst();

   return 0;
}

LRESULT HDRPanoramaPhotoModeView::OnMessageHDRAEBNext(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_manager.ReleaseAEBNext();
   return 0;
}

LRESULT HDRPanoramaPhotoModeView::OnMessageHDRAEBLast(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_host.LockActionMode(false);

   m_manager.OnReleasedAEBLast();

   return 0;
}

void HDRPanoramaPhotoModeView::UpdateAEBShutterSpeedList()
{
   int iItem = m_cbAEBBracketedShots.GetCurSel();
   if (iItem == CB_ERR)
      return;

   size_t uiNumShots = m_cbAEBBracketedShots.GetItemData(iItem);

   m_manager.RecalcAEBShutterSpeedList(uiNumShots);

   m_lcAEBShutterSpeedValues.SetRedraw(FALSE);
   m_lcAEBShutterSpeedValues.DeleteAllItems();

   const std::vector<ImageProperty>& vecAEBShutterSpeedValues = m_manager.ShutterSpeedValues();

   for (size_t i = 0, iMax = vecAEBShutterSpeedValues.size(); i<iMax; i++)
   {
      m_lcAEBShutterSpeedValues.InsertItem(m_lcAEBShutterSpeedValues.GetItemCount(),
         vecAEBShutterSpeedValues[i].AsString());
   }

   m_lcAEBShutterSpeedValues.SetRedraw(TRUE);
}
