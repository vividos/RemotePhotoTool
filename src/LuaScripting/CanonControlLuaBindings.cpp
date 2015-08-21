//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file CanonControlLuaBindings.cpp Lua bindings for the CanonControl library
//

// includes
#include "stdafx.h"
#include "CanonControlLuaBindings.hpp"
#include "SourceInfo.hpp"
#include "SourceDevice.hpp"
#include "ShutterReleaseSettings.hpp"
#include "RemoteReleaseControl.hpp"
#include "Viewfinder.hpp"
#include "BulbReleaseControl.hpp"
#include "Asio.hpp"

/// name for Lua value in App object to store handler for asyncWaitForCamera()
LPCTSTR c_pszAsyncWaitForCamera_OnConnectedHandler = _T("__AsyncWaitForCamera_OnConnectedHandler");

/// name for Lua value in App object to store handler for setAvailImageHandler()
LPCTSTR c_pszSetAvailImageHandler_OnAvailImageHandler = _T("__SetAvailImageHandler_OnAvailImageHandler");

CanonControlLuaBindings::CanonControlLuaBindings(Lua::State& state, boost::asio::io_service::strand& strand)
:m_state(state),
m_strand(strand)
{
}

CanonControlLuaBindings::~CanonControlLuaBindings()
{
   CancelHandlers();
   CleanupBindings();
}

void CanonControlLuaBindings::InitBindings()
{
   // init of instance must be done in the same thread that actually uses camera
   m_strand.post([&]{
      m_upInstance.reset(new Instance(Instance::Get()));
   });

   /// global Sys table
   Lua::Table sys = GetState().GetTable(_T("Sys"));

   /// local instance = Sys:getInstance()
   sys.AddFunction("getInstance",
      std::bind(&CanonControlLuaBindings::SysGetInstance, shared_from_this()));

   InitConstants();
}

void CanonControlLuaBindings::InitConstants()
{
   Lua::Table constants = GetState().AddTable(_T("Constants"));

   InitSourceDeviceConstants(constants);
   InitShutterReleaseSettingsConstants(constants);
   InitRemoteReleaseControlConstants(constants);
}

void CanonControlLuaBindings::InitSourceDeviceConstants(Lua::Table& constants)
{
   Lua::Table sourceDevice = GetState().AddTable(_T(""));

   sourceDevice.AddValue(_T("capRemoteReleaseControl"), Lua::Value(SourceDevice::capRemoteReleaseControl));
   sourceDevice.AddValue(_T("capRemoteViewfinder"), Lua::Value(SourceDevice::capRemoteViewfinder));

   constants.AddValue(_T("SourceDevice"), Lua::Value(sourceDevice));
}

void CanonControlLuaBindings::InitShutterReleaseSettingsConstants(Lua::Table& constants)
{
   Lua::Table shutterReleaseSettings = GetState().AddTable(_T(""));

   shutterReleaseSettings.AddValue(_T("saveToCamera"), Lua::Value(ShutterReleaseSettings::saveToCamera));
   shutterReleaseSettings.AddValue(_T("saveToHost"), Lua::Value(ShutterReleaseSettings::saveToHost));
   shutterReleaseSettings.AddValue(_T("saveToBoth"), Lua::Value(ShutterReleaseSettings::saveToBoth));

   constants.AddValue(_T("ShutterReleaseSettings"), Lua::Value(shutterReleaseSettings));
}

void CanonControlLuaBindings::InitRemoteReleaseControlConstants(Lua::Table& constants)
{
   Lua::Table remoteReleaseControl = GetState().AddTable(_T(""));

   remoteReleaseControl.AddValue(_T("capChangeShootingParameter"), Lua::Value(RemoteReleaseControl::capChangeShootingParameter));
   remoteReleaseControl.AddValue(_T("capChangeShootingMode"), Lua::Value(RemoteReleaseControl::capChangeShootingMode));
   remoteReleaseControl.AddValue(_T("capZoomControl"), Lua::Value(RemoteReleaseControl::capZoomControl));
   remoteReleaseControl.AddValue(_T("capViewfinder"), Lua::Value(RemoteReleaseControl::capViewfinder));
   remoteReleaseControl.AddValue(_T("capReleaseWhileViewfinder"), Lua::Value(RemoteReleaseControl::capReleaseWhileViewfinder));
   remoteReleaseControl.AddValue(_T("capAFLock"), Lua::Value(RemoteReleaseControl::capAFLock));
   remoteReleaseControl.AddValue(_T("capBulbMode"), Lua::Value(RemoteReleaseControl::capBulbMode));
   remoteReleaseControl.AddValue(_T("capUILock"), Lua::Value(RemoteReleaseControl::capUILock));

   remoteReleaseControl.AddValue(_T("propEventPropertyChanged"), Lua::Value(RemoteReleaseControl::propEventPropertyChanged));
   remoteReleaseControl.AddValue(_T("propEventPropertyDescChanged"), Lua::Value(RemoteReleaseControl::propEventPropertyDescChanged));

   remoteReleaseControl.AddValue(_T("stateEventCameraShutdown"), Lua::Value(RemoteReleaseControl::stateEventCameraShutdown));
   remoteReleaseControl.AddValue(_T("stateEventRotationAngle"), Lua::Value(RemoteReleaseControl::stateEventRotationAngle));
   remoteReleaseControl.AddValue(_T("stateEventMemoryCardSlotOpen"), Lua::Value(RemoteReleaseControl::stateEventMemoryCardSlotOpen));
   remoteReleaseControl.AddValue(_T("stateEventReleaseError"), Lua::Value(RemoteReleaseControl::stateEventReleaseError));
   remoteReleaseControl.AddValue(_T("stateEventBulbExposureTime"), Lua::Value(RemoteReleaseControl::stateEventBulbExposureTime));
   remoteReleaseControl.AddValue(_T("stateEventInternalError"), Lua::Value(RemoteReleaseControl::stateEventInternalError));

   remoteReleaseControl.AddValue(_T("downloadEventStarted"), Lua::Value(RemoteReleaseControl::downloadEventStarted));
   remoteReleaseControl.AddValue(_T("downloadEventInProgress"), Lua::Value(RemoteReleaseControl::downloadEventInProgress));
   remoteReleaseControl.AddValue(_T("downloadEventFinished"), Lua::Value(RemoteReleaseControl::downloadEventFinished));

   remoteReleaseControl.AddValue(_T("commandAdjustFocus"), Lua::Value(RemoteReleaseControl::commandAdjustFocus));
   remoteReleaseControl.AddValue(_T("commandAdjustWhiteBalance"), Lua::Value(RemoteReleaseControl::commandAdjustWhiteBalance));
   remoteReleaseControl.AddValue(_T("commandAdjustExposure"), Lua::Value(RemoteReleaseControl::commandAdjustExposure));

   remoteReleaseControl.AddValue(_T("shootingModeP"), Lua::Value(RemoteReleaseControl::shootingModeP));
   remoteReleaseControl.AddValue(_T("shootingModeTv"), Lua::Value(RemoteReleaseControl::shootingModeTv));
   remoteReleaseControl.AddValue(_T("shootingModeAv"), Lua::Value(RemoteReleaseControl::shootingModeAv));
   remoteReleaseControl.AddValue(_T("shootingModeM"), Lua::Value(RemoteReleaseControl::shootingModeM));

   constants.AddValue(_T("RemoteReleaseControl"), Lua::Value(remoteReleaseControl));
}

void CanonControlLuaBindings::CancelHandlers()
{
   // cancel all callbacks that may be active
   if (m_spViewfinder != nullptr)
   {
      m_spViewfinder->SetAvailImageHandler();
      m_spViewfinder.reset();
   }

   if (m_spRemoteRelaseControl != nullptr)
   {
      // TODO
      //m_spRemoteRelaseControl->RemoveDownloadEventHandler();
      m_spRemoteRelaseControl.reset();
   }

   if (m_upInstance != nullptr)
   {
      m_upInstance->AsyncWaitForCamera();
      // m_upInstance isn't reset() here, since the bindings could be reused
   }

   // clean up all stored Lua values
   Lua::Value appValue = GetState().GetValue(_T("App"));

   if (appValue.GetType() == Lua::Value::typeTable)
   {
      Lua::Table app = appValue.Get<Lua::Table>();
      //Lua::Table app = GetState().GetTable(_T("App"));

      app.AddValue(c_pszAsyncWaitForCamera_OnConnectedHandler, Lua::Value());
      app.AddValue(c_pszSetAvailImageHandler_OnAvailImageHandler, Lua::Value());
   }

   GetState().CollectGarbage();
}

void CanonControlLuaBindings::CleanupBindings()
{
   m_upInstance.reset();

   GetState().AddValue(_T("Constants"), Lua::Value());

   GetState().CollectGarbage();
}

std::vector<Lua::Value> CanonControlLuaBindings::SysGetInstance()
{
   Lua::Table instance = GetState().AddTable(_T(""));

   instance.AddFunction("getVersion",
      std::bind(&CanonControlLuaBindings::InstanceGetVersion, shared_from_this()));

   instance.AddFunction("enumerateDevices",
      std::bind(&CanonControlLuaBindings::InstanceEnumerateDevices, shared_from_this()));

   instance.AddFunction("asyncWaitForCamera",
      std::bind(&CanonControlLuaBindings::InstanceAsyncWaitForCamera, shared_from_this(), std::placeholders::_1));

   std::vector<Lua::Value> vecRetValues;

   vecRetValues.push_back(Lua::Value(instance));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::InstanceGetVersion()
{
   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(m_upInstance->Version()));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::InstanceEnumerateDevices()
{
   std::vector<std::shared_ptr<SourceInfo>> vecSourceInfo;
   m_upInstance->EnumerateDevices(vecSourceInfo);

   std::vector<Lua::Value> vecRetValues;

   Lua::Table table = GetState().AddTable(_T(""));

   size_t uiIndex = 0;

   std::for_each(vecSourceInfo.begin(), vecSourceInfo.end(), [&](std::shared_ptr<SourceInfo> spSourceInfo){
      AddSourceInfo(table, uiIndex++, spSourceInfo);
   });

   table.AddValue(_T("length"), Lua::Value(static_cast<int>(uiIndex)));

   vecRetValues.push_back(Lua::Value(table));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::InstanceAsyncWaitForCamera(const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 1 && vecParams.size() != 2)
      throw Lua::Exception(_T("instance:asyncWaitForCamera() needs callback parameter"), GetState().GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("instance:asyncWaitForCamera() was passed an illegal 'self' value"), GetState().GetState(), __FILE__, __LINE__);

   Lua::Table app = GetState().GetTable(_T("App"));

   if (vecParams.size() == 1)
   {
      app.AddValue(c_pszAsyncWaitForCamera_OnConnectedHandler, Lua::Value());

      m_upInstance->AsyncWaitForCamera();
   }
   else
   {
      app.AddValue(c_pszAsyncWaitForCamera_OnConnectedHandler, vecParams[1]);

      auto fnAsyncWaitForCamera = std::bind(&CanonControlLuaBindings::AsyncWaitForCamera_OnCameraConnected, shared_from_this());

      m_upInstance->AsyncWaitForCamera(m_strand.wrap(fnAsyncWaitForCamera));
   }

   return std::vector<Lua::Value>();
}

void CanonControlLuaBindings::AsyncWaitForCamera_OnCameraConnected()
{
   RecursiveMutex::TryLockType lock(m_mtxAsyncWaitForCamera_InScript);
   if (!lock.Try(0))
      return; // handler is currently active, don't call it again

   Lua::Table app = GetState().GetTable(_T("App"));

   Lua::Value value = app.GetValue(c_pszAsyncWaitForCamera_OnConnectedHandler);
   if (value.GetType() != Lua::Value::typeFunction)
   {
      m_fnOutputDebugString(_T("Runtime error: callback for asyncWaitForCamera() is not a function"));
      return;
   }

   std::vector<Lua::Value> vecParams;
   vecParams.push_back(Lua::Value(app));

   try
   {
      app.CallFunction(c_pszAsyncWaitForCamera_OnConnectedHandler, 0, vecParams);
   }
   catch (const Lua::Exception& ex)
   {
      if (m_fnOutputDebugString != nullptr)
         m_fnOutputDebugString(ex.Message());
   }
}

void CanonControlLuaBindings::AddSourceInfo(Lua::Table& table, size_t uiIndex, std::shared_ptr<SourceInfo> spSourceInfo)
{
   Lua::Table sourceInfo = GetState().AddTable(_T(""));

   sourceInfo.AddValue(_T("name"), Lua::Value(spSourceInfo->Name()));

   sourceInfo.AddFunction("open",
      std::bind(&CanonControlLuaBindings::SourceInfoOpen, shared_from_this(),
         spSourceInfo, std::placeholders::_1));

   // add our table as index 1..N of parent table; Lua uses 1-based indices
   CString cszIndex;
   cszIndex.Format(_T("%u"), unsigned(uiIndex + 1));

   table.AddValue(cszIndex, Lua::Value(sourceInfo));
}

std::vector<Lua::Value> CanonControlLuaBindings::SourceInfoOpen(
   std::shared_ptr<SourceInfo> spSourceInfo,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 1)
      throw Lua::Exception(_T("invalid number of parameters to SourceInfo:open()"), GetState().GetState(), __FILE__, __LINE__);

   std::shared_ptr<SourceDevice> spSourceDevice = spSourceInfo->Open();

   Lua::Table sourceDevice = GetState().AddTable(_T(""));
   InitSourceDeviceTable(spSourceDevice, sourceDevice);

   std::vector<Lua::Value> vecRetValues;

   vecRetValues.push_back(Lua::Value(sourceDevice));

   return vecRetValues;
}

void CanonControlLuaBindings::InitSourceDeviceTable(std::shared_ptr<SourceDevice> spSourceDevice, Lua::Table& sourceDevice)
{
   sourceDevice.AddValue(_T("modelName"), Lua::Value(spSourceDevice->ModelName()));
   sourceDevice.AddValue(_T("serialNumber"), Lua::Value(spSourceDevice->SerialNumber()));

   sourceDevice.AddFunction("getDeviceCapability",
      std::bind(&CanonControlLuaBindings::SourceDeviceGetDeviceCapability, shared_from_this(), spSourceDevice, std::placeholders::_1));
   sourceDevice.AddFunction("enumDeviceProperties",
      std::bind(&CanonControlLuaBindings::SourceDeviceEnumDeviceProperties, shared_from_this(), spSourceDevice));
   sourceDevice.AddFunction("getDeviceProperty",
      std::bind(&CanonControlLuaBindings::SourceDeviceGetDeviceProperty, shared_from_this(), spSourceDevice, std::placeholders::_1));
   sourceDevice.AddFunction("enterReleaseControl",
      std::bind(&CanonControlLuaBindings::SourceDeviceEnterReleaseControl, shared_from_this(), spSourceDevice));
}

std::vector<Lua::Value> CanonControlLuaBindings::SourceDeviceGetDeviceCapability(std::shared_ptr<SourceDevice> spSourceDevice,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to SourceDevice:getDeviceCapability()"), GetState().GetState(), __FILE__, __LINE__);

   SourceDevice::T_enDeviceCapability enDeviceCapability =
      static_cast<SourceDevice::T_enDeviceCapability>(vecParams[1].Get<int>());

   bool bCapable = spSourceDevice->GetDeviceCapability(enDeviceCapability);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(bCapable));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::SourceDeviceEnumDeviceProperties(std::shared_ptr<SourceDevice> spSourceDevice)
{
   std::vector<unsigned int> vecDeviceProperties = spSourceDevice->EnumDeviceProperties();

   std::vector<Lua::Value> vecRetValues;

   if (!vecDeviceProperties.empty())
   {
      Lua::Table table = GetState().AddTable(_T(""));

      size_t uiIndex = 1;
      std::for_each(vecDeviceProperties.begin(), vecDeviceProperties.end(), [&](unsigned int uiPropertyId){
         CString cszKey;
         cszKey.Format(_T("%u"), uiIndex++);

         table.AddValue(cszKey, Lua::Value(static_cast<int>(uiPropertyId)));
      });

      table.AddValue(_T("length"), Lua::Value(static_cast<int>(uiIndex-1)));

      vecRetValues.push_back(Lua::Value(table));
   }

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::SourceDeviceGetDeviceProperty(std::shared_ptr<SourceDevice> spSourceDevice,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to SourceDevice:getDeviceProperty()"), GetState().GetState(), __FILE__, __LINE__);

   unsigned int uiPropertyId =
      static_cast<unsigned int>(vecParams[1].Get<int>());

   DeviceProperty deviceProperty = spSourceDevice->GetDeviceProperty(uiPropertyId);

   Lua::Table table = GetState().AddTable(_T(""));

   AddDeviceProperty(table, deviceProperty, spSourceDevice);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(table));

   return vecRetValues;
}

void CanonControlLuaBindings::AddDeviceProperty(Lua::Table& table, const DeviceProperty& deviceProperty, std::shared_ptr<SourceDevice> spSourceDevice)
{
   table.AddValue(_T("id"), Lua::Value(static_cast<int>(deviceProperty.Id())));
   table.AddValue(_T("name"), Lua::Value(deviceProperty.Name()));
   table.AddValue(_T("asString"), Lua::Value(deviceProperty.AsString()));
   table.AddValue(_T("isReadOnly"), Lua::Value(deviceProperty.IsReadOnly()));
   // Value(), ValidValues() and ValueAsString() are not called here, since Lua users
   // can't use Variant values anyway.
}

std::vector<Lua::Value> CanonControlLuaBindings::SourceDeviceEnterReleaseControl(std::shared_ptr<SourceDevice> spSourceDevice)
{
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl = spSourceDevice->EnterReleaseControl();
   m_spRemoteRelaseControl = spRemoteReleaseControl;

   Lua::Table remoteReleaseControl = GetState().AddTable(_T(""));
   InitRemoteReleaseControlTable(spRemoteReleaseControl, remoteReleaseControl);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(remoteReleaseControl));

   return vecRetValues;
}

void CanonControlLuaBindings::InitRemoteReleaseControlTable(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, Lua::Table& remoteReleaseControl)
{
   remoteReleaseControl.AddFunction("getCapability",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlGetCapability, shared_from_this(), spRemoteReleaseControl, std::placeholders::_1));

   remoteReleaseControl.AddFunction("setReleaseSettings",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlSetReleaseSettings, shared_from_this(), spRemoteReleaseControl, std::placeholders::_1));

   // TODO event handlers

   remoteReleaseControl.AddFunction("enumImageProperties",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlEnumImageProperties, shared_from_this(), spRemoteReleaseControl));
   remoteReleaseControl.AddFunction("getImageProperty",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlGetImageProperty, shared_from_this(), spRemoteReleaseControl, std::placeholders::_1));

   remoteReleaseControl.AddFunction("startViewfinder",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlStartViewfinder, shared_from_this(), spRemoteReleaseControl));

   remoteReleaseControl.AddFunction("numAvailableShots",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlNumAvailableShots, shared_from_this(), spRemoteReleaseControl));
   remoteReleaseControl.AddFunction("sendCommand",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlSendCommand, shared_from_this(), spRemoteReleaseControl, std::placeholders::_1));

   remoteReleaseControl.AddFunction("release",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlRelease, shared_from_this(), spRemoteReleaseControl));
   remoteReleaseControl.AddFunction("startBulb",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlStartBulb, shared_from_this(), spRemoteReleaseControl));
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlGetCapability(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:sendCommand()"), GetState().GetState(), __FILE__, __LINE__);

   RemoteReleaseControl::T_enRemoteCapability enCapability =
      static_cast<RemoteReleaseControl::T_enRemoteCapability>(vecParams[1].Get<int>());

   bool bCapability = spRemoteReleaseControl->GetCapability(enCapability);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(bCapability));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlSetReleaseSettings(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   const std::vector<Lua::Value>& /*vecParams*/)
{
   // TODO implement

   return std::vector<Lua::Value>();
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlEnumImageProperties(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
{
   std::vector<unsigned int> vecImageProperties = spRemoteReleaseControl->EnumImageProperties();

   std::vector<Lua::Value> vecRetValues;

   if (!vecImageProperties.empty())
   {
      Lua::Table table = GetState().AddTable(_T(""));

      size_t uiIndex = 1;
      std::for_each(vecImageProperties.begin(), vecImageProperties.end(), [&](unsigned int uiPropertyId){
         CString cszKey;
         cszKey.Format(_T("%u"), uiIndex++);

         table.AddValue(cszKey, Lua::Value(static_cast<int>(uiPropertyId)));
      });

      table.AddValue(_T("length"), Lua::Value(static_cast<int>(uiIndex - 1)));

      vecRetValues.push_back(Lua::Value(table));
   }

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlGetImageProperty(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:getImageProperty()"), GetState().GetState(), __FILE__, __LINE__);

   unsigned int uiPropertyId =
      static_cast<unsigned int>(vecParams[1].Get<int>());

   ImageProperty imageProperty = spRemoteReleaseControl->GetImageProperty(uiPropertyId);

   Lua::Table table = GetState().AddTable(_T(""));

   AddImageProperty(table, imageProperty, spRemoteReleaseControl);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(table));

   return vecRetValues;
}

void CanonControlLuaBindings::AddImageProperty(Lua::Table& table, const ImageProperty& imageProperty,
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
{
   table.AddValue(_T("id"), Lua::Value(static_cast<int>(imageProperty.Id())));
   table.AddValue(_T("name"), Lua::Value(imageProperty.Name()));
   table.AddValue(_T("asString"), Lua::Value(imageProperty.AsString()));
   table.AddValue(_T("isReadOnly"), Lua::Value(imageProperty.IsReadOnly()));
   // Value() and ValueAsString() are not called here, since Lua users
   // can't use Variant values anyway.
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlStartViewfinder(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
{
   std::shared_ptr<Viewfinder> spViewfinder = spRemoteReleaseControl->StartViewfinder();
   m_spViewfinder = spViewfinder;

   Lua::Table viewfinder = GetState().AddTable(_T(""));
   InitViewfinderTable(spViewfinder, viewfinder);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(viewfinder));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlNumAvailableShots(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
{
   unsigned int uiNumShots = spRemoteReleaseControl->NumAvailableShots();

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(static_cast<int>(uiNumShots)));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlSendCommand(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:sendCommand()"), GetState().GetState(), __FILE__, __LINE__);

   RemoteReleaseControl::T_enCameraCommand enCameraCommand =
      static_cast<RemoteReleaseControl::T_enCameraCommand>(vecParams[1].Get<int>());

   spRemoteReleaseControl->SendCommand(enCameraCommand);

   return std::vector<Lua::Value>();
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlRelease(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
{
   spRemoteReleaseControl->Release();

   return std::vector<Lua::Value>();
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlStartBulb(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
{
   std::shared_ptr<BulbReleaseControl> spBulbReleaseControl = spRemoteReleaseControl->StartBulb();

   Lua::Table bulbReleaseControl = GetState().AddTable(_T(""));
   InitBulbReleaseControlTable(spBulbReleaseControl, bulbReleaseControl);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(bulbReleaseControl));

   return vecRetValues;
}

void CanonControlLuaBindings::InitViewfinderTable(std::shared_ptr<Viewfinder> spViewfinder, Lua::Table& viewfinder)
{
   viewfinder.AddFunction("setAvailImageHandler",
      std::bind(&CanonControlLuaBindings::ViewfinderSetAvailImageHandler, shared_from_this(), spViewfinder, std::placeholders::_1));
}

std::vector<Lua::Value> CanonControlLuaBindings::ViewfinderSetAvailImageHandler(std::shared_ptr<Viewfinder> spViewfinder,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 1 && vecParams.size() != 2)
      throw Lua::Exception(_T("viewfinder:setAvailImageHandler() needs callback parameter"), GetState().GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("viewfinder:setAvailImageHandler() was passed an illegal 'self' value"), GetState().GetState(), __FILE__, __LINE__);

   Lua::Table app = GetState().GetTable(_T("App"));

   if (vecParams.size() == 1)
   {
      app.AddValue(c_pszSetAvailImageHandler_OnAvailImageHandler, Lua::Value());

      spViewfinder->SetAvailImageHandler(Viewfinder::T_fnOnAvailViewfinderImage());
   }
   else
   {
      app.AddValue(c_pszSetAvailImageHandler_OnAvailImageHandler, vecParams[1]);

      auto fnOnAvailImage = std::bind(&CanonControlLuaBindings::SetAvailImageHandler_OnAvailImageHandler,
         shared_from_this(), std::placeholders::_1);

      spViewfinder->SetAvailImageHandler(m_strand.wrap(fnOnAvailImage));
   }

   return std::vector<Lua::Value>();
}

void CanonControlLuaBindings::SetAvailImageHandler_OnAvailImageHandler(const std::vector<BYTE>& vecImage)
{
   Lua::Table app = GetState().GetTable(_T("App"));

   Lua::Value value = app.GetValue(c_pszSetAvailImageHandler_OnAvailImageHandler);
   if (value.GetType() != Lua::Value::typeFunction)
   {
      m_fnOutputDebugString(_T("Runtime error: callback for setAvailImageHandler() is not a function"));
      return;
   }

   std::vector<Lua::Value> vecParams;

   size_t uiSize = vecImage.size();
   Lua::Userdata userdata = GetState().AddUserdata(uiSize);
   memcpy(userdata.Data(), vecImage.data(), uiSize);

   vecParams.push_back(Lua::Value(userdata));

   try
   {
      app.CallFunction(c_pszAsyncWaitForCamera_OnConnectedHandler, 0, vecParams);
   }
   catch (const Lua::Exception& ex)
   {
      if (m_fnOutputDebugString != nullptr)
         m_fnOutputDebugString(ex.Message());
   }
}

void CanonControlLuaBindings::InitBulbReleaseControlTable(std::shared_ptr<BulbReleaseControl> spBulbReleaseControl, Lua::Table& bulbReleaseControl)
{
   bulbReleaseControl.AddFunction("elapsedTime",
      std::bind(&CanonControlLuaBindings::BulbReleaseControlElapsedTime, shared_from_this(), spBulbReleaseControl));

   bulbReleaseControl.AddFunction("stop",
      std::bind(&CanonControlLuaBindings::BulbReleaseControlStop, shared_from_this(), spBulbReleaseControl));
}

std::vector<Lua::Value> CanonControlLuaBindings::BulbReleaseControlElapsedTime(std::shared_ptr<BulbReleaseControl> spBulbReleaseControl)
{
   double dElapsed = spBulbReleaseControl->ElapsedTime();

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(dElapsed));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::BulbReleaseControlStop(std::shared_ptr<BulbReleaseControl> spBulbReleaseControl)
{
   spBulbReleaseControl->Stop();

   return std::vector<Lua::Value>();
}
