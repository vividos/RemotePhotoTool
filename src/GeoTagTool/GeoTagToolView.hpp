//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file GeoTagTool\GeoTagToolView.hpp View for GeoTagTool
//
#pragma once

/// form view for GeoTagTool
class GeoTagToolView : public CDialogImpl<GeoTagToolView>
{
public:
   /// form ID
   enum { IDD = IDD_GEOTAGTOOL_FORM };

   /// called to pre-translate message
   BOOL PreTranslateMessage(MSG* pMsg);

   BEGIN_MSG_MAP(GeoTagToolView)
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   // LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};
