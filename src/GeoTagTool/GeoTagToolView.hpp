//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file GeoTagTool\GeoTagToolView.hpp View for GeoTagTool
//
#pragma once

#include "GPS/SatelliteInfo.hpp"
#include "GPS/PositionInfo.hpp"
#include "SatelliteInfoCtrl.hpp"
#include "SatelliteRadarCtrl.hpp"

/// form view for GeoTagTool
class GeoTagToolView :
   public CDialogImpl<GeoTagToolView>,
   public CWinDataExchange<GeoTagToolView>
{
public:
   /// form ID
   enum { IDD = IDD_GEOTAGTOOL_FORM };

   /// called to pre-translate message
   BOOL PreTranslateMessage(MSG* pMsg);

private:
   BEGIN_DDX_MAP(GeoTagToolView)
      DDX_CONTROL(IDC_STATIC_SATINFO_GRAPH, m_satelliteInfoCtrl)
      DDX_CONTROL(IDC_STATIC_SATINFO_RADAR, m_satelliteRadarCtrl)
      DDX_CONTROL_HANDLE(IDC_STATIC_POSITION_INFO, m_staticPositionInfo)
   END_DDX_MAP()

   BEGIN_MSG_MAP(GeoTagToolView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   // LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   /// called when dialog is initialized
   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

   /// called when position infos are updated
   void OnUpdatePositionInfo(const GPS::PositionInfo& positionInfo);

   /// called when satellite infos are updated
   void OnUpdateSatelliteInfo(const std::vector<GPS::SatelliteInfo>& satelliteInfos);

private:
   /// satellite info control
   SatelliteInfoCtrl m_satelliteInfoCtrl;

   /// satellite radar control
   SatelliteRadarCtrl m_satelliteRadarCtrl;

   /// current position info
   CStatic m_staticPositionInfo;
};
