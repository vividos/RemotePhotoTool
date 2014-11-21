//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ViewFinderView.cpp View for viewfinder image
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "ViewFinderView.hpp"
#include "RemoteReleaseControl.hpp"
#include "CameraException.hpp"
#include "CameraErrorDlg.hpp"

ViewFinderView::ViewFinderView(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
:m_spRemoteReleaseControl(spRemoteReleaseControl),
 m_bShowHistogram(false)
{
}

void ViewFinderView::SetViewfinder(std::shared_ptr<Viewfinder> spViewfinder)
{
   ATLASSERT(m_upViewFinderWindow != nullptr);

   m_upViewFinderWindow->SetViewfinder(spViewfinder);
}

void ViewFinderView::EnableUpdate(bool bEnable)
{
   ATLASSERT(m_upViewFinderWindow != nullptr);

   m_upViewFinderWindow->EnableUpdate(bEnable);
}

LRESULT ViewFinderView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   DlgResize_Init(false, false);

   SetupZoomControls();
   SetupLinesModeCombobox();
   SetupViewfinderWindow();

   return TRUE;
}

LRESULT ViewFinderView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_upViewFinderWindow.reset();

   return 0;
}

LRESULT ViewFinderView::OnComboLinesModeSelEndOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   int iSel = m_cbLinesMode.GetCurSel();
   if (iSel == CB_ERR)
      return 0;

   DWORD_PTR dwData = m_cbLinesMode.GetItemData(iSel);
   ViewFinderImageWindow::T_enLinesMode enLinesMode = static_cast<ViewFinderImageWindow::T_enLinesMode>(dwData);

   if (enLinesMode <= ViewFinderImageWindow::linesModeMax && m_upViewFinderWindow != nullptr)
      m_upViewFinderWindow->SetLinesMode(enLinesMode);

   return 0;
}

LRESULT ViewFinderView::OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   WORD wType = LOWORD(wParam);
   if (wType != SB_ENDSCROLL)
      return 0;

   unsigned int uiPos = m_tbZoom.GetPos();
   SetZoomPos(uiPos);

   return 0;
}

LRESULT ViewFinderView::OnBnClickedAutoFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   try
   {
      EnableUpdate(false);

      m_spRemoteReleaseControl->SendCommand(RemoteReleaseControl::commandAdjustFocus);

      EnableUpdate(true);
   }
   catch(CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Error while adjusting focus"), ex);
      dlg.DoModal(m_hWnd);
   }

   return 0;
}

LRESULT ViewFinderView::OnBnClickedAutoWhiteBalance(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   try
   {
      EnableUpdate(false);

      m_spRemoteReleaseControl->SendCommand(RemoteReleaseControl::commandAdjustWhiteBalance);

      EnableUpdate(true);
   }
   catch(CameraException& ex)
   {
      CameraErrorDlg dlg(_T("Error while adjusting focus"), ex);
      dlg.DoModal(m_hWnd);
   }

   return 0;
}

LRESULT ViewFinderView::OnBnClickedZoomOut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   unsigned int uiPos = static_cast<unsigned int>(m_tbZoom.GetPos());

   if (uiPos > 0)
   {
      uiPos--;

      SetZoomPos(uiPos);
      m_tbZoom.SetPos(uiPos);
   }

   return 0;
}

LRESULT ViewFinderView::OnBnClickedZoomIn(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   unsigned int uiPos = static_cast<unsigned int>(m_tbZoom.GetPos());

   if (uiPos < m_vecAllZoomValues.size()-1)
   {
      uiPos++;

      SetZoomPos(uiPos);
      m_tbZoom.SetPos(uiPos);
   }

   return 0;
}

LRESULT ViewFinderView::OnBnClickedHistogram(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_bShowHistogram = !m_bShowHistogram;

   m_upViewFinderWindow->ShowHistogram(m_bShowHistogram);

   return 0;
}

LRESULT ViewFinderView::OnBnClickedPreviousImageViewer(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   return 0;
}

LRESULT ViewFinderView::OnCheckViewfinderShowOverexposed(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CButton btn(GetDlgItem(IDC_CHECK_VIEWFINDER_SHOW_OVEREXPOSED));
   bool bChecked = btn.GetCheck() == BST_CHECKED;

   m_upViewFinderWindow->ShowZebraPattern(bChecked);

   return 0;
}

void ViewFinderView::SetupZoomControls()
{
   unsigned int uiPropertyId = m_spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propCurrentZoomPos);
   m_spRemoteReleaseControl->EnumImagePropertyValues(uiPropertyId, m_vecAllZoomValues);

   if (!m_spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capZoomControl) || m_vecAllZoomValues.empty())
   {
      GetDlgItem(IDC_BUTTON_ZOOM_OUT).EnableWindow(FALSE);
      m_tbZoom.EnableWindow(FALSE);
      GetDlgItem(IDC_BUTTON_ZOOM_IN).EnableWindow(FALSE);

      return;
   }

   m_tbZoom.SetRange(0, m_vecAllZoomValues.size()-1, FALSE);

   // position
   ImageProperty currentPos = m_spRemoteReleaseControl->GetImageProperty(uiPropertyId);

   auto iter = std::find(m_vecAllZoomValues.begin(), m_vecAllZoomValues.end(), currentPos);
   if (iter != m_vecAllZoomValues.end())
   {
      unsigned int uiCurrentPos = std::distance(m_vecAllZoomValues.begin(), iter);

      m_tbZoom.SetPos(uiCurrentPos);
   }
}

void ViewFinderView::SetupLinesModeCombobox()
{
   int iItem = m_cbLinesMode.AddString(_T("No lines"));     m_cbLinesMode.SetItemData(iItem, ViewFinderImageWindow::linesModeNoLines);
   iItem = m_cbLinesMode.AddString(_T("Rule of thirds"));   m_cbLinesMode.SetItemData(iItem, ViewFinderImageWindow::linesModeRuleOfThird);
   iItem = m_cbLinesMode.AddString(_T("Golden ratio"));     m_cbLinesMode.SetItemData(iItem, ViewFinderImageWindow::linesModeGoldenRatio);

   m_cbLinesMode.SetCurSel(0);
}

void ViewFinderView::SetupViewfinderWindow()
{
   CRect rcViewfinder;
   GetDlgItem(IDC_STATIC_VIEWFINDER_FRAME).GetWindowRect(rcViewfinder);
   ScreenToClient(&rcViewfinder);

   m_upViewFinderWindow.reset(new ViewFinderImageWindow);
   m_upViewFinderWindow->Create(m_hWnd, rcViewfinder);
   m_upViewFinderWindow->SetDlgCtrlID(IDC_STATIC_VIEWFINDER_FRAME);
}

void ViewFinderView::SetZoomPos(unsigned int uiPos)
{
   if (m_vecAllZoomValues.empty())
      return;

   if (uiPos >= m_vecAllZoomValues.size())
      uiPos = m_vecAllZoomValues.size()-1;

   m_spRemoteReleaseControl->SetImageProperty(m_vecAllZoomValues[uiPos]);
}
