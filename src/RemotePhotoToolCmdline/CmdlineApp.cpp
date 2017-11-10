//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file CmdlineApp.cpp Command line app
//

// includes
#include "stdafx.h"
#include "CmdlineApp.hpp"
#include <ulib/Exception.hpp>
#include "CameraException.hpp"
#include "AppCommand.hpp"
#include "AppOptions.hpp"
#include <ulib/thread/Event.hpp>
#include "CameraScriptProcessor.hpp"
#include "Instance.hpp"
#include "SourceInfo.hpp"
#include "SourceDevice.hpp"
#include "CameraFileSystem.hpp"
#include "RemoteReleaseControl.hpp"
#include "ShutterReleaseSettings.hpp"
#include "Filesystem.hpp"
#include <ulib/Path.hpp>
#include <ulib/CrashReporter.hpp>
#include "../version.h"
#include <thread>
#include <shlobj.h>

CmdlineApp::CmdlineApp()
{
   _tprintf(_T("RemotePhotoTool Command-Line %s\n%s\n\n"),
      _T(VERSIONINFO_FILEVERSION_DISPLAYSTRING),
      _T(VERSIONINFO_COPYRIGHT));
}

void CmdlineApp::InitCrashReporter()
{
   CString cszFolder = Path::SpecialFolder(CSIDL_LOCAL_APPDATA) + _T("\\RemotePhotoToolCmdline\\");

   if (!Path(cszFolder).FolderExists())
      CreateDirectory(cszFolder, NULL);

   cszFolder += _T("crashdumps\\");

   if (!Path(cszFolder).FolderExists())
      CreateDirectory(cszFolder, NULL);

   CrashReporter::Init(cszFolder, _T("RemotePhotoToolCmdline"));
}

void CmdlineApp::Run(int argc, TCHAR* argv[])
{
   // parse options
   AppOptions options(m_vecCommandList);
   options.Parse(argc, argv);

   if (options.IsSelectedHelpOption())
      return;

   if (m_vecCommandList.empty())
   {
      options.OutputHelp();
      return;
   }

   // run command list
   std::for_each(m_vecCommandList.begin(), m_vecCommandList.end(), [&](const AppCommand& cmd)
   {
      try
      {
         Exec(cmd);
      }
      catch (const CameraException& ex)
      {
         _tprintf(_T("CameraException was thrown: \"%s\"\n"), ex.Message().GetString());
         throw;
      }
      catch (const Exception& ex)
      {
         _tprintf(_T("Exception was thrown: \"%s\"\n"), ex.Message().GetString());
         throw;
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
   case AppCommand::showFilesystem: ShowFileSystem(cmd.m_cszData); break;
   case AppCommand::deviceProperties: ListDeviceProperties(); break;
   case AppCommand::checkUnknownDeviceProps: CheckUnknownDeviceProperties(); break;
   case AppCommand::imageProperties: ListImageProperties(); break;
   case AppCommand::checkUnknownImageProps: CheckUnknownImageProperties(); break;
   case AppCommand::remoteCapabilities: ListRemoteCapabilities(); break;
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
         _tprintf(_T("Device %Iu: \"%s\"\n"), i+1, spSourceInfo->Name().GetString());
      }
   }

   _tprintf(_T("\n"));
}

void CmdlineApp::OpenByName(const CString& cszName)
{
   _tprintf(_T("Opening device %s\n"), cszName.GetString());

   Instance inst = Instance::Get();

   std::vector<std::shared_ptr<SourceInfo>> vecSourceDevices;
   inst.EnumerateDevices(vecSourceDevices);

   int iPosOpen = cszName.Find(_T('{'));
   int iPosClose = cszName.Find(_T('}'), iPosOpen + 1);
   if (iPosOpen != -1 && iPosClose != -1)
   {
      CString cszIndex = cszName.Mid(iPosOpen + 1, iPosClose - iPosOpen - 1);

      // index was passed as 1-based
      size_t iIndex = _tcstoul(cszIndex, nullptr, 10) - 1;

      if (iIndex >= vecSourceDevices.size())
         throw Exception(_T("Invalid index for camera"), __FILE__, __LINE__);

      std::shared_ptr<SourceInfo> spSourceInfo = vecSourceDevices[iIndex];

      _tprintf(_T("Opening camera: %s\n"), spSourceInfo->Name().GetString());
      _tprintf(_T("Device Id: %s\n"), spSourceInfo->DeviceId().GetString());

      try
      {
         m_spSourceDevice = spSourceInfo->Open();
      }
      catch (const CameraException& ex)
      {
         _tprintf(_T("Exception while opening camera: %s\n"), ex.Message().GetString());
         throw;
      }

      _tprintf(_T("\n"));
      return;
   }

   for (size_t i=0, iMax=vecSourceDevices.size(); i<iMax; i++)
   {
      std::shared_ptr<SourceInfo> spSourceInfo = vecSourceDevices[i];
      if (spSourceInfo->Name() == cszName)
      {
         _tprintf(_T("Opening camera: %s\n"), spSourceInfo->Name().GetString());

         m_spSourceDevice = spSourceInfo->Open();

         _tprintf(_T("\n"));
         return;
      }
   }

   throw Exception(_T("Couldn't find camera model: ") + cszName, __FILE__, __LINE__);
}

void CmdlineApp::OutputDeviceInfo()
{
   if (m_spSourceDevice == nullptr)
   {
      _tprintf(_T("No device was opened before.\n"));
      return;
   }

   _tprintf(_T("Device info about \"%s\"\n"), m_spSourceDevice->ModelName().GetString());
   _tprintf(_T("Serial number \"%s\"\n"), m_spSourceDevice->SerialNumber().GetString());
   _tprintf(_T("\n"));

   // output capabilities
   _tprintf(_T("Device capabilities\n"));
   bool bCanRelease = m_spSourceDevice->GetDeviceCapability(SourceDevice::capRemoteReleaseControl);
   bool bCanUseViewfinder = m_spSourceDevice->GetDeviceCapability(SourceDevice::capRemoteViewfinder);
   bool canUseFilesystem = m_spSourceDevice->GetDeviceCapability(SourceDevice::capCameraFileSystem);

   _tprintf(_T("can release shutter: %s\n"), bCanRelease ? _T("yes") : _T("no"));
   _tprintf(_T("can use remote viewfinder: %s\n"), bCanUseViewfinder ? _T("yes") : _T("no"));
   _tprintf(_T("can use file system: %s\n"), canUseFilesystem ? _T("yes") : _T("no"));

   _tprintf(_T("\n"));
}

/// recursively lists all files and folders in camera file system, starting from given path
void ListFilesAndFolders(std::shared_ptr<CameraFileSystem> spFileSystem, const CString& path)
{
   _tprintf(_T("Path: %s\n"), path.GetString());

   _tprintf(_T("Folders:\n"));
   std::vector<CString> allFolders = spFileSystem->EnumFolders(path);

   for (size_t index = 0, maxIndex = allFolders.size(); index < maxIndex; index++)
   {
      _tprintf(_T("[%Iu] %s\n"), index, allFolders[index].GetString());
   }

   _tprintf(_T("%Iu folder(s) found.\n\n"), allFolders.size());

   _tprintf(_T("Files:\n"));
   std::vector<FileInfo> allFiles = spFileSystem->EnumFiles(path);

   for (size_t index = 0, maxIndex = allFiles.size(); index < maxIndex; index++)
   {
      const FileInfo& fileInfo = allFiles[index];

      _tprintf(_T("File \"%s\"\n"), fileInfo.m_filename.GetString());
   }

   _tprintf(_T("%Iu file(s) found.\n\n"), allFiles.size());

   for (size_t index = 0, maxIndex = allFolders.size(); index < maxIndex; index++)
   {
      CString subPath = path + CameraFileSystem::PathSeparator + allFolders[index];

      ListFilesAndFolders(spFileSystem, subPath);
   }
}

void CmdlineApp::ShowFileSystem(const CString& path)
{
   _tprintf(_T("Shows file system\n"));

   if (m_spSourceDevice == nullptr)
   {
      _tprintf(_T("No device was opened before.\n"));
      return;
   }

   bool canUseFilesystem = m_spSourceDevice->GetDeviceCapability(SourceDevice::capCameraFileSystem);
   if (!canUseFilesystem)
   {
      _tprintf(_T("File system can't be accessed on this device.\n\n"));
      return;
   }

   std::shared_ptr<CameraFileSystem> spFileSystem = m_spSourceDevice->GetFileSystem();

   if (spFileSystem == nullptr)
   {
      _tprintf(_T("File system couldn't be opened.\n"));
      return;
   }

   ListFilesAndFolders(spFileSystem, path);
}

void CmdlineApp::ListDeviceProperties()
{
   _tprintf(_T("Device properties\n"));

   if (m_spSourceDevice == nullptr)
   {
      _tprintf(_T("No device was opened before.\n"));
      return;
   }

   std::vector<unsigned int> vecProperties = m_spSourceDevice->EnumDeviceProperties();

   for (size_t i=0, iMax=vecProperties.size(); i<iMax; i++)
   {
      unsigned int propertyId = vecProperties[i];
      DeviceProperty dp = m_spSourceDevice->GetDeviceProperty(propertyId);

      _tprintf(_T("Property \"%s\" (%04x)%s: %s (%s)\n"),
         dp.Name().GetString(),
         propertyId,
         dp.IsReadOnly() ? _T(" [read-only]") : _T(""),
         dp.Value().ToString().GetString(),
         dp.AsString().GetString());

      PrintValidDevicePropertyValues(dp);
   }

   _tprintf(_T("\n"));
}

void CmdlineApp::CheckUnknownDeviceProperties()
{
   _tprintf(_T("Checking for unknown device properties (this may take a while)\n"));

   if (m_spSourceDevice == nullptr)
   {
      _tprintf(_T("No device was opened before.\n"));
      return;
   }

   unsigned int foundProperties = 0;
   for (unsigned int propId = 0; propId < 0xffff; propId++)
   {
      try
      {
         DeviceProperty dp = m_spSourceDevice->GetDeviceProperty(propId);

         if (dp.Name() != _T("???") ||
            dp.Value().Type() == Variant::typeInvalid)
            continue;

         _tprintf(_T("Id [%04x] Name [%s] Value [%s]\n"),
            propId,
            dp.Name().GetString(),
            dp.AsString().GetString());

         PrintValidDevicePropertyValues(dp);

         foundProperties++;
      }
      catch (...)
      {
         continue;
      }
   }

   _tprintf(_T("Found %u unknown device properties\n"), foundProperties);

   _tprintf(_T("\n"));
}

void CmdlineApp::PrintValidDevicePropertyValues(const DeviceProperty& dp) const
{
   const std::vector<Variant>& vecValidValues = dp.ValidValues();
   for (size_t j = 0, jMax = vecValidValues.size(); j<jMax; j++)
   {
      _tprintf(_T("   Valid value: %s (%s)\n"),
         vecValidValues[j].ToString().GetString(),
         dp.ValueAsString(vecValidValues[j]).GetString());
   }
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
      unsigned int propertyId = vecImageProperties[i];

      ImageProperty ip = m_spReleaseControl->GetImageProperty(propertyId);

      _tprintf(_T("Image property \"%s\" (%04x)%s: %s (%s)\n"),
         ip.Name().GetString(),
         propertyId,
         ip.IsReadOnly() ? _T(" [read-only]") : _T(""),
         ip.Value().ToString().GetString(),
         ip.AsString().GetString());

      PrintValidImagePropertyValues(propertyId);
   }

   _tprintf(_T("\n"));
}

void CmdlineApp::CheckUnknownImageProperties()
{
   _tprintf(_T("Checking for unknown image properties (this may take a while)\n"));

   EnsureReleaseControl();

   // start viewfinder to potentially find more image properties
   std::shared_ptr<Viewfinder> spViewfinder;
   if (m_spReleaseControl->GetCapability(RemoteReleaseControl::capViewfinder))
      spViewfinder = m_spReleaseControl->StartViewfinder();

   unsigned int foundProperties = 0;
   for (unsigned int propId = 0; propId < 0xffff; propId++)
   {
      try
      {
         ImageProperty ip = m_spReleaseControl->GetImageProperty(propId);

         if (ip.Name() != _T("???") ||
            ip.Value().Type() == Variant::typeInvalid)
            continue;

         _tprintf(_T("Id [%04x] Name [%s] Value [%s] ReadOnly [%s]\n"),
            propId,
            ip.Name().GetString(),
            ip.AsString().GetString(),
            ip.IsReadOnly() ? _T("yes") : _T("no"));

         PrintValidImagePropertyValues(propId);

         foundProperties++;
      }
      catch (...)
      {
         continue;
      }
   }

   _tprintf(_T("Found %u unknown image properties\n"), foundProperties);

   _tprintf(_T("\n"));
}

void CmdlineApp::PrintValidImagePropertyValues(unsigned int propertyId)
{
   ATLASSERT(m_spReleaseControl != nullptr);

   std::vector<ImageProperty> vecValues;
   m_spReleaseControl->EnumImagePropertyValues(propertyId, vecValues);

   for (size_t i = 0, iMax = vecValues.size(); i<iMax; i++)
   {
      const ImageProperty& ip = vecValues[i];
      _tprintf(_T("   Valid value: %s (%s)\n"),
         ip.Value().ToString().GetString(),
         ip.ValueAsString(ip.Value()).GetString());
   }
}

void CmdlineApp::ListRemoteCapabilities()
{
   _tprintf(_T("List remote capture capabilites\n"));

   EnsureReleaseControl();

   struct SRemoteCapList
   {
      RemoteReleaseControl::T_enRemoteCapability enCapability;
      LPCTSTR pszCapabilityName;
   };

   SRemoteCapList s_capList[] =
   {
      { RemoteReleaseControl::capChangeShootingParameter, _T("can change shooting parameter at all") },
      { RemoteReleaseControl::capChangeShootingMode,      _T("ability to change shooting modes") },
      { RemoteReleaseControl::capZoomControl,             _T("can control zoom") },
      { RemoteReleaseControl::capViewfinder,              _T("can fetch live view image") },
      { RemoteReleaseControl::capReleaseWhileViewfinder,  _T("can capture during live view") },
      { RemoteReleaseControl::capAFLock,                  _T("supports AF lock/unlock") },
      { RemoteReleaseControl::capBulbMode,                _T("supports bulb mode") },
   };

   for (unsigned int i = 0; i<sizeof(s_capList) / sizeof(*s_capList); i++)
   {
      bool bSupported = m_spReleaseControl->GetCapability(s_capList[i].enCapability);

      _tprintf(_T("%s: %s\n"), s_capList[i].pszCapabilityName, bSupported ? _T("yes") : _T("no"));
   }

   _tprintf(_T("\n"));
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
   ManualResetEvent evtStop(false);
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

   ManualResetEvent evtPictureTaken(false);

   // add handler to signal when picture was taken
   ShutterReleaseSettings settings(
      ShutterReleaseSettings::saveToBoth,
      std::bind(&ManualResetEvent::Set, &evtPictureTaken));

   m_spReleaseControl->SetReleaseSettings(settings);

   // also add handler when there's a release error
   m_spReleaseControl->AddStateEventHandler(
      [&evtPictureTaken](RemoteReleaseControl::T_enStateEvent enStateEvent, unsigned int errorCode)
      {
         if (enStateEvent == RemoteReleaseControl::stateEventReleaseError)
         {
            _tprintf(_T("Error while shutter release: %u\n"), errorCode);
            evtPictureTaken.Set();
         }
      });

   m_spReleaseControl->Release();

   while (!evtPictureTaken.Wait(10))
      Instance::OnIdle();
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
