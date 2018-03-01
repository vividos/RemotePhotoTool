//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SatelliteInfoCtrl.cpp Satellite info control showing satellite detail infos
//
#include "stdafx.h"
#include "SatelliteInfoCtrl.hpp"

void SatelliteInfoCtrl::DrawList(CDC& dc, CRect& rc)
{
   dc.FillSolidRect(rc, RGB(192, 192, 192));

   size_t numSatellites = m_satelliteInfos.size();
   if (numSatellites == 0)
      return;

   int columnWidth = rc.Width() / numSatellites;

   std::vector<GPS::SatelliteInfo>::const_iterator iter = m_satelliteInfos.begin(), stop = m_satelliteInfos.end();

   // find out max. SNR
   unsigned int maxSNR = 0;
   for (; iter != stop; iter++)
      maxSNR = std::max(maxSNR, iter->SignalNoiseRatio());

   // when no SNR is high enough, set max to 50
   if (maxSNR < 50)
      maxSNR = 50;

   int bkMode = dc.SetBkMode(TRANSPARENT);
   COLORREF oldColor = dc.SetTextColor(RGB(255, 255, 255));

   // draw each column, according to the infos in the set
   int columnStart = 0;
   iter = m_satelliteInfos.begin(); // restart
   for (; iter != stop; iter++)
   {
      const GPS::SatelliteInfo& info = *iter;

      // draw a frame
      CRect rectFrame(columnStart, rc.top + 2, columnStart + columnWidth - 1 - 2, rc.bottom - 17);
      dc.DrawEdge(rectFrame, EDGE_ETCHED, BF_ADJUST | BF_FLAT | BF_RECT);

      // note: BF_ADJUST adjusted our rectFrame

      // calculate percent height
      double dHeight = double(info.SignalNoiseRatio()) / maxSNR;

      CRect rectBar(rectFrame);
      rectBar.top = rectBar.bottom - int(dHeight * rectBar.Height());

      // fill bar it with color
      COLORREF cr = info.UsedForFix() ? RGB(0, 0, 255) : RGB(96, 96, 96);
      dc.FillSolidRect(rectBar, cr);

      // draw SNR value
      {
         CString textSNR;
         textSNR.Format(_T("%u"), info.SignalNoiseRatio());

         CRect rectSNR(rectBar);
         // if enough space, draw it under the top line of the bar, else adjust rect
         if (rectSNR.Height() < 16)
         {
            dc.SetTextColor(RGB(0, 0, 0));
            rectSNR.bottom = rectSNR.top - 1;
            rectSNR.top = rectSNR.bottom - 16;
         }
         else
            dc.SetTextColor(RGB(255, 255, 255));

         dc.DrawText(textSNR, textSNR.GetLength(), rectSNR, DT_CENTER | DT_NOPREFIX | DT_SINGLELINE);
      }

      // draw sat id
      CString satID;
      satID.Format(_T("%u"), info.SatID());

      CRect rcSatId(columnStart, rc.bottom - 16, columnStart + columnWidth - 1, rc.bottom);

      dc.SetTextColor(RGB(0, 0, 0));
      dc.DrawText(satID, satID.GetLength(), rcSatId, DT_CENTER | DT_NOPREFIX | DT_SINGLELINE);

      // advance to next frame
      columnStart += columnWidth;
   }

   dc.SetTextColor(oldColor);
   dc.SetBkMode(bkMode);
}
