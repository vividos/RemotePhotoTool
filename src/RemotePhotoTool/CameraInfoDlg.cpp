//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CameraInfoDlg.cpp Camera info dialog
//

// includes
#include "StdAfx.h"
#include "resource.h"
#include "CameraInfoDlg.hpp"
#include "Instance.hpp"
#include "CameraException.hpp"
#include "SourceDevice.hpp"
#include "RemoteReleaseControl.hpp"

LRESULT CameraInfoDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   CenterWindow(GetParent());

   DlgResize_Init(false, false);

   CString cszText;
   try
   {
      cszText = CollectCameraInfo();
   }
   catch(CameraException& ex)
   {
      cszText.AppendFormat(_T("\nError while collecting camera info: %s\n"), ex.Message().GetString());
   }

   cszText.Replace(_T("\n"), _T("\r\n"));
   m_ecCameraInfo.SetWindowText(cszText);

   return TRUE;
}

CString CameraInfoDlg::CollectCameraInfo()
{
   CString cszText;

   cszText = _T("Camera model: ") + m_sourceDevice.ModelName() + _T("\n");
   cszText += _T("Serial number: ") + m_sourceDevice.SerialNumber() + _T("\n\n");

   // get available device properties
   CollectDeviceProperties(cszText);

   // check if device supports remote release control
   if (!m_sourceDevice.GetDeviceCapability(SourceDevice::capRemoteReleaseControl))
   {
      cszText += _T("Camera doesn't support remote release control");
      return cszText;
   }

   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl = m_sourceDevice.EnterReleaseControl();

   CollectCapabilities(spRemoteReleaseControl, cszText);
   CollectImageProperties(spRemoteReleaseControl, cszText);

   CollectShootingModeInfos(spRemoteReleaseControl, cszText);

   return cszText;
}

void CameraInfoDlg::CollectDeviceProperties(CString& cszText)
{
   cszText += _T("Supported device properties:\n");

   std::vector<unsigned int> vecProperties;
   try
   {
      vecProperties = m_sourceDevice.EnumDeviceProperties();
   }
   catch (CameraException& ex)
   {
      cszText.AppendFormat(_T("Error while enumerating device properties: %s\n"),
         ex.Message().GetString());
   }

   for (size_t i=0, iMax = vecProperties.size(); i<iMax; i++)
   {
      try
      {
         unsigned int uiPropertyId = vecProperties[i];
         DeviceProperty p = m_sourceDevice.GetDeviceProperty(uiPropertyId);

         cszText.AppendFormat(_T("Property %s (%08x): [%s]\n"),
            p.Name().GetString(), uiPropertyId, p.AsString().GetString());
      }
      catch(CameraException& ex)
      {
         cszText.AppendFormat(_T("Error while evaluating property %08x: %s\n"),
            vecProperties[i], ex.Message().GetString());
      }
   }

   cszText += _T("\n");
}

void CameraInfoDlg::CollectCapabilities(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, CString& cszText)
{
   cszText += _T("Remote release control capabilities:\n");

   cszText += _T("- can change shooting parameter: ");
   cszText += spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capChangeShootingParameter) ? _T("Yes") : _T("NO");
   cszText += _T("\n");

   cszText += _T("- can change shooting mode: ");
   cszText += spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capChangeShootingMode) ? _T("Yes") : _T("NO");
   cszText += _T("\n");

   cszText += _T("- can control zoom: ");
   cszText += spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capZoomControl) ? _T("Yes") : _T("NO");
   cszText += _T("\n");

   cszText += _T("- supports viewfinder: ");
   cszText += spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capViewfinder) ? _T("Yes") : _T("NO");
   cszText += _T("\n");

   cszText += _T("- can release while viewfinder is on: ");
   cszText += spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capReleaseWhileViewfinder) ? _T("Yes") : _T("NO");
   cszText += _T("\n");

   cszText += _T("- can lock/unlock AF: ");
   cszText += spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capAFLock) ? _T("Yes") : _T("NO");
   cszText += _T("\n");

   cszText += _T("\n");
}

void CameraInfoDlg::CollectImageProperties(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, CString& cszText)
{
   cszText += _T("Supported image properties:\n");

   std::vector<unsigned int> vecImageProperties = spRemoteReleaseControl->EnumImageProperties();

   if (vecImageProperties.empty())
   {
      cszText += _T("no image properties found.\n");
   }
   else
   for (size_t i=0,iMax=vecImageProperties.size(); i<iMax; i++)
   {
      unsigned int uiPropertyId = vecImageProperties[i];

      try
      {
         ImageProperty ip = spRemoteReleaseControl->GetImageProperty(uiPropertyId);

         cszText.AppendFormat(_T("Image property \"%s\" (%04x)%s: %s (%s)\n"),
            ip.Name().GetString(),
            uiPropertyId,
            ip.IsReadOnly() ? _T(" [read-only]") : _T(""),
            ip.Value().ToString().GetString(),
            ip.AsString().GetString());

         try
         {
            std::vector<ImageProperty> vecValues;
            spRemoteReleaseControl->EnumImagePropertyValues(vecImageProperties[i], vecValues);

            for (size_t j=0, jMax=vecValues.size(); j<jMax; j++)
            {
               const ImageProperty& ip2 = vecValues[j];
               cszText.AppendFormat(_T("   Valid value: %s (%s)\n"),
                  ip2.Value().ToString().GetString(),
                  ip.ValueAsString(ip2.Value()).GetString());
            }
         }
         catch (CameraException& ex)
         {
            cszText.AppendFormat(_T("Error while enumerating device property values for %08x: %s\n"),
               uiPropertyId, ex.Message().GetString());
         }
      }
      catch (CameraException& ex)
      {
         cszText.AppendFormat(_T("Error while getting device property %08x: %s\n"),
            uiPropertyId, ex.Message().GetString());
      }
   }

   cszText += _T("\n");
}

void CameraInfoDlg::CollectShootingModeInfos(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, CString& cszText)
{
   cszText += _T("Supported shooting modes:\n");

   try
   {
      if (!spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capChangeShootingMode))
      {
         cszText += _T("(Switching shooting modes not supported, can only dump current mode)\n");

         unsigned int uiShootingModePropertyId =
            spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propShootingMode);

         // no, just dump current shooting mode
         ImageProperty currentShootingMode = spRemoteReleaseControl->GetImageProperty(uiShootingModePropertyId);

         CollectShootingModeDetails(spRemoteReleaseControl, currentShootingMode, cszText);
      }
      else
      {
      // switch through all shooting modes and dump
         std::vector<ImageProperty> vecShootingModes;
         unsigned int uiPropertyId = spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propShootingMode);
         spRemoteReleaseControl->EnumImagePropertyValues(uiPropertyId, vecShootingModes);

         if (vecShootingModes.empty())
            cszText += _T("(No shooting modes found)\n");

         std::for_each(vecShootingModes.begin(), vecShootingModes.end(), [&](const ImageProperty& ip)
         {
            spRemoteReleaseControl->SetImageProperty(ip);

            CollectShootingModeDetails(spRemoteReleaseControl, ip, cszText);
         });
      }
   }
   catch (CameraException& ex)
   {
      cszText.AppendFormat(_T("Error while enumerating shooting modes: %s\n"),
         ex.Message().GetString());
   }
}

void CameraInfoDlg::CollectShootingModeDetails(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   const ImageProperty& shootingMode, CString& cszText)
{
   cszText.AppendFormat(_T("Shooting mode: %s\n"), shootingMode.AsString().GetString());

   // get Av values
   {
      std::vector<ImageProperty> vecAvValues;
      unsigned int uiAvPropertyId = spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propAv);

      try
      {
         spRemoteReleaseControl->EnumImagePropertyValues(uiAvPropertyId, vecAvValues);
      }
      catch(const CameraException& ex)
      {
         cszText.AppendFormat(_T("Error while enumerating Av values: %s\n"), ex.Message().GetString());
      }

      if (!vecAvValues.empty())
      {
         cszText += _T("  Available aperture values: ");
         for (size_t i=0, iMax=vecAvValues.size(); i<iMax; i++)
         {
            const ImageProperty& val = vecAvValues[i];

            cszText.AppendFormat(_T("%s%s"), i==0 ? _T("") : _T(", "), val.AsString().GetString());
         }
         cszText += _T("\n");
      }
   }

   // get Tv values
   {
      std::vector<ImageProperty> vecTvValues;
      unsigned int uiTvPropertyId = spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propTv);

      try
      {
         spRemoteReleaseControl->EnumImagePropertyValues(uiTvPropertyId, vecTvValues);
      }
      catch(const CameraException& ex)
      {
         cszText.AppendFormat(_T("Exception while enumerating Tv values: %s\n"), ex.Message().GetString());
      }

      if (!vecTvValues.empty())
      {
         cszText += _T("  Available shutter speed values: ");
         for (size_t i=0, iMax=vecTvValues.size(); i<iMax; i++)
         {
            const ImageProperty& val = vecTvValues[i];

            cszText.AppendFormat(_T("%s%s"), i==0 ? _T("") : _T(", "), val.AsString().GetString());
         }
         cszText += _T("\n");
      }
   }

   // get exposure compensation values
   {
      std::vector<ImageProperty> vecEcValues;
      unsigned int uiEcPropertyId = spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propExposureCompensation);

      try
      {
         spRemoteReleaseControl->EnumImagePropertyValues(uiEcPropertyId, vecEcValues);
      }
      catch(const CameraException& ex)
      {
         cszText.AppendFormat(_T("Error while enumerating exposure compensation values: %s\n"), ex.Message().GetString());
      }

      if (!vecEcValues.empty())
      {
         cszText += _T("  Available exposure compensation values: ");
         for (size_t i=0, iMax=vecEcValues.size(); i<iMax; i++)
         {
            const ImageProperty& val = vecEcValues[i];

            cszText.AppendFormat(_T("%s%s"), i==0 ? _T("") : _T(", "), val.AsString().GetString());
         }
         cszText += _T("\n");
      }
   }

   cszText += _T("\n");
}
