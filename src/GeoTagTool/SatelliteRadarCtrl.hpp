//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SatelliteRadarCtrl.hpp Satellite radar control showing satellite positions on the sky
//
#pragma once

#include "GPS/SatelliteInfo.hpp"
#include <vector>
#include <set>

/// \brief satellite radar control
/// \details shows a radar style control, displaying the GPS satellites'
/// position based on the azimuth and elevation values reported, and shows if
/// a satellite is used for the GPS fix. When enabled, the previous positions
/// of the satellites are drawn, displaying "sat trails" where one can see the
/// satellites' path over the sky.
class SatelliteRadarCtrl :
   public CWindowImpl<SatelliteRadarCtrl>
{
public:
   /// ctor
   SatelliteRadarCtrl()
      :m_enableSatTrails(true)
   {
   }

   /// enables satellite trails
   void EnableSatTrails(bool enableSatTrails = true)
   {
      m_enableSatTrails = enableSatTrails;
   }

   /// called to reset satellite trails points; the recording of trails is
   /// still active
   void ResetSatTrails()
   {
      m_setSatTrails.clear();
   }

   /// called to update satellite infos
   void UpdateSatelliteInfos(const std::vector<GPS::SatelliteInfo>& satelliteInfos)
   {
      m_satelliteInfos = satelliteInfos;
      CollectSatTrails();
   }

private:
   BEGIN_MSG_MAP(SatelliteRadarCtrl)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
   END_MSG_MAP()

   /// called when control has to be repainted
   LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      if (m_font.IsNull())
         m_font = CWindow(GetParent()).GetFont();

      CPaintDC dc(m_hWnd);

      CRect rc;
      GetClientRect(rc);

      CFontHandle oldFont = dc.SelectFont(m_font);
      DrawRadar(dc, rc);
      dc.SelectFont(oldFont);

      return 0;
   }

   /// draws radar
   void DrawRadar(CDC& dc, CRect& rc);

   /// collects satellite trails' points
   void CollectSatTrails();

private:
   /// control font to use
   CFontHandle m_font;

   /// satellite infos
   std::vector<GPS::SatelliteInfo> m_satelliteInfos;

   /// indicates if control should draw sat trails
   bool m_enableSatTrails;

   /// sat trails; pair of elevation/azimuth positions to draw
   std::set<std::pair<unsigned int, unsigned int>> m_setSatTrails;
};
