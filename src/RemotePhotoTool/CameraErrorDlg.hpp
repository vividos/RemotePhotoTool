//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CameraErrorDlg.hpp Camera error dialog
//
#pragma once

// includes
#include "CameraException.hpp"
#include "resource.h"

/// camera error dialog
class CameraErrorDlg :
   public CDialogImpl<CameraErrorDlg>,
   public CWinDataExchange<CameraErrorDlg>
{
public:
   /// ctor
   CameraErrorDlg(const CString& cszMessage, const CameraException& ex)
      :m_cszMessage(cszMessage),
       m_cszFunction(ex.Function()),
       m_uiErrorCode(ex.ErrorCode())
   {
   }

   /// dialog id
   enum { IDD = IDD_CAMERA_ERROR };

private:
   BEGIN_DDX_MAP(CameraErrorDlg)
      DDX_CONTROL_HANDLE(IDC_EDIT_ERROR_MESSAGE, m_ecErrorMessage)
      DDX_CONTROL_HANDLE(IDC_EDIT_ERROR_DETAILS, m_ecErrorDetails)
   END_DDX_MAP()

   BEGIN_MSG_MAP(CameraErrorDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
      COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
   END_MSG_MAP()

   /// called on start of dialog
   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      DoDataExchange(DDX_LOAD);

      CenterWindow(GetParent());

      m_ecErrorMessage.SetWindowText(m_cszMessage);

      CString cszDetails;
      cszDetails.Format(_T("Function [%s]\r\nError Code [%08x]"),
         m_cszFunction.GetString(),
         m_uiErrorCode);

      m_ecErrorDetails.SetWindowText(cszDetails);

      return TRUE;
   }

   /// called on closing dialog
   LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      EndDialog(wID);
      return 0;
   }

private:
   /// error message edit control
   CEdit m_ecErrorMessage;

   /// error details edit control
   CEdit m_ecErrorDetails;

   /// error message
   CString m_cszMessage;

   /// error function
   CString m_cszFunction;

   /// error code
   unsigned int m_uiErrorCode;
};
