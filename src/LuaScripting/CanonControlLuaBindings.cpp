//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
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
#include <atomic>

/// name for Lua value in App object to store handler for asyncWaitForCamera()
LPCTSTR c_pszAsyncWaitForCamera_OnConnectedHandler = _T("__AsyncWaitForCamera_OnConnectedHandler");

/// name for Lua value in App object to store handler for setAvailImageHandler()
LPCTSTR c_pszSetAvailImageHandler_OnAvailImageHandler = _T("__SetAvailImageHandler_OnAvailImageHandler");

/// name for onFinishedTransfer function stored in RemoteReleaseControl table
LPCTSTR c_pszReleaseSettingsOnFinishedTransfer = _T("__ReleaseSettings_OnFinishedTransfer");

/// name for table to store PropertyEvent handler functions
LPCTSTR c_pszPropertyHandlerTable = _T("__PropertyHandler");

/// name for table to store StateEvent handler functions
LPCTSTR c_pszStateHandlerTable = _T("__StateHandler");

/// name for table to store DownloadEvent handler functions
LPCTSTR c_pszDownloadHandlerTable = _T("__DownloadHandler");

/// cycle time for event timer
const unsigned int c_uiEventTimerCycleInMilliseconds = 100;

CanonControlLuaBindings::CanonControlLuaBindings(Lua::State& state, boost::asio::io_service::strand& strand)
:m_state(state),
m_strand(strand),
m_timerEventHandling(m_strand.get_io_service()),
m_evtTimerStopped(true, false) // manual-reset event
{
}

CanonControlLuaBindings::~CanonControlLuaBindings() throw()
{
   // when one of these asserts fail, then the user of this class
   // forgot to call CancelHandlers(); but we cannot call it here
   // since we need a running Lua script worker thread for this.
   ATLASSERT(m_spRemoteRelaseControl == nullptr);
   ATLASSERT(m_spViewfinder == nullptr);

   try
   {
      CleanupBindings();
   }
   catch (...)
   {
   }
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
      std::bind(&CanonControlLuaBindings::SysGetInstance, shared_from_this(),
         std::placeholders::_1));

   InitConstants();

   RestartEventTimer();
}

void CanonControlLuaBindings::InitConstants()
{
   Lua::Table constants = GetState().AddTable(_T("Constants"));

   InitSourceDeviceConstants(constants);
   InitImagePropertyConstants(constants);
   InitShootingModeConstants(constants);
   InitShutterReleaseSettingsConstants(constants);
   InitRemoteReleaseControlConstants(constants);
   InitViewfinderConstants(constants);
}

void CanonControlLuaBindings::InitSourceDeviceConstants(Lua::Table& constants)
{
   Lua::Table sourceDevice = GetState().AddTable(_T(""));

   sourceDevice.AddValue(_T("capRemoteReleaseControl"), Lua::Value(SourceDevice::capRemoteReleaseControl));
   sourceDevice.AddValue(_T("capRemoteViewfinder"), Lua::Value(SourceDevice::capRemoteViewfinder));

   constants.AddValue(_T("SourceDevice"), Lua::Value(sourceDevice));
}

void CanonControlLuaBindings::InitImagePropertyConstants(Lua::Table& constants)
{
   Lua::Table imageProperty = GetState().AddTable(_T(""));

   imageProperty.AddValue(_T("shootingMode"), Lua::Value(T_enImagePropertyType::propShootingMode));
   imageProperty.AddValue(_T("driveMode"), Lua::Value(T_enImagePropertyType::propDriveMode));
   imageProperty.AddValue(_T("ISOSpeed"), Lua::Value(T_enImagePropertyType::propISOSpeed));
   imageProperty.AddValue(_T("meteringMode"), Lua::Value(T_enImagePropertyType::propMeteringMode));
   imageProperty.AddValue(_T("AFMode"), Lua::Value(T_enImagePropertyType::propAFMode));
   imageProperty.AddValue(_T("Av"), Lua::Value(T_enImagePropertyType::propAv));
   imageProperty.AddValue(_T("Tv"), Lua::Value(T_enImagePropertyType::propTv));
   imageProperty.AddValue(_T("exposureCompensation"), Lua::Value(T_enImagePropertyType::propExposureCompensation));
   imageProperty.AddValue(_T("flashExposureComp"), Lua::Value(T_enImagePropertyType::propFlashExposureComp));
   imageProperty.AddValue(_T("focalLength"), Lua::Value(T_enImagePropertyType::propFocalLength));
   imageProperty.AddValue(_T("flashMode"), Lua::Value(T_enImagePropertyType::propFlashMode));
   imageProperty.AddValue(_T("whiteBalance"), Lua::Value(T_enImagePropertyType::propWhiteBalance));
   imageProperty.AddValue(_T("AFDistance"), Lua::Value(T_enImagePropertyType::propAFDistance));
   imageProperty.AddValue(_T("currentZoomPos"), Lua::Value(T_enImagePropertyType::propCurrentZoomPos));
   imageProperty.AddValue(_T("maxZoomPos"), Lua::Value(T_enImagePropertyType::propMaxZoomPos));
   imageProperty.AddValue(_T("availableShots"), Lua::Value(T_enImagePropertyType::propAvailableShots));
   imageProperty.AddValue(_T("saveTo"), Lua::Value(T_enImagePropertyType::propSaveTo));
   imageProperty.AddValue(_T("batteryLevel"), Lua::Value(T_enImagePropertyType::propBatteryLevel));
   imageProperty.AddValue(_T("imageFormat"), Lua::Value(T_enImagePropertyType::propImageFormat));

   constants.AddValue(_T("ImageProperty"), Lua::Value(imageProperty));
}

void CanonControlLuaBindings::InitShootingModeConstants(Lua::Table& constants)
{
   Lua::Table shootingMode = GetState().AddTable(_T(""));

   shootingMode.AddValue(_T("shootingModeP"), Lua::Value(RemoteReleaseControl::shootingModeP));
   shootingMode.AddValue(_T("shootingModeTv"), Lua::Value(RemoteReleaseControl::shootingModeTv));
   shootingMode.AddValue(_T("shootingModeAv"), Lua::Value(RemoteReleaseControl::shootingModeAv));
   shootingMode.AddValue(_T("shootingModeM"), Lua::Value(RemoteReleaseControl::shootingModeM));

   constants.AddValue(_T("ShootingMode"), Lua::Value(shootingMode));
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

   remoteReleaseControl.AddValue(_T("saveToCamera"), Lua::Value(ShutterReleaseSettings::saveToCamera));
   remoteReleaseControl.AddValue(_T("saveToHost"), Lua::Value(ShutterReleaseSettings::saveToHost));
   remoteReleaseControl.AddValue(_T("saveToBoth"), Lua::Value(ShutterReleaseSettings::saveToBoth));

   constants.AddValue(_T("RemoteReleaseControl"), Lua::Value(remoteReleaseControl));
}

void CanonControlLuaBindings::InitViewfinderConstants(Lua::Table& constants)
{
   Lua::Table viewfinder = GetState().AddTable(_T(""));

   viewfinder.AddValue(_T("capOutputTypeVideoOut"), Lua::Value(Viewfinder::capOutputTypeVideoOut));
   viewfinder.AddValue(_T("capGetHistogram"), Lua::Value(Viewfinder::capGetHistogram));

   viewfinder.AddValue(_T("outputTypeLCD"), Lua::Value(Viewfinder::outputTypeLCD));
   viewfinder.AddValue(_T("outputTypeVideoOut"), Lua::Value(Viewfinder::outputTypeVideoOut));
   viewfinder.AddValue(_T("outputTypeOff"), Lua::Value(Viewfinder::outputTypeOff));

   viewfinder.AddValue(_T("histogramLuminance"), Lua::Value(Viewfinder::histogramLuminance));
   viewfinder.AddValue(_T("histogramRed"), Lua::Value(Viewfinder::histogramRed));
   viewfinder.AddValue(_T("histogramGreen"), Lua::Value(Viewfinder::histogramGreen));
   viewfinder.AddValue(_T("histogramBlue"), Lua::Value(Viewfinder::histogramBlue));

   constants.AddValue(_T("Viewfinder"), Lua::Value(viewfinder));
}

void CanonControlLuaBindings::LuaValueFromVariant(const Variant& value, Lua::Value& luaValue)
{
   switch (value.Type())
   {
   case Variant::typeBool:
      luaValue = Lua::Value(value.Get<bool>());
      break;

   case Variant::typeString:
      luaValue = Lua::Value(value.Get<CString>());
      break;

   case Variant::typeInt8:
      luaValue = Lua::Value(static_cast<int>(value.Get<char>()));
      break;

   case Variant::typeUInt8:
      luaValue = Lua::Value(static_cast<int>(value.Get<unsigned char>()));
      break;

   case Variant::typeInt16:
      luaValue = Lua::Value(static_cast<int>(value.Get<short>()));
      break;

   case Variant::typeUInt16:
      luaValue = Lua::Value(static_cast<int>(value.Get<unsigned short>()));
      break;

   case Variant::typeInt32:
      luaValue = Lua::Value(value.Get<int>());
      break;

   case Variant::typeUInt32:
      luaValue = Lua::Value(static_cast<double>(value.Get<unsigned int>()));
      break;

   case Variant::typeInt64:
      luaValue = Lua::Value(static_cast<double>(value.Get<__int64>()));
      break;

   case Variant::typeUInt64:
      luaValue = Lua::Value(static_cast<double>(value.Get<unsigned __int64>()));
      break;

   case Variant::typeFloat:
      luaValue = Lua::Value(static_cast<double>(value.Get<float>()));
      break;

   case Variant::typeDouble:
      luaValue = Lua::Value(value.Get<double>());
      break;

      //case Variant::typeByteBlock:
      //case Variant::typeRational:
      //case Variant::typePoint:
      //case Variant::typeRect:
      //case Variant::typeTime:
   default:
      break; // value stays nil
   }
}

void CanonControlLuaBindings::RestartEventTimer()
{
   m_evtTimerStopped.Reset();

   m_timerEventHandling.expires_from_now(boost::posix_time::milliseconds(c_uiEventTimerCycleInMilliseconds));
   m_timerEventHandling.async_wait(
      m_strand.wrap(
         std::bind(&CanonControlLuaBindings::OnTimerEventHandling, shared_from_this(), std::placeholders::_1)));
}

void CanonControlLuaBindings::OnTimerEventHandling(const boost::system::error_code& error)
{
   if (error)
   {
      // timer was canceled
      m_evtTimerStopped.Set();
      return;
   }

   Instance::OnIdle();

   RestartEventTimer();
}

void CanonControlLuaBindings::CancelHandlers()
{
   // cancel all callbacks that may be active
   if (m_spViewfinder != nullptr)
   {
      m_spViewfinder->SetAvailImageHandler();

      m_spViewfinder->Close();

      m_spViewfinder.reset();
   }

   if (m_spRemoteRelaseControl != nullptr)
   {
      std::for_each(m_setAllPropertyHandlerIds.begin(), m_setAllPropertyHandlerIds.end(), [&](int iHandlerId)
      {
         m_spRemoteRelaseControl->RemovePropertyEventHandler(iHandlerId);
      });

      std::for_each(m_setAllStateHandlerIds.begin(), m_setAllStateHandlerIds.end(), [&](int iHandlerId)
      {
         m_spRemoteRelaseControl->RemoveStateEventHandler(iHandlerId);
      });

      std::for_each(m_setAllDownloadHandlerIds.begin(), m_setAllDownloadHandlerIds.end(), [&](int iHandlerId)
      {
         m_spRemoteRelaseControl->RemoveDownloadEventHandler(iHandlerId);
      });

      m_releaseSettings.HandlerOnFinishedTransfer(ShutterReleaseSettings::T_fnOnFinishedTransfer());
      m_spRemoteRelaseControl->SetReleaseSettings(m_releaseSettings);

      m_spRemoteRelaseControl->Close();

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

      app.AddValue(c_pszAsyncWaitForCamera_OnConnectedHandler, Lua::Value());
      app.AddValue(c_pszSetAvailImageHandler_OnAvailImageHandler, Lua::Value());
   }

   GetState().CollectGarbage();
}

void CanonControlLuaBindings::StopTimer()
{
   m_timerEventHandling.cancel();

   std::atomic<bool> finished = false;

   do
   {
      m_strand.post([&] {
         finished = m_evtTimerStopped.Wait(0);
      });
      Sleep(50);
   } while (!finished);
}

void CanonControlLuaBindings::CleanupBindings()
{
   m_upInstance.reset();

   GetState().AddValue(_T("Constants"), Lua::Value());

   GetState().CollectGarbage();
}

std::vector<Lua::Value> CanonControlLuaBindings::SysGetInstance(Lua::State& state)
{
   Lua::Table instance = state.AddTable(_T(""));

   instance.AddFunction("getVersion",
      std::bind(&CanonControlLuaBindings::InstanceGetVersion, shared_from_this()));

   instance.AddFunction("enumerateDevices",
      std::bind(&CanonControlLuaBindings::InstanceEnumerateDevices, shared_from_this(),
         std::placeholders::_1));

   instance.AddFunction("asyncWaitForCamera",
      std::bind(&CanonControlLuaBindings::InstanceAsyncWaitForCamera, shared_from_this(),
         std::placeholders::_1, std::placeholders::_2));

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

std::vector<Lua::Value> CanonControlLuaBindings::InstanceEnumerateDevices(Lua::State& state)
{
   std::vector<std::shared_ptr<SourceInfo>> vecSourceInfo;
   m_upInstance->EnumerateDevices(vecSourceInfo);

   std::vector<Lua::Value> vecRetValues;

   Lua::Table table = state.AddTable(_T(""));

   size_t uiIndex = 0;

   std::for_each(vecSourceInfo.begin(), vecSourceInfo.end(), [&](std::shared_ptr<SourceInfo> spSourceInfo){
      AddSourceInfo(state, table, uiIndex++, spSourceInfo);
   });

   table.AddValue(_T("length"), Lua::Value(static_cast<int>(uiIndex)));

   vecRetValues.push_back(Lua::Value(table));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::InstanceAsyncWaitForCamera(
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 1 && vecParams.size() != 2)
      throw Lua::Exception(_T("instance:asyncWaitForCamera() needs callback parameter"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("instance:asyncWaitForCamera() was passed an illegal 'self' value"), state.GetState(), __FILE__, __LINE__);

   Lua::Table app = state.GetTable(_T("App"));

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
   // this assertion fails when a handler is still attached to a
   // bindings object that was destroyed earlier.
   ATLASSERT(this != nullptr);

   RecursiveMutex::TryLockType lock(m_mtxAsyncWaitForCamera_InScript);
   if (!lock.Try(0))
      return; // handler is currently active, don't call it again

   Lua::Table app = GetState().GetTable(_T("App"));

   Lua::Value value = app.GetValue(c_pszAsyncWaitForCamera_OnConnectedHandler);
   if (value.GetType() != Lua::Value::typeFunction)
   {
      if (m_fnOutputDebugString != nullptr)
         m_fnOutputDebugString(_T("Runtime error: callback for asyncWaitForCamera() is not a function\n"));

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

void CanonControlLuaBindings::AddSourceInfo(Lua::State& state, Lua::Table& table, size_t uiIndex, std::shared_ptr<SourceInfo> spSourceInfo)
{
   Lua::Table sourceInfo = state.AddTable(_T(""));

   sourceInfo.AddValue(_T("name"), Lua::Value(spSourceInfo->Name()));
   sourceInfo.AddValue(_T("deviceId"), Lua::Value(spSourceInfo->DeviceId()));

   sourceInfo.AddFunction("open",
      std::bind(&CanonControlLuaBindings::SourceInfoOpen, shared_from_this(),
         spSourceInfo, std::placeholders::_1, std::placeholders::_2));

   // add our table as index 1..N of parent table; Lua uses 1-based indices
   table.AddValue(uiIndex + 1, Lua::Value(sourceInfo));
}

std::vector<Lua::Value> CanonControlLuaBindings::SourceInfoOpen(
   std::shared_ptr<SourceInfo> spSourceInfo,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 1)
      throw Lua::Exception(_T("invalid number of parameters to SourceInfo:open()"), state.GetState(), __FILE__, __LINE__);

   std::shared_ptr<SourceDevice> spSourceDevice = spSourceInfo->Open();

   Lua::Table sourceDevice = state.AddTable(_T(""));
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
      std::bind(&CanonControlLuaBindings::SourceDeviceGetDeviceCapability, shared_from_this(),
         spSourceDevice, std::placeholders::_1, std::placeholders::_2));

   sourceDevice.AddFunction("enumDeviceProperties",
      std::bind(&CanonControlLuaBindings::SourceDeviceEnumDeviceProperties, shared_from_this(),
         spSourceDevice, std::placeholders::_1));

   sourceDevice.AddFunction("getDeviceProperty",
      std::bind(&CanonControlLuaBindings::SourceDeviceGetDeviceProperty, shared_from_this(),
         spSourceDevice, std::placeholders::_1, std::placeholders::_2));

   sourceDevice.AddFunction("enterReleaseControl",
      std::bind(&CanonControlLuaBindings::SourceDeviceEnterReleaseControl, shared_from_this(),
         spSourceDevice, std::placeholders::_1));
}

std::vector<Lua::Value> CanonControlLuaBindings::SourceDeviceGetDeviceCapability(
   std::shared_ptr<SourceDevice> spSourceDevice,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to SourceDevice:getDeviceCapability()"),
         state.GetState(), __FILE__, __LINE__);

   SourceDevice::T_enDeviceCapability enDeviceCapability =
      static_cast<SourceDevice::T_enDeviceCapability>(vecParams[1].Get<int>());

   bool bCapable = spSourceDevice->GetDeviceCapability(enDeviceCapability);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(bCapable));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::SourceDeviceEnumDeviceProperties(
   std::shared_ptr<SourceDevice> spSourceDevice,
   Lua::State& state)
{
   std::vector<unsigned int> vecDeviceProperties = spSourceDevice->EnumDeviceProperties();

   std::vector<Lua::Value> vecRetValues;

   if (!vecDeviceProperties.empty())
   {
      Lua::Table table = state.AddTable(_T(""));

      size_t uiIndex = 1;
      std::for_each(vecDeviceProperties.begin(), vecDeviceProperties.end(), [&](unsigned int uiPropertyId){
         table.AddValue(uiIndex++, Lua::Value(static_cast<int>(uiPropertyId)));
      });

      table.AddValue(_T("length"), Lua::Value(static_cast<int>(uiIndex-1)));

      vecRetValues.push_back(Lua::Value(table));
   }

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::SourceDeviceGetDeviceProperty(
   std::shared_ptr<SourceDevice> spSourceDevice,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to SourceDevice:getDeviceProperty()"), state.GetState(), __FILE__, __LINE__);

   unsigned int uiPropertyId =
      static_cast<unsigned int>(vecParams[1].Get<int>());

   DeviceProperty deviceProperty = spSourceDevice->GetDeviceProperty(uiPropertyId);

   Lua::Table table = state.AddTable(_T(""));

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

   Variant value = deviceProperty.Value();

   Lua::Value luaValue;
   if (!value.IsArray())
   {
      LuaValueFromVariant(value, luaValue);
   }

   table.AddValue(_T("value"), luaValue);
}

std::vector<Lua::Value> CanonControlLuaBindings::SourceDeviceEnterReleaseControl(
   std::shared_ptr<SourceDevice> spSourceDevice,
   Lua::State& state)
{
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl = spSourceDevice->EnterReleaseControl();
   m_spRemoteRelaseControl = spRemoteReleaseControl;

   m_releaseSettings = ShutterReleaseSettings(
      ShutterReleaseSettings::saveToBoth,
      ShutterReleaseSettings::T_fnOnFinishedTransfer());
   m_releaseSettings.Filename(_T("images/IMG_0000.jpg"));

   Lua::Table remoteReleaseControl = state.AddTable(_T(""));
   InitRemoteReleaseControlTable(spRemoteReleaseControl, remoteReleaseControl);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(remoteReleaseControl));

   return vecRetValues;
}

void CanonControlLuaBindings::InitRemoteReleaseControlTable(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, Lua::Table& remoteReleaseControl)
{
   remoteReleaseControl.AddFunction("getCapability",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlGetCapability, shared_from_this(), spRemoteReleaseControl,
         std::placeholders::_1, std::placeholders::_2));

   remoteReleaseControl.AddFunction("getReleaseSettings",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlGetReleaseSettings, shared_from_this(), spRemoteReleaseControl,
         std::placeholders::_1, std::placeholders::_2));

   remoteReleaseControl.AddFunction("setReleaseSettings",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlSetReleaseSettings, shared_from_this(), spRemoteReleaseControl,
         std::placeholders::_1, std::placeholders::_2));

   remoteReleaseControl.AddFunction("addPropertyEventHandler",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlAddPropertyEventHandler, shared_from_this(), spRemoteReleaseControl,
         std::placeholders::_1, std::placeholders::_2));

   remoteReleaseControl.AddFunction("removePropertyEventHandler",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlRemovePropertyEventHandler, shared_from_this(), spRemoteReleaseControl,
         std::placeholders::_1, std::placeholders::_2));

   remoteReleaseControl.AddFunction("addStateEventHandler",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlAddStateEventHandler, shared_from_this(), spRemoteReleaseControl,
         std::placeholders::_1, std::placeholders::_2));

   remoteReleaseControl.AddFunction("removeStateEventHandler",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlRemoveStateEventHandler, shared_from_this(), spRemoteReleaseControl,
         std::placeholders::_1, std::placeholders::_2));

   remoteReleaseControl.AddFunction("addDownloadEventHandler",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlAddDownloadEventHandler, shared_from_this(), spRemoteReleaseControl,
         std::placeholders::_1, std::placeholders::_2));

   remoteReleaseControl.AddFunction("removeDownloadEventHandler",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlRemoveDownloadEventHandler, shared_from_this(), spRemoteReleaseControl,
         std::placeholders::_1, std::placeholders::_2));

   remoteReleaseControl.AddFunction("getImagePropertyByType",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlGetImagePropertyByType, shared_from_this(), spRemoteReleaseControl,
         std::placeholders::_1, std::placeholders::_2));

   remoteReleaseControl.AddFunction("getShootingModeImageProperty",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlGetShootingModeImageProperty, shared_from_this(), spRemoteReleaseControl,
         std::placeholders::_1, std::placeholders::_2));

   remoteReleaseControl.AddFunction("enumImageProperties",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlEnumImageProperties, shared_from_this(),
         spRemoteReleaseControl, std::placeholders::_1));

   remoteReleaseControl.AddFunction("getImageProperty",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlGetImageProperty, shared_from_this(), spRemoteReleaseControl,
         std::placeholders::_1, std::placeholders::_2));

   remoteReleaseControl.AddFunction("startViewfinder",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlStartViewfinder, shared_from_this(),
         spRemoteReleaseControl, std::placeholders::_1));

   remoteReleaseControl.AddFunction("numAvailableShots",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlNumAvailableShots, shared_from_this(), spRemoteReleaseControl));
   remoteReleaseControl.AddFunction("sendCommand",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlSendCommand, shared_from_this(), spRemoteReleaseControl,
         std::placeholders::_1, std::placeholders::_2));

   remoteReleaseControl.AddFunction("release",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlRelease, shared_from_this(), spRemoteReleaseControl));
   remoteReleaseControl.AddFunction("startBulb",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlStartBulb, shared_from_this(),
         spRemoteReleaseControl, std::placeholders::_1));
   remoteReleaseControl.AddFunction("close",
      std::bind(&CanonControlLuaBindings::RemoteReleaseControlClose, shared_from_this(), spRemoteReleaseControl));

   // internal values
   Lua::Table app = GetState().GetTable(_T("App"));
   app.AddValue(c_pszReleaseSettingsOnFinishedTransfer, Lua::Value());
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlGetCapability(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:getCapability()"), state.GetState(), __FILE__, __LINE__);

   RemoteReleaseControl::T_enRemoteCapability enCapability =
      static_cast<RemoteReleaseControl::T_enRemoteCapability>(vecParams[1].Get<int>());

   bool bCapability = spRemoteReleaseControl->GetCapability(enCapability);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(bCapability));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlGetReleaseSettings(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 1)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:getReleaseSettings()"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("first parameter must be the RemoteReleaseControl table"), state.GetState(), __FILE__, __LINE__);

   Lua::Table tableReleaseSettings = state.AddTable(_T(""));
   InitReleaseSettingsTable(state, m_releaseSettings, tableReleaseSettings);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(tableReleaseSettings));

   return vecRetValues;
}

void CanonControlLuaBindings::InitReleaseSettingsTable(
   Lua::State& state,
   const ShutterReleaseSettings& releaseSettings,
   Lua::Table& tableReleaseSettings)
{
   Lua::Table app = state.GetTable(_T("App"));
   Lua::Value onFinishedTransfer = app.GetValue(c_pszReleaseSettingsOnFinishedTransfer);

   tableReleaseSettings.AddValue(_T("saveTarget"), Lua::Value(static_cast<int>(releaseSettings.SaveTarget())));
   tableReleaseSettings.AddValue(_T("outputFilename"), Lua::Value(m_releaseSettings.Filename()));
   tableReleaseSettings.AddValue(_T("onFinishedTransfer"), onFinishedTransfer);
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlSetReleaseSettings(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:setReleaseSettings()"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("first parameter must be the RemoteReleaseControl table"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[1].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("second parameter must be the ReleaseSettings table"), state.GetState(), __FILE__, __LINE__);

   //Lua::Table tableRemoteReleaseControl = vecParams[0].Get<Lua::Table>();
   Lua::Table tableReleaseSettings = vecParams[1].Get<Lua::Table>();

   m_releaseSettings.SaveTarget(
      static_cast<ShutterReleaseSettings::T_enSaveTarget>(
         tableReleaseSettings.GetValue(_T("saveTarget")).Get<int>()));

   m_releaseSettings.Filename(
      tableReleaseSettings.GetValue(_T("outputFilename")).Get<CString>());

   Lua::Value onFinishedTransfer = tableReleaseSettings.GetValue(_T("onFinishedTransfer"));

   Lua::Table app = state.GetTable(_T("App"));
   app.AddValue(c_pszReleaseSettingsOnFinishedTransfer, onFinishedTransfer);

   if (onFinishedTransfer.GetType() != Lua::Value::typeNil)
   {
      auto fnOnFinishedTransfer = std::bind(
         &CanonControlLuaBindings::SetReleaseSettings_OnFinishedTransfer,
         shared_from_this(),
         spRemoteReleaseControl,
         std::placeholders::_1);

      m_releaseSettings.HandlerOnFinishedTransfer(m_strand.wrap(fnOnFinishedTransfer));
   }
   else
      m_releaseSettings.HandlerOnFinishedTransfer(ShutterReleaseSettings::T_fnOnFinishedTransfer());

   m_spRemoteRelaseControl->SetReleaseSettings(m_releaseSettings);

   return std::vector<Lua::Value>();
}

void CanonControlLuaBindings::SetReleaseSettings_OnFinishedTransfer(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   const ShutterReleaseSettings& releaseSettings)
{
   Lua::State& state = GetState();

   Lua::Table app = state.GetTable(_T("App"));
   Lua::Value value = app.GetValue(c_pszReleaseSettingsOnFinishedTransfer);

   if (value.GetType() != Lua::Value::typeFunction)
   {
      if (m_fnOutputDebugString != nullptr)
         m_fnOutputDebugString(_T("Runtime error: callback for setReleaseSettings() is not a function\n"));

      return;
   }

   std::vector<Lua::Value> vecParams;

   Lua::Table tableReleaseSettings = state.AddTable(_T(""));
   InitReleaseSettingsTable(state, releaseSettings, tableReleaseSettings);

   vecParams.push_back(Lua::Value(tableReleaseSettings));

   try
   {
      app.CallFunction(c_pszReleaseSettingsOnFinishedTransfer, 0, vecParams);
   }
   catch (const Lua::Exception& ex)
   {
      if (m_fnOutputDebugString != nullptr)
         m_fnOutputDebugString(ex.Message() + _T("\n"));
   }
}

void CanonControlLuaBindings::RemoteReleaseControl_PropertyEventHandler(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   std::shared_ptr<int> spHandlerId,
   RemoteReleaseControl::T_enPropertyEvent enPropertyEvent,
   unsigned int eventData)
{
   Lua::State& state = GetState();

   Lua::Table app = state.GetTable(_T("App"));
   Lua::Table handlerTable = app.GetValue(c_pszPropertyHandlerTable).Get<Lua::Table>();

   int iHandlerId = *spHandlerId.get();

   Lua::Value callbackFunction = handlerTable.GetValue(iHandlerId);

   std::vector<Lua::Value> vecParams;

   vecParams.push_back(Lua::Value(app));
   vecParams.push_back(Lua::Value(static_cast<int>(enPropertyEvent)));
   vecParams.push_back(Lua::Value(static_cast<int>(eventData)));

   try
   {
      Lua::Function func = callbackFunction.Get<Lua::Function>();
      func.Call(0, vecParams);
   }
   catch (const Lua::Exception& ex)
   {
      if (m_fnOutputDebugString != nullptr)
         m_fnOutputDebugString(ex.Message() + _T("\n"));
   }
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlAddPropertyEventHandler(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:addPropertyEventHandler()"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("first parameter must be the RemoteReleaseControl table"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[1].GetType() != Lua::Value::typeFunction)
      throw Lua::Exception(_T("second parameter must be callback function"), state.GetState(), __FILE__, __LINE__);

   // bind handler function and store handler id in its argument
   std::shared_ptr<int> spHandlerId(new int);

   auto fn = std::bind(
      &CanonControlLuaBindings::RemoteReleaseControl_PropertyEventHandler,
      shared_from_this(),
      spRemoteReleaseControl,
      spHandlerId,
      std::placeholders::_1,
      std::placeholders::_2);

   int iHandlerId = spRemoteReleaseControl->AddPropertyEventHandler(m_strand.wrap(fn));

   *spHandlerId = iHandlerId;

   m_setAllPropertyHandlerIds.insert(iHandlerId);

   // store function as value to an array value with index
   Lua::Value function = vecParams[1];

   Lua::Table app = state.GetTable(_T("App"));

   if (app.GetValue(c_pszPropertyHandlerTable).GetType() == Lua::Value::typeNil)
   {
      Lua::Table table = state.AddTable(_T(""));
      app.AddValue(c_pszPropertyHandlerTable, Lua::Value(table));
   }

   Lua::Table handlerTable = app.GetValue(c_pszPropertyHandlerTable).Get<Lua::Table>();

   handlerTable.AddValue(iHandlerId, function);

   // return handler id
   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(iHandlerId));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlRemovePropertyEventHandler(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:removePropertyEventHandler()"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("first parameter must be the RemoteReleaseControl table"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[1].GetType() != Lua::Value::typeNumber)
      throw Lua::Exception(_T("second parameter must be handler id"), state.GetState(), __FILE__, __LINE__);

   // remove handler
   int iHandlerId = vecParams[1].Get<int>();

   spRemoteReleaseControl->RemovePropertyEventHandler(iHandlerId);

   m_setAllPropertyHandlerIds.erase(iHandlerId);

   // remove function from handler table
   Lua::Table app = state.GetTable(_T("App"));
   Lua::Table handlerTable = app.GetValue(c_pszPropertyHandlerTable).Get<Lua::Table>();

   handlerTable.AddValue(iHandlerId, Lua::Value());

   return std::vector<Lua::Value>();
}

void CanonControlLuaBindings::RemoteReleaseControl_StateEventHandler(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   std::shared_ptr<int> spHandlerId,
   RemoteReleaseControl::T_enStateEvent enStateEvent,
   unsigned int eventData)
{
   Lua::State& state = GetState();

   Lua::Table app = state.GetTable(_T("App"));
   Lua::Table handlerTable = app.GetValue(c_pszStateHandlerTable).Get<Lua::Table>();

   int iHandlerId = *spHandlerId.get();

   Lua::Value callbackFunction = handlerTable.GetValue(iHandlerId);

   std::vector<Lua::Value> vecParams;

   vecParams.push_back(Lua::Value(app));
   vecParams.push_back(Lua::Value(static_cast<int>(enStateEvent)));
   vecParams.push_back(Lua::Value(static_cast<int>(eventData)));

   try
   {
      Lua::Function func = callbackFunction.Get<Lua::Function>();
      func.Call(0, vecParams);
   }
   catch (const Lua::Exception& ex)
   {
      if (m_fnOutputDebugString != nullptr)
         m_fnOutputDebugString(ex.Message() + _T("\n"));
   }
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlAddStateEventHandler(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:addStateEventHandler()"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("first parameter must be the RemoteReleaseControl table"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[1].GetType() != Lua::Value::typeFunction)
      throw Lua::Exception(_T("second parameter must be callback function"), state.GetState(), __FILE__, __LINE__);

   // bind handler function and store handler id in its argument
   std::shared_ptr<int> spHandlerId(new int);

   auto fn = std::bind(
      &CanonControlLuaBindings::RemoteReleaseControl_StateEventHandler,
      shared_from_this(),
      spRemoteReleaseControl,
      spHandlerId,
      std::placeholders::_1,
      std::placeholders::_2);

   int iHandlerId = spRemoteReleaseControl->AddStateEventHandler(m_strand.wrap(fn));

   *spHandlerId = iHandlerId;

   m_setAllStateHandlerIds.insert(iHandlerId);

   // store function as value to an array value with index
   Lua::Value function = vecParams[1];

   Lua::Table app = state.GetTable(_T("App"));

   if (app.GetValue(c_pszStateHandlerTable).GetType() == Lua::Value::typeNil)
   {
      Lua::Table table = state.AddTable(_T(""));
      app.AddValue(c_pszStateHandlerTable, Lua::Value(table));
   }

   Lua::Table handlerTable = app.GetValue(c_pszStateHandlerTable).Get<Lua::Table>();

   handlerTable.AddValue(iHandlerId, function);

   // return handler id
   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(iHandlerId));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlRemoveStateEventHandler(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:removeStateEventHandler()"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("first parameter must be the RemoteReleaseControl table"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[1].GetType() != Lua::Value::typeNumber)
      throw Lua::Exception(_T("second parameter must be handler id"), state.GetState(), __FILE__, __LINE__);

   // remove handler
   int iHandlerId = vecParams[1].Get<int>();

   spRemoteReleaseControl->RemoveStateEventHandler(iHandlerId);

   m_setAllStateHandlerIds.erase(iHandlerId);

   // remove function from handler table
   Lua::Table app = state.GetTable(_T("App"));
   Lua::Table handlerTable = app.GetValue(c_pszStateHandlerTable).Get<Lua::Table>();

   handlerTable.AddValue(iHandlerId, Lua::Value());

   return std::vector<Lua::Value>();
}

void CanonControlLuaBindings::RemoteReleaseControl_DownloadEventHandler(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   std::shared_ptr<int> spHandlerId,
   RemoteReleaseControl::T_enDownloadEvent enDownloadEvent,
   unsigned int eventData)
{
   Lua::State& state = GetState();

   Lua::Table app = state.GetTable(_T("App"));
   Lua::Table handlerTable = app.GetValue(c_pszDownloadHandlerTable).Get<Lua::Table>();

   int iHandlerId = *spHandlerId.get();

   Lua::Value callbackFunction = handlerTable.GetValue(iHandlerId);

   std::vector<Lua::Value> vecParams;

   vecParams.push_back(Lua::Value(app));
   vecParams.push_back(Lua::Value(static_cast<int>(enDownloadEvent)));
   vecParams.push_back(Lua::Value(static_cast<int>(eventData)));

   try
   {
      Lua::Function func = callbackFunction.Get<Lua::Function>();
      func.Call(0, vecParams);
   }
   catch (const Lua::Exception& ex)
   {
      if (m_fnOutputDebugString != nullptr)
         m_fnOutputDebugString(ex.Message() + _T("\n"));
   }
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlAddDownloadEventHandler(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:addDownloadEventHandler()"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("first parameter must be the RemoteReleaseControl table"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[1].GetType() != Lua::Value::typeFunction)
      throw Lua::Exception(_T("second parameter must be callback function"), state.GetState(), __FILE__, __LINE__);

   // bind handler function and store handler id in its argument
   std::shared_ptr<int> spHandlerId(new int);

   auto fn = std::bind(
      &CanonControlLuaBindings::RemoteReleaseControl_DownloadEventHandler,
      shared_from_this(),
      spRemoteReleaseControl,
      spHandlerId,
      std::placeholders::_1,
      std::placeholders::_2);

   int iHandlerId = spRemoteReleaseControl->AddDownloadEventHandler(m_strand.wrap(fn));

   *spHandlerId = iHandlerId;

   m_setAllDownloadHandlerIds.insert(iHandlerId);

   // store function as value to an array value with index
   Lua::Value function = vecParams[1];

   Lua::Table app = state.GetTable(_T("App"));

   if (app.GetValue(c_pszDownloadHandlerTable).GetType() == Lua::Value::typeNil)
   {
      Lua::Table table = state.AddTable(_T(""));
      app.AddValue(c_pszDownloadHandlerTable, Lua::Value(table));
   }

   Lua::Table handlerTable = app.GetValue(c_pszDownloadHandlerTable).Get<Lua::Table>();

   handlerTable.AddValue(iHandlerId, function);

   // return handler id
   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(iHandlerId));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlRemoveDownloadEventHandler(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:removeDownloadEventHandler()"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("first parameter must be the RemoteReleaseControl table"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[1].GetType() != Lua::Value::typeNumber)
      throw Lua::Exception(_T("second parameter must be handler id"), state.GetState(), __FILE__, __LINE__);

   // remove handler
   int iHandlerId = vecParams[1].Get<int>();

   spRemoteReleaseControl->RemoveDownloadEventHandler(iHandlerId);

   m_setAllDownloadHandlerIds.erase(iHandlerId);

   // remove function from handler table
   Lua::Table app = state.GetTable(_T("App"));
   Lua::Table handlerTable = app.GetValue(c_pszDownloadHandlerTable).Get<Lua::Table>();

   handlerTable.AddValue(iHandlerId, Lua::Value());

   return std::vector<Lua::Value>();
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlEnumImageProperties(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state)
{
   std::vector<unsigned int> vecImageProperties = spRemoteReleaseControl->EnumImageProperties();

   std::vector<Lua::Value> vecRetValues;

   if (!vecImageProperties.empty())
   {
      Lua::Table table = state.AddTable(_T(""));

      size_t uiIndex = 1;
      std::for_each(vecImageProperties.begin(), vecImageProperties.end(), [&](unsigned int uiPropertyId){
         table.AddValue(uiIndex++, Lua::Value(static_cast<int>(uiPropertyId)));
      });

      table.AddValue(_T("length"), Lua::Value(static_cast<int>(uiIndex - 1)));

      vecRetValues.push_back(Lua::Value(table));
   }

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlGetImageProperty(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:getImageProperty()"), state.GetState(), __FILE__, __LINE__);

   unsigned int uiPropertyId =
      static_cast<unsigned int>(vecParams[1].Get<int>());

   ImageProperty imageProperty = spRemoteReleaseControl->GetImageProperty(uiPropertyId);

   Lua::Table table = state.AddTable(_T(""));

   AddImageProperty(table, imageProperty, spRemoteReleaseControl);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(table));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlGetImagePropertyByType(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:getImagePropertyByType()"), state.GetState(), __FILE__, __LINE__);

   T_enImagePropertyType enImagePropertyType =
      static_cast<T_enImagePropertyType>(vecParams[1].Get<int>());

   unsigned int imagePropertyId = spRemoteReleaseControl->MapImagePropertyTypeToId(enImagePropertyType);

   ImageProperty imageProperty = spRemoteReleaseControl->GetImageProperty(imagePropertyId);

   Lua::Table table = state.AddTable(_T(""));

   AddImageProperty(table, imageProperty, spRemoteReleaseControl);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(table));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlGetShootingModeImageProperty(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:getShootingModeImageProperty()"), state.GetState(), __FILE__, __LINE__);

   RemoteReleaseControl::T_enShootingMode enShootingMode =
      static_cast<RemoteReleaseControl::T_enShootingMode>(vecParams[1].Get<int>());

   ImageProperty imageProperty = spRemoteReleaseControl->MapShootingModeToImagePropertyValue(enShootingMode);

   Lua::Table table = state.AddTable(_T(""));

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

   Variant value = imageProperty.Value();

   Lua::Value luaValue;
   if (!value.IsArray())
   {
      LuaValueFromVariant(value, luaValue);
   }

   table.AddValue(_T("value"), luaValue);
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlStartViewfinder(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state)
{
   std::shared_ptr<Viewfinder> spViewfinder = spRemoteReleaseControl->StartViewfinder();
   m_spViewfinder = spViewfinder;

   Lua::Table viewfinder = state.AddTable(_T(""));
   InitViewfinderTable(spViewfinder, viewfinder);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(viewfinder));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlNumAvailableShots(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
{
   unsigned int uiNumShots = spRemoteReleaseControl->NumAvailableShots();

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(static_cast<int>(uiNumShots)));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlSendCommand(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 2)
      throw Lua::Exception(_T("invalid number of parameters to RemoteReleaseControl:sendCommand()"), state.GetState(), __FILE__, __LINE__);

   RemoteReleaseControl::T_enCameraCommand enCameraCommand =
      static_cast<RemoteReleaseControl::T_enCameraCommand>(vecParams[1].Get<int>());

   try
   {
      spRemoteReleaseControl->SendCommand(enCameraCommand);
   }
   catch (...)
   {
   }

   return std::vector<Lua::Value>();
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlRelease(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
{
   spRemoteReleaseControl->Release();

   return std::vector<Lua::Value>();
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlStartBulb(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
   Lua::State& state)
{
   std::shared_ptr<BulbReleaseControl> spBulbReleaseControl = spRemoteReleaseControl->StartBulb();

   Lua::Table bulbReleaseControl = state.AddTable(_T(""));
   InitBulbReleaseControlTable(spBulbReleaseControl, bulbReleaseControl);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(bulbReleaseControl));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::RemoteReleaseControlClose(
   std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
{
   spRemoteReleaseControl->Close();

   return std::vector<Lua::Value>();
}

void CanonControlLuaBindings::InitViewfinderTable(std::shared_ptr<Viewfinder> spViewfinder, Lua::Table& viewfinder)
{
   viewfinder.AddFunction("getCapability",
      std::bind(&CanonControlLuaBindings::ViewfinderGetCapability, shared_from_this(), spViewfinder,
         std::placeholders::_1, std::placeholders::_2));

   viewfinder.AddFunction("setOutputType",
      std::bind(&CanonControlLuaBindings::ViewfinderSetOutputType, shared_from_this(), spViewfinder,
         std::placeholders::_1, std::placeholders::_2));

   viewfinder.AddFunction("setAvailImageHandler",
      std::bind(&CanonControlLuaBindings::ViewfinderSetAvailImageHandler, shared_from_this(), spViewfinder,
         std::placeholders::_1, std::placeholders::_2));

   viewfinder.AddFunction("getHistogram",
      std::bind(&CanonControlLuaBindings::ViewfinderGetHistogram, shared_from_this(), spViewfinder,
         std::placeholders::_1, std::placeholders::_2));

   viewfinder.AddFunction("close",
      std::bind(&CanonControlLuaBindings::ViewfinderClose, shared_from_this(), spViewfinder,
         std::placeholders::_1, std::placeholders::_2));
}

std::vector<Lua::Value> CanonControlLuaBindings::ViewfinderGetCapability(std::shared_ptr<Viewfinder> spViewfinder,
   Lua::State& state, const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 1 && vecParams.size() != 2)
      throw Lua::Exception(_T("viewfinder:getCapability() needs capability parameter"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("viewfinder:getCapability() was passed an illegal 'self' value"), state.GetState(), __FILE__, __LINE__);

   Viewfinder::T_enViewfinderCapability enCapability =
      static_cast<Viewfinder::T_enViewfinderCapability>(vecParams[1].Get<int>());

   bool bCapability = spViewfinder->GetCapability(enCapability);

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(bCapability));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::ViewfinderSetOutputType(
   std::shared_ptr<Viewfinder> spViewfinder,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 1 && vecParams.size() != 2)
      throw Lua::Exception(_T("viewfinder:setOutputType() needs output type parameter"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("viewfinder:setOutputType() was passed an illegal 'self' value"), state.GetState(), __FILE__, __LINE__);

   int outputType = vecParams[1].Get<int>();

   spViewfinder->SetOutputType(static_cast<Viewfinder::T_enOutputType>(outputType));

   return std::vector<Lua::Value>();
}

std::vector<Lua::Value> CanonControlLuaBindings::ViewfinderSetAvailImageHandler(
   std::shared_ptr<Viewfinder> spViewfinder,
   Lua::State& state,
   const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 1 && vecParams.size() != 2)
      throw Lua::Exception(_T("viewfinder:setAvailImageHandler() needs callback parameter"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("viewfinder:setAvailImageHandler() was passed an illegal 'self' value"), state.GetState(), __FILE__, __LINE__);

   Lua::Table app = state.GetTable(_T("App"));

   if (vecParams.size() == 1)
   {
      app.AddValue(c_pszSetAvailImageHandler_OnAvailImageHandler, Lua::Value());

      spViewfinder->SetAvailImageHandler(Viewfinder::T_fnOnAvailViewfinderImage());
   }
   else
   {
      app.AddValue(c_pszSetAvailImageHandler_OnAvailImageHandler, vecParams[1]);

      auto fnOnAvailImage = std::bind(
         &CanonControlLuaBindings::SetAvailImageHandler_OnAvailImageHandler,
         shared_from_this(),
         spViewfinder,
         std::placeholders::_1);

      spViewfinder->SetAvailImageHandler(m_strand.wrap(fnOnAvailImage));
   }

   return std::vector<Lua::Value>();
}

void CanonControlLuaBindings::SetAvailImageHandler_OnAvailImageHandler(
   std::shared_ptr<Viewfinder> spViewfinder,
   const std::vector<BYTE>& vecImage)
{
   Lua::Table app = GetState().GetTable(_T("App"));

   Lua::Value value = app.GetValue(c_pszSetAvailImageHandler_OnAvailImageHandler);

   if (value.GetType() == Lua::Value::typeNil)
      return; // Lua value already set to nil, but handler still active

   if (value.GetType() != Lua::Value::typeFunction)
   {
      if (m_fnOutputDebugString != nullptr)
         m_fnOutputDebugString(_T("Runtime error: callback for setAvailImageHandler() is not a function\n"));

      spViewfinder->SetAvailImageHandler(Viewfinder::T_fnOnAvailViewfinderImage());
      return;
   }

   std::vector<Lua::Value> vecParams;

   // first 'self' parameter: viewfinder
   Lua::Table viewfinder = GetState().AddTable(_T(""));
   InitViewfinderTable(spViewfinder, viewfinder);

   vecParams.push_back(Lua::Value(viewfinder));

   // second parameter: image
   size_t uiSize = vecImage.size();
   Lua::Userdata userdata = GetState().AddUserdata(uiSize);
   memcpy(userdata.Data(), vecImage.data(), uiSize);

   vecParams.push_back(Lua::Value(userdata));

   try
   {
      app.CallFunction(c_pszSetAvailImageHandler_OnAvailImageHandler, 0, vecParams);
   }
   catch (const Lua::Exception& ex)
   {
      if (m_fnOutputDebugString != nullptr)
         m_fnOutputDebugString(ex.Message());

      // unregister handler to not receive any more events, which may also end in an error
      spViewfinder->SetAvailImageHandler(Viewfinder::T_fnOnAvailViewfinderImage());
   }
}

std::vector<Lua::Value> CanonControlLuaBindings::ViewfinderGetHistogram(std::shared_ptr<Viewfinder> spViewfinder,
   Lua::State& state, const std::vector<Lua::Value>& vecParams)
{
   if (vecParams.size() != 1 && vecParams.size() != 2)
      throw Lua::Exception(_T("viewfinder:getHistogram() needs histogram type parameter"), state.GetState(), __FILE__, __LINE__);

   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("viewfinder:getHistogram() was passed an illegal 'self' value"), state.GetState(), __FILE__, __LINE__);

   Viewfinder::T_enHistogramType enHistogramType =
      static_cast<Viewfinder::T_enHistogramType>(vecParams[1].Get<int>());

   std::vector<unsigned int> histogram;
   spViewfinder->GetHistogram(enHistogramType, histogram);

   Lua::Table histogramTable = state.AddTable(_T(""));

   size_t maxIndex = histogram.size();
   for (size_t index = 0; index < maxIndex; index++)
   {
      histogramTable.AddValue(
         static_cast<int>(index + 1), // 1-based
         Lua::Value(static_cast<int>(histogram[index])));
   }

   histogramTable.AddValue(_T("length"), Lua::Value(static_cast<int>(maxIndex)));

   std::vector<Lua::Value> vecRetValues;
   vecRetValues.push_back(Lua::Value(histogramTable));

   return vecRetValues;
}

std::vector<Lua::Value> CanonControlLuaBindings::ViewfinderClose(std::shared_ptr<Viewfinder> spViewfinder,
   Lua::State& state, const std::vector<Lua::Value>& vecParams)
{
   if (vecParams[0].GetType() != Lua::Value::typeTable)
      throw Lua::Exception(_T("viewfinder:setAvailImageHandler() was passed an illegal 'self' value"), state.GetState(), __FILE__, __LINE__);

   spViewfinder->Close();

   return std::vector<Lua::Value>();
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
