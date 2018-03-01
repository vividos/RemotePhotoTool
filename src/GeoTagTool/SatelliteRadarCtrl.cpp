//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SatelliteRadarCtrl.cpp Satellite radar control showing satellite positions on the sky
//
#include "stdafx.h"
#include "SatelliteRadarCtrl.hpp"

const double g_dPi = 3.14159265;

void SatelliteRadarCtrl::DrawRadar(CDC& dc, CRect& rc)
{
   dc.FillSolidRect(rc, RGB(192, 192, 192));

   int outerRadius = std::min(rc.Width(), rc.Height()) / 2 - 5;
   CPoint centerPoint(rc.Width() / 2, rc.Height() / 2);

   CBrush blackBrush;
   blackBrush.CreateSolidBrush(RGB(0, 0, 0));
   HBRUSH oldBrush = dc.SelectBrush(blackBrush);

   CPen bluePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
   HPEN oldPen = dc.SelectPen(bluePen);

   dc.SetBkColor(RGB(0, 0, 0)); // black
   dc.SetTextColor(RGB(255, 255, 255)); // white

   {
      // draw circles from 0 to 75 degrees
      for (int circle = 0; circle < 90; circle += 15)
      {
         // draw circle
         int radius = int((90 - circle) / 90.0 * outerRadius);
         {
            CRect rcCircle(
               centerPoint.x - radius, centerPoint.y - radius,
               centerPoint.x + radius, centerPoint.y + radius);

            dc.Ellipse(rcCircle);
         }

         // draw text
         if (circle > 0)
         {
            CString textElevation;
            textElevation.Format(_T("%u°"), circle);

            CPoint pointTextCenter(centerPoint.x + int(radius * 0.707106781),
               centerPoint.y - int(radius * 0.707106781));

            CRect rectText(
               pointTextCenter.x - 20, pointTextCenter.y - 8,
               pointTextCenter.x + 20, pointTextCenter.y + 8);

            dc.DrawText(textElevation, textElevation.GetLength(), rectText, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
         }
      }
   }

   // draw "spikes"
   {
      for (int i = 0; i < 360; i += 30)
      {
         // draw spike
         dc.MoveTo(
            centerPoint.x + int(outerRadius * cos((i / 180.0) * g_dPi)),
            centerPoint.y + int(outerRadius * sin((i / 180.0) * g_dPi)));

         dc.LineTo(centerPoint.x, centerPoint.y);

         // draw text
         {
            int radius = int((90 - 15) / 90.0 * outerRadius);

            CString textAzimuth;
            if (i == 0)
               textAzimuth = _T("E");
            else
               if (i == 90)
                  textAzimuth = _T("S");
               else
                  if (i == 180)
                     textAzimuth = _T("W");
                  else
                     if (i == 270)
                        textAzimuth = _T("N");
                     else
                        textAzimuth.Format(_T("%u°"), (i + 90) % 360);

            CPoint pointTextCenter(
               centerPoint.x + int(radius * cos(i / 180.0 * g_dPi)),
               centerPoint.y + int(radius * sin(i / 180.0 * g_dPi)));

            CRect rectText(
               pointTextCenter.x - 20, pointTextCenter.y - 10,
               pointTextCenter.x + 20, pointTextCenter.y + 10);

            dc.DrawText(textAzimuth, textAzimuth.GetLength(), rectText, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
         }
      }
   }

   // draw sat trails; do this before drawing satellites so we don't overdraw them
   if (m_enableSatTrails)
   {
      std::set<std::pair<unsigned int, unsigned int> >::const_iterator iter = m_setSatTrails.begin(), stop = m_setSatTrails.end();
      for (; iter != stop; iter++)
      {
         int radius = int((90 - iter->first) / 90.0 * outerRadius);
         int iAngle = (iter->second + 270) % 360;

         CPoint ptTrailCenter(
            centerPoint.x + int(radius * cos(iAngle / 180.0 * g_dPi)),
            centerPoint.y + int(radius * sin(iAngle / 180.0 * g_dPi)));

         // green pixel
         dc.SetPixel(ptTrailCenter, RGB(0, 255, 0));
      }
   }

   CPen grayPen = CreatePen(PS_SOLID, 1, RGB(96, 96, 96));
   CPen yellowPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));

   CBrush lightGrayBrush;
   lightGrayBrush.CreateSolidBrush(RGB(160, 160, 160));

   CBrush redBrush;
   redBrush.CreateSolidBrush(RGB(255, 0, 0));

   // draw satellites
   std::vector<GPS::SatelliteInfo>::const_iterator iter = m_satelliteInfos.begin(), stop = m_satelliteInfos.end();
   for (; iter != stop; iter++)
   {
      const GPS::SatelliteInfo& info = *iter;
      int radius = int((90 - info.Elevation()) / 90.0 * outerRadius);
      int iAngle = (info.Azimuth() + 270) % 360;

      CPoint pointSatCenter(
         centerPoint.x + int(radius * cos(iAngle / 180.0 * g_dPi)),
         centerPoint.y + int(radius * sin(iAngle / 180.0 * g_dPi)));

      // draw satellite
      {
         CRect rectSat(pointSatCenter.x - 5, pointSatCenter.y - 5,
            pointSatCenter.x + 5, pointSatCenter.y + 5);

         dc.SelectPen(info.UsedForFix() ? yellowPen : grayPen);
         dc.SelectBrush(info.UsedForFix() ? redBrush : lightGrayBrush);

         // draw sat circle
         CRect rectSatCircle(
            pointSatCenter.x - 3, pointSatCenter.y - 3,
            pointSatCenter.x + 3, pointSatCenter.y + 3);

         dc.Ellipse(rectSatCircle);

         // draw cross / dot
/*
         dc.MoveTo(pointSatCenter.x-1, pointSatCenter.y);
         dc.LineTo(pointSatCenter.x+1, pointSatCenter.y);
         dc.MoveTo(pointSatCenter.x, pointSatCenter.y-1);
         dc.LineTo(pointSatCenter.x, pointSatCenter.y+1);
*/
         dc.SetPixel(pointSatCenter, info.UsedForFix() ? RGB(255, 0, 0) : RGB(160, 160, 160));

         // draw left part
         CRect rectLeftSatPart(
            pointSatCenter.x - 7, pointSatCenter.y - 5,
            pointSatCenter.x - 3, pointSatCenter.y + 5);

         dc.Rectangle(rectLeftSatPart);

         // draw right part
         CRect rectRightSatPart(
            pointSatCenter.x + 3, pointSatCenter.y - 5,
            pointSatCenter.x + 7, pointSatCenter.y + 5);

         dc.Rectangle(rectRightSatPart);
      }

      // draw sat id
      {
         // yellow for active, dark gray for passive
         dc.SetTextColor(info.UsedForFix() ? RGB(255, 255, 0) : RGB(96, 96, 96));

         // draw sat id
         CString textSatID;
         textSatID.Format(_T("%u"), info.SatID());

         CRect rectSatId(
            pointSatCenter.x - 10, pointSatCenter.y + 5,
            pointSatCenter.x + 10, pointSatCenter.y + 5 + 16);

         dc.DrawText(textSatID, textSatID.GetLength(), rectSatId, DT_CENTER | DT_NOPREFIX | DT_SINGLELINE);
      }
   }

   dc.SelectPen(oldPen);
   dc.SelectBrush(oldBrush);
}

void SatelliteRadarCtrl::CollectSatTrails()
{
   std::vector<GPS::SatelliteInfo>::const_iterator iter = m_satelliteInfos.begin(), stop = m_satelliteInfos.end();
   for (; iter != stop; iter++)
   {
      const GPS::SatelliteInfo& info = *iter;

      m_setSatTrails.insert(std::make_pair(info.Elevation(), info.Azimuth()));
   }
}
