//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CmdlineApp.cpp Command line app
//

// includes
#include "stdafx.h"
#include "CmdlineApp.hpp"
#include "Exception.hpp"
#include "CameraException.hpp"
#include "AppCommand.hpp"
#include "AppOptions.hpp"
#include "Event.hpp"
#include "CameraScriptProcessor.hpp"
#include "Instance.hpp"
#include "SourceInfo.hpp"
#include "SourceDevice.hpp"
#include "RemoteReleaseControl.hpp"
#include "ShutterReleaseSettings.hpp"
#include "Filesystem.hpp"
#include "CrashReporter.hpp"
#include "..\version.h"
#include <thread>

CmdlineApp::CmdlineApp()
{
   _tprintf(_T("RemotePhotoTool Command-Line %s\n%s\n\n"),
      _T(VERSIONINFO_FILEVERSION_DISPLAYSTRING),
      _T(VERSIONINFO_COPYRIGHT));
}

void CmdlineApp::InitCrashReporter()
{
   CString cszFolder = App_GetAppDataFolder(appDataUserNonRoaming) + _T("\\RemotePhotoToolCmdline\\");

   if (!Directory_Exists(cszFolder))
      CreateDirectory(cszFolder, NULL);

   cszFolder += _T("crashdumps\\");

   if (!Directory_Exists(cszFolder))
      CreateDirectory(cszFolder, NULL);

   CrashReporter::Init(cszFolder);
}

void CmdlineApp::Run(int argc, TCHAR* argv[])
{
   // parse options
   AppOptions options(m_vecCommandList);
   options.Parse(argc, argv);

   if (m_vecCommandList.empty())
   {
      options.OutputHelp();
      return;
   }

   if (options.IsSelectedHelpOption())
      return;

   // run command list
   std::for_each(m_vecCommandList.begin(), m_vecCommandList.end(), [&](const AppCommand& cmd)
   {
      try
      {
         Exec(cmd);
      }
      catch(const CameraException& ex)
      {
         _tprintf(_T("CameraException was thrown: \"%s\"\n"), ex.Message().GetString());
      }
      catch(const Exception& ex)
      {
         _tprintf(_T("Exception was thrown: \"%s\"\n"), ex.Message().GetString());
      }
   });
}

void CmdlineApp::Exec(const AppCommand& cmd)
{
   switch (cmd.m_enCommand)
   {
   case AppCommand::showVersion: PrintVersionInfo(); break;
   case AppCommand::listDevices: ListDevices(); break;
   case AppCommand::openDevice:  OpenByName(cmd.m_cszData); break;
   case AppCommand::closeDevice:
      m_spSourceDevice.reset();
      m_spReleaseControl.reset();
      break;
   case AppCommand::deviceInfo: OutputDeviceInfo(); break;
   case AppCommand::deviceProperties: ListDeviceProperties(); break;
   case AppCommand::imageProperties: ListImageProperties(); break;
   case AppCommand::listenEvents: ListenToEvents(); break;
   case AppCommand::releaseShutter: ReleaseShutter(); break;
   case AppCommand::runScript: RunScript(cmd.m_cszData); break;
   default:
      ATLASSERT(false);
      break;
   }
}

void CmdlineApp::PrintVersionInfo()
{
   _tprintf(_T("CanonControl version info\n\n"));

   Instance inst = Instance::Get();

   CString cszVersionInfo = inst.Version();

   _tprintf(_T("%s\n"), cszVersionInfo.GetString());
}

void CmdlineApp::ListDevices()
{
   _tprintf(_T("Devices list\n"));

   Instance inst = Instance::Get();

   std::vector<std::shared_ptr<SourceInfo>> vecSourceDevices;
   inst.EnumerateDevices(vecSourceDevices);

   if (vecSourceDevices.empty())
   {
      _tprintf(_T("No device found.\n"));
   }
   else
   {
      for (size_t i=0,iMax=vecSourceDevices.size(); i<iMax; i++)
      {
         std::shared_ptr<SourceInfo> spSourceInfo = vecSourceDevices[i];
         _tprintf(_T("Device %lu: \"%s\"\n"), i+1, spSourceInfo->Name().GetString());
      }
   }

   _tprintf(_T("\n"));
}

void CmdlineApp::OpenByName(const CString& cszName)
{
   Instance inst = Instance::Get();

   std::vector<std::shared_ptr<SourceInfo>> vecSourceDevices;
   inst.EnumerateDevices(vecSourceDevices);

   int iPosOpen = cszName.Find(_T('{'));
   int iPosClose = cszName.Find(_T('}'), iPosOpen + 1);
   if (iPosOpen != -1 && iPosClose != -1)
   {
      CString cszIndex = cszName.Mid(iPosOpen + 1, iPosClose - iPosOpen - 1);

      size_t iIndex = _tcstoul(cszIndex, nullptr, 10);

      if (iIndex >= vecSourceDevices.size())
         throw Exception(_T("Invalid index for camera"), __FILE__, __LINE__);

      std::shared_ptr<SourceInfo> spSourceInfo = vecSourceDevices[iIndex];

      _tprintf(_T("Opening camera: %s\n"), spSourceInfo->Name().GetString());

      m_spSourceDevice = spSourceInfo->Open();
      return;
   }

   for (size_t i=0,iMax=vecSourceDevices.size(); i<iMax; i++)
   {
      std::shared_ptr<SourceInfo> spSourceInfo = vecSourceDevices[i];
      if (spSourceInfo->Name() == cszName)
      {
         _tprintf(_T("Opening camera: %s\n"), spSourceInfo->Name().GetString());

         m_spSourceDevice = spSourceInfo->Open();
         return;
      }
   }

   throw Exception(_T("Couldn't find camera model: ") + cszName, __FILE__, __LINE__);
}

void CmdlineApp::OutputDeviceInfo()
{
   _tprintf(_T("Device info about \"%s\"\n"), m_spSourceDevice->ModelName().GetString());
   _tprintf(_T("Serial number \"%s\"\n"), m_spSourceDevice->SerialNumber().GetString());

   // output capabilities
   _tprintf(_T("Device capabilities\n"));
   bool bCanRelease = m_spSourceDevice->GetDeviceCapability(SourceDevice::capRemoteReleaseControl);
   bool bCanUseViewfinder = m_spSourceDevice->GetDeviceCapability(SourceDevice::capRemoteViewfinder);

   _tprintf(_T("can release shutter: %s\n"), bCanRelease ? _T("yes") : _T("no"));
   _tprintf(_T("can use remote viewfinder: %s\n"), bCanUseViewfinder ? _T("yes") : _T("no"));
   _tprintf(_T("\n"));
}

void CmdlineApp::ListDeviceProperties()
{
   // output device properties
   _tprintf(_T("Device properties\n"));

   std::vector<unsigned int> vecProperties = m_spSourceDevice->EnumDeviceProperties();

   for (size_t i=0, iMax=vecProperties.size(); i<iMax; i++)
   {
      unsigned int uiPropertyId = vecProperties[i];
      DeviceProperty dp = m_spSourceDevice->GetDeviceProperty(uiPropertyId);

      _tprintf(_T("Property \"%s\" (%04x)%s: %s (%s)\n"),
         dp.Name().GetString(),
         uiPropertyId,
         dp.IsReadOnly() ? _T(" [read-only]") : _T(""),
         dp.Value().ToString().GetString(),
         dp.AsString().GetString());

      std::vector<Variant> vecValidValues = dp.ValidValues();
      for (size_t j=0, jMax=vecValidValues.size(); j<jMax; j++)
      {
         _tprintf(_T("   Valid value: %s (%s)\n"),
            vecValidValues[j].ToString().GetString(),
            dp.ValueAsString(vecValidValues[j]).GetString());
      }
   }
   _tprintf(_T("\n"));
}

void CmdlineApp::ListImageProperties()
{
   _tprintf(_T("List image properties\n"));

   EnsureReleaseControl();

   std::vector<unsigned int> vecImageProperties = m_spReleaseControl->EnumImageProperties();
   if (vecImageProperties.empty())
   {
      _tprintf(_T("no image properties found.\n"));
   }
   else
   for (size_t i=0,iMax=vecImageProperties.size(); i<iMax; i++)
   {
      unsigned int uiPropertyId = vecImageProperties[i];

      ImageProperty ip = m_spReleaseControl->GetImageProperty(uiPropertyId);

      _tprintf(_T("Image property \"%s\" (%04x)%s: %s (%s)\n"),
         ip.Name().GetString(),
         uiPropertyId,
         ip.IsReadOnly() ? _T(" [read-only]") : _T(""),
         ip.Value().ToString().GetString(),
         ip.AsString().GetString());

      std::vector<ImageProperty> vecValues;
      m_spReleaseControl->EnumImagePropertyValues(vecImageProperties[i], vecValues);

      for (size_t j=0, jMax=vecValues.size(); j<jMax; j++)
      {
         const ImageProperty& ip2 = vecValues[j];
         _tprintf(_T("   Valid value: %s (%s)\n"),
            ip2.Value().ToString().GetString(),
            ip.ValueAsString(ip2.Value()).GetString());
      }
   }
}

void CmdlineApp::ListenToEvents()
{
   _tprintf(_T("Listens for events from camera\n"));

   EnsureReleaseControl();

   int iPropertyEvent = m_spReleaseControl->AddPropertyEventHandler(
      [&](RemoteReleaseControl::T_enPropertyEvent enPropertyEvent, unsigned int uiValue)
   {
      ImageProperty prop = m_spReleaseControl->GetImageProperty(uiValue);

      _tprintf(_T("Property%s changed: Id=%04x Name=%s Value=%s\n"),
         enPropertyEvent == RemoteReleaseControl::propEventPropertyChanged ? _T("") : _T(" desc."),
         uiValue,
         prop.Name().GetString(),
         prop.AsString().GetString());
   });

   int iStateEvent = m_spReleaseControl->AddStateEventHandler(
      [&](RemoteReleaseControl::T_enStateEvent enStateEvent, unsigned int uiValue)
   {
      _tprintf(_T("State changed: State=%s Value=%u\n"),
         enStateEvent == RemoteReleaseControl::stateEventCameraShutdown ? _T("CameraShutdown") :
         enStateEvent == RemoteReleaseControl::stateEventRotationAngle ? _T("RotationAngle") :
         enStateEvent == RemoteReleaseControl::stateEventMemoryCardSlotOpen ? _T("MemoryCardSlotOpen") :
         enStateEvent == RemoteReleaseControl::stateEventReleaseError ? _T("ReleaseError") :
         enStateEvent == RemoteReleaseControl::stateEventBulbExposureTime ? _T("BulbExposureTime") :
         enStateEvent == RemoteReleaseControl::stateEventInternalError ? _T("InternalError") :
         _T("???"),
         uiValue);
   });

   _tprintf(_T("Press any key to exit listening for events...\n\n"));

   // wait for key and run OnIdle() in the meantime
   Event evtStop(true, false);
   std::thread idleThread([&evtStop]()
   {
      (void)fgetc(stdin);
      evtStop.Set();
   });

   while (!evtStop.Wait(10))
      Instance::OnIdle();

   idleThread.join();

   m_spReleaseControl->RemovePropertyEventHandler(iPropertyEvent);
   m_spReleaseControl->RemoveStateEventHandler(iStateEvent);
}

void CmdlineApp::EnsureReleaseControl()
{
   if (m_spReleaseControl != nullptr)
      return;

   if (m_spSourceDevice == nullptr)
      throw Exception(_T("Source device not opened."), __FILE__, __LINE__);

   _tprintf(_T("Starting Remote Release Control\n"));

   m_spReleaseControl = m_spSourceDevice->EnterReleaseControl();
}

void CmdlineApp::ReleaseShutter()
{
   _tprintf(_T("Release shutter\n"));

   EnsureReleaseControl();

   unsigned int uiNumAvailShot = m_spReleaseControl->NumAvailableShots();
   _tprintf(_T("Memory for %u images available\n"), uiNumAvailShot);

   Event evtPictureTaken(true, false);
   ShutterReleaseSettings settings(
      ShutterReleaseSettings::saveToHost,
      std::bind(&Event::Set, &evtPictureTaken));

   m_spReleaseControl->SetReleaseSettings(settings);

   m_spReleaseControl->Release();

   evtPictureTaken.Wait();
}

void CmdlineApp::RunScript(const CString& cszFilename)
{
   _tprintf(_T("Loading script: %s\n"), cszFilename.GetString());

   CameraScriptProcessor proc;

   proc.SetOutputDebugStringHandler(
      [&](const CString& cszText){
         _tprintf(_T("%s"), cszText.GetString());
   });

   proc.LoadScript(cszFilename);

   proc.Run();

   _tprintf(_T("Press any key to abort running script.\n\n"));

   (void)fgetc(stdin);

   proc.Stop();
}
