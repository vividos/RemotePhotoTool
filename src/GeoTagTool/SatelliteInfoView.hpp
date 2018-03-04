//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SatelliteInfoView.hpp Satellite info view
//
#pragma once

#include "GPS/SatelliteInfo.hpp"
#include "GPS/PositionInfo.hpp"
#include "SatelliteInfoCtrl.hpp"
#include "SatelliteRadarCtrl.hpp"

namespace GPS
{
   class Receiver;
}

/// \brief view to show satellite infos
class SatelliteInfoView :
   public CDialogImpl<SatelliteInfoView>,
   public CDialogResize<SatelliteInfoView>,
   public CWinDataExchange<SatelliteInfoView>
{
public:
   /// ctor
   SatelliteInfoView();

   /// form ID
   enum { IDD = IDD_SATELLITE_INFO_FORM };

   /// called to pre-translate message
   BOOL PreTranslateMessage(MSG* pMsg);

   /// called when GPS receiver is about to be started
   void OnStartingGPSReceiver(GPS::Receiver& gpsReceiver);

private:
   BEGIN_DDX_MAP(SatelliteInfoView)
      DDX_CONTROL(IDC_STATIC_SATINFO_RADAR, m_satelliteRadarCtrl);
      DDX_CONTROL(IDC_STATIC_SATINFO_GRAPH, m_satelliteInfoCtrl);
      DDX_CONTROL_HANDLE(IDC_STATIC_POSITION_INFO, m_staticPositionInfo)
      DDX_CONTROL_HANDLE(IDC_STATIC_RAW_NMEA0183_DATA, m_staticNMEA0183Data)
   END_DDX_MAP()

   BEGIN_MSG_MAP(SatelliteInfoView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      CHAIN_MSG_MAP(CDialogResize<SatelliteInfoView>)
   END_MSG_MAP()

   BEGIN_DLGRESIZE_MAP(SatelliteInfoView)
      DLGRESIZE_CONTROL(IDC_STATIC_SATINFO_GRAPH, DLSZ_SIZE_X)
      DLGRESIZE_CONTROL(IDC_STATIC_POSITION_INFO, DLSZ_SIZE_Y)
      DLGRESIZE_CONTROL(IDC_STATIC_RAW_NMEA0183_DATA, DLSZ_SIZE_X | DLSZ_SIZE_Y)
   END_DLGRESIZE_MAP()

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
   /// satellite radar control
   SatelliteRadarCtrl m_satelliteRadarCtrl;

   /// satellite info control
   SatelliteInfoCtrl m_satelliteInfoCtrl;

   /// current position info
   CStatic m_staticPositionInfo;

   /// raw NMEA 0183 data
   CStatic m_staticNMEA0183Data;
};
