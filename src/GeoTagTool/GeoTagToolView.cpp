//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file GeoTagToolView.cpp View for GeoTagTool
//
#include "stdafx.h"
#include "res/Ribbon.h"
#include "resource.h"
#include "GeoTagToolView.hpp"
#include "GPS/Receiver.hpp"

BOOL GeoTagToolView::PreTranslateMessage(MSG* pMsg)
{
   return CWindow::IsDialogMessage(pMsg);
}

void GeoTagToolView::OnStartingGPSReceiver(GPS::Receiver& gpsReceiver)
{
   gpsReceiver.PositionUpdate().Add(
      std::bind(&GeoTagToolView::OnUpdatePositionInfo, this, std::placeholders::_1));

   gpsReceiver.SatelliteInfoUpdate().Add(
      std::bind(&GeoTagToolView::OnUpdateSatelliteInfo, this, std::placeholders::_1));
}

LRESULT GeoTagToolView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   return 0;
}

void GeoTagToolView::OnUpdatePositionInfo(const GPS::PositionInfo& positionInfo)
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

void GeoTagToolView::OnUpdateSatelliteInfo(const std::vector<GPS::SatelliteInfo>& satelliteInfos)
{
   m_satelliteInfoCtrl.UpdateSatelliteInfos(satelliteInfos);
   m_satelliteInfoCtrl.Invalidate();

   m_satelliteRadarCtrl.UpdateSatelliteInfos(satelliteInfos);
   m_satelliteRadarCtrl.Invalidate();
}
