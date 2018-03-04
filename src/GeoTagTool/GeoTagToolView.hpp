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

namespace GPS
{
   class Receiver;
}

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

   /// called when GPS receiver is about to be started
   void OnStartingGPSReceiver(GPS::Receiver& gpsReceiver);

private:
   BEGIN_DDX_MAP(GeoTagToolView)
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

private:
};
