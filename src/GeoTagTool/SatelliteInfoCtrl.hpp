//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SatelliteInfoCtrl.hpp Satellite info control showing satellite detail infos
//
#pragma once

#include "GPS/SatelliteInfo.hpp"
#include <vector>

/// \brief satellite info control
/// \details shows a chart of bars, displaying the signals visible and in use
/// by the GPS receiver, including their signal-to-noise ratio (SNR) as bar
/// height.
class SatelliteInfoCtrl :
   public CWindowImpl<SatelliteInfoCtrl>
{
public:
   /// ctor
   SatelliteInfoCtrl()
   {
   }

   /// called to update satellite infos
   void UpdateSatelliteInfos(const std::vector<GPS::SatelliteInfo>& satelliteInfos)
   {
      m_satelliteInfos = satelliteInfos;
   }

private:
   BEGIN_MSG_MAP(SatelliteInfoCtrl)
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
      DrawList(dc, rc);
      dc.SelectFont(oldFont);

      return 0;
   }

   /// draws list of satellite infos
   void DrawList(CDC& dc, CRect& rc);

private:
   /// control font to use
   CFontHandle m_font;

   /// satellite infos to show
   std::vector<GPS::SatelliteInfo> m_satelliteInfos;
};
