//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SatelliteInfoView.cpp Satellite info view
//
#include "stdafx.h"
#include "resource.h"
#include "SatelliteInfoView.hpp"
#include "GPS/Receiver.hpp"

BOOL SatelliteInfoView::PreTranslateMessage(MSG* pMsg)
{
   return CWindow::IsDialogMessage(pMsg);
}

void SatelliteInfoView::OnStartingGPSReceiver(GPS::Receiver& gpsReceiver)
{
   gpsReceiver.PositionUpdate().Add(
      std::bind(&SatelliteInfoView::OnUpdatePositionInfo, this, std::placeholders::_1));

   gpsReceiver.SatelliteInfoUpdate().Add(
      std::bind(&SatelliteInfoView::OnUpdateSatelliteInfo, this, std::placeholders::_1));

   gpsReceiver.RawNMEA0183Update().Add(
      std::bind(&SatelliteInfoView::OnUpdateRawNMEA0183Data, this, std::placeholders::_1));
}

LRESULT SatelliteInfoView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   DlgResize_Init(false, false);

   return 0;
}

void SatelliteInfoView::OnUpdatePositionInfo(const GPS::PositionInfo& positionInfo)
{
   CString text, latitude, longitude;

   GPS::WGS84::Coordinate c = positionInfo.Coordinate();
   if (c.IsValid())
      c.ToString(GPS::WGS84::Coordinate::fmtDDD_MM_aph_ss_dot_sss, latitude, longitude);

   text.Format(
      _T("Latitude: %s\r\n")
      _T("Longitude: %s\r\n"),
      c.IsValid() ? latitude.GetString() : _T("invalid"),
      c.IsValid() ? longitude.GetString() : _T("invalid")
   );

   m_staticPositionInfo.SetWindowText(text);
}

void SatelliteInfoView::OnUpdateSatelliteInfo(const std::vector<GPS::SatelliteInfo>& satelliteInfos)
{
   m_satelliteInfoCtrl.UpdateSatelliteInfos(satelliteInfos);
   m_satelliteInfoCtrl.Invalidate();

   m_satelliteRadarCtrl.UpdateSatelliteInfos(satelliteInfos);
   m_satelliteRadarCtrl.Invalidate();
}

void SatelliteInfoView::OnUpdateRawNMEA0183Data(const CString& rawNMEA0183data)
{
   int end = m_editNMEA0183Data.GetWindowTextLength();
   m_editNMEA0183Data.SetSel(end, end);

   m_editNMEA0183Data.ReplaceSel(rawNMEA0183data);
}
