//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2026 Michael Fink
//
/// \file CameraControlLuaBindings.hpp Lua bindings for the CameraControl library
//
#pragma once

// includes
#include "Lua.hpp"
#include "Instance.hpp"
#include <ulib/thread/RecursiveMutex.hpp>
#include <ulib/thread/Event.hpp>
#include <asio.hpp>
#include "ShutterReleaseSettings.hpp"
#include "RemoteReleaseControl.hpp"

// forward references
class SourceDevice;
class SourceInfo;
class DeviceProperty;
class ImageProperty;
class Viewfinder;
class BulbReleaseControl;

/// \brief Lua bindings for CameraControl library
/// \details Provides bindings for all classes and functions in the CameraControl
/// library. As soon as the object is destroyed, the bindings are deregistered. All
/// callback handlers registered are reset and won't be called anymore.
class CameraControlLuaBindings : public std::enable_shared_from_this<CameraControlLuaBindings>
{
public:
   /// function type to output debug strings
   typedef std::function<void(const CString&)> T_fnOutputDebugString;

   /// ctor; inits bindings
   CameraControlLuaBindings(Lua::State& state, asio::io_service::strand& strand);

   /// dtor; cleans up bindings
   virtual ~CameraControlLuaBindings();

   /// sets output debug string handler
   void SetOutputDebugStringHandler(T_fnOutputDebugString fnOutputDebugString)
   {
      m_fnOutputDebugString = fnOutputDebugString;
   }

   /// inits bindings to CameraControl; since the this parameter is needed in
   /// the bindings, call this immediately after the ctor
   void InitBindings();

   /// cancels all handlers of async operations
   void CancelHandlers();

   /// stops internal timer
   void StopTimer();

private:
   /// returns Lua state object
   Lua::State& GetState() { return m_state; }

   /// inits constants used in various calls
   void InitConstants();

   /// inits constants for SourceDevice table
   void InitSourceDeviceConstants(Lua::Table& constants);

   /// inits constants for ImageProperty table
   void InitImagePropertyConstants(Lua::Table& constants);

   /// inits constants for ShootingMode table
   void InitShootingModeConstants(Lua::Table& constants);

   /// inits constants for ShutterReleaseSettings table
   void InitShutterReleaseSettingsConstants(Lua::Table& constants);

   /// inits constants for RemoteReleaseControl
   void InitRemoteReleaseControlConstants(Lua::Table& constants);

   /// inits constants for Viewfinder
   void InitViewfinderConstants(Lua::Table& constants);

   /// converts Variant value to Lua::Value
   static void LuaValueFromVariant(const Variant& value, Lua::Value& luaValue);

   /// modifies an existing variant, keeping the type, and replacing the value with a Lua value
   static bool ModifyVariantFromLuaValue(const Lua::Value& luaValue, Variant& value);

   /// restarts timer for event handling
   void RestartEventTimer();

   /// cleans up all bindings
   void CleanupBindings();

   /// handler for timer used for event handling
   void OnTimerEventHandling(const std::error_code& error);


   // Sys functions

   /// local instance = Sys:getInstance()
   std::vector<Lua::Value> SysGetInstance(Lua::State& state);


   // Instance functions

   /// local version = Instance:getVersion()
   std::vector<Lua::Value> InstanceGetVersion();

   /// local sourceInfoArray = instance:enumerateDevices()
   std::vector<Lua::Value> InstanceEnumerateDevices(Lua::State& state);

   /// instance:asyncWaitForCamera(callbackFunction)
   std::vector<Lua::Value> InstanceAsyncWaitForCamera(Lua::State& state,
      const std::vector<Lua::Value>& vecParams);

   /// method called when a camera has been connected
   void AsyncWaitForCamera_OnCameraConnected();


   // SourceInfo functions

   /// adds a source info table to given table
   /// { name = "camera name", function open() ... end }
   void AddSourceInfo(Lua::State& state, Lua::Table& table, size_t uiIndex, std::shared_ptr<SourceInfo> spSourceInfo);

   /// local sourceDevice = sourceInfo:open()
   std::vector<Lua::Value> SourceInfoOpen(std::shared_ptr<SourceInfo> spSourceInfo,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);


   // SourceDevice functions

   /// initializes SourceDevice table
   void InitSourceDeviceTable(std::shared_ptr<SourceDevice> spSourceDevice, Lua::Table& sourceDevice);

   /// local isCapable = sourceDevice:getDeviceCapability(Constants.SourceDevice.capXxx)
   std::vector<Lua::Value> SourceDeviceGetDeviceCapability(std::shared_ptr<SourceDevice> spSourceDevice,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// local arrayDeviceProps = sourceDevice:enumDeviceProperties()
   std::vector<Lua::Value> SourceDeviceEnumDeviceProperties(std::shared_ptr<SourceDevice> spSourceDevice,
      Lua::State& state);

   /// local deviceProperty = sourceDevice:getDeviceProperty()
   std::vector<Lua::Value> SourceDeviceGetDeviceProperty(std::shared_ptr<SourceDevice> spSourceDevice,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// adds a device property values to given table
   /// { id = "property id", name = "name", asString = "value", isReadOnly = true/false end }
   void AddDeviceProperty(Lua::State& state,
      Lua::Table& table,
      const DeviceProperty& deviceProperty);

   /// adds list of valid values for a device property
   void AddDevicePropertyValidValues(
      Lua::State& state,
      Lua::Table& table,
      const DeviceProperty& deviceProperty);

   /// enters release control and returns RemoteReleaseControl table
   std::vector<Lua::Value> SourceDeviceEnterReleaseControl(std::shared_ptr<SourceDevice> spSourceDevice,
      Lua::State& state);


   // RemoteReleaseControl functions

   /// initializes RemoteReleaseControl table
   void InitRemoteReleaseControlTable(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, Lua::Table& remoteReleaseControl);

   /// local isCapable = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capXxx)
   std::vector<Lua::Value> RemoteReleaseControlGetCapability(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// local releaseSettings = remoteReleaseControl:getReleaseSettings()
   std::vector<Lua::Value> RemoteReleaseControlGetReleaseSettings(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// initializes ReleaseSettings table
   void InitReleaseSettingsTable(Lua::State& state, const ShutterReleaseSettings& releaseSettings,
      Lua::Table& tableReleaseSettings);

   /// sets new release settings
   std::vector<Lua::Value> RemoteReleaseControlSetReleaseSettings(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// callback that is called when image has been transferred
   void SetReleaseSettings_OnFinishedTransfer(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      const ShutterReleaseSettings& releaseSettings);

   /// event handler for property change events
   void RemoteReleaseControl_PropertyEventHandler(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      std::shared_ptr<int> spHandlerId,
      RemoteReleaseControl::T_enPropertyEvent,
      unsigned int eventData);

   /// local handlerId = remoteReleaseControl:addPropertyEventHandler(callbackFunction)
   std::vector<Lua::Value> RemoteReleaseControlAddPropertyEventHandler(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state,
      const std::vector<Lua::Value>& vecParams);

   /// remoteReleaseControl:removePropertyEventHandler(handlerId)
   std::vector<Lua::Value> RemoteReleaseControlRemovePropertyEventHandler(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state,
      const std::vector<Lua::Value>& vecParams);

   /// event handler for state events
   void RemoteReleaseControl_StateEventHandler(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      std::shared_ptr<int> spHandlerId,
      RemoteReleaseControl::T_enStateEvent,
      unsigned int eventData);

   /// local handlerId = remoteReleaseControl:addStateEventHandler(callbackFunction)
   std::vector<Lua::Value> RemoteReleaseControlAddStateEventHandler(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state,
      const std::vector<Lua::Value>& vecParams);

   /// remoteReleaseControl:removeStateEventHandler(handlerId)
   std::vector<Lua::Value> RemoteReleaseControlRemoveStateEventHandler(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state,
      const std::vector<Lua::Value>& vecParams);

   /// event handler for download events
   void RemoteReleaseControl_DownloadEventHandler(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      std::shared_ptr<int> spHandlerId,
      RemoteReleaseControl::T_enDownloadEvent,
      unsigned int eventData);

   /// local handlerId = remoteReleaseControl:addDownloadEventHandler(callbackFunction)
   std::vector<Lua::Value> RemoteReleaseControlAddDownloadEventHandler(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state,
      const std::vector<Lua::Value>& vecParams);

   /// remoteReleaseControl:removeDownloadEventHandler(handlerId)
   std::vector<Lua::Value> RemoteReleaseControlRemoveDownloadEventHandler(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state,
      const std::vector<Lua::Value>& vecParams);

   /// local imagePropertiesList = remoteReleaseControl:enumImageProperties()
   std::vector<Lua::Value> RemoteReleaseControlEnumImageProperties(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, Lua::State& state);

   /// local imageProperty = remoteReleaseControl:getImageProperty()
   std::vector<Lua::Value> RemoteReleaseControlGetImageProperty(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// local imageProperty = remoteReleaseControl:getImagePropertyByType(imagePropertyType)
   std::vector<Lua::Value> RemoteReleaseControlGetImagePropertyByType(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state,
      const std::vector<Lua::Value>& vecParams);

   /// local imageProperty = remoteReleaseControl:getShootingModeImageProperty(shootingMode)
   std::vector<Lua::Value> RemoteReleaseControlGetShootingModeImageProperty(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state,
      const std::vector<Lua::Value>& vecParams);

   /// remoteReleaseControl:setImageProperty(imageProperty)
   std::vector<Lua::Value> RemoteReleaseControlSetImageProperty(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// adds ImageProperty table
   void AddImageProperty(Lua::State& state,
      Lua::Table& table,
      const ImageProperty& imageProperty,
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   /// adds list of valid values for an image property
   void AddImagePropertyValidValues(
      Lua::State& state,
      Lua::Table& table,
      const ImageProperty& imageProperty,
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   /// local viewfinder = remoteReleaseControl:startViewfinder()
   std::vector<Lua::Value> RemoteReleaseControlStartViewfinder(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, Lua::State& state);

   /// local numShots = remoteReleaseControl:numAvailableShots()
   std::vector<Lua::Value> RemoteReleaseControlNumAvailableShots(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   /// remoteReleaseControl:sendCommand()
   std::vector<Lua::Value> RemoteReleaseControlSendCommand(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// remoteReleaseControl:release()
   std::vector<Lua::Value> RemoteReleaseControlRelease(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   /// local bulbReleaseControl = remoteReleaseControl:startBulb()
   std::vector<Lua::Value> RemoteReleaseControlStartBulb(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, Lua::State& state);

   /// local remoteReleaseControl:close()
   std::vector<Lua::Value> RemoteReleaseControlClose(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);


   // Viewfinder functions

   /// initializes viewfinder table
   void InitViewfinderTable(std::shared_ptr<Viewfinder> spViewfinder, Lua::Table& viewfinder);

   /// local isCapable = viewfinder:getCapability(Constants.Viewfinder.capXxx)
   std::vector<Lua::Value> ViewfinderGetCapability(std::shared_ptr<Viewfinder> spViewfinder,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// viewfinder:setOutputType(outputType)
   std::vector<Lua::Value> ViewfinderSetOutputType(std::shared_ptr<Viewfinder> spViewfinder,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// viewfinder:setAvailImageHandler(callbackFunction)
   std::vector<Lua::Value> ViewfinderSetAvailImageHandler(std::shared_ptr<Viewfinder> spViewfinder,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// called when a new viewfinder image is available
   void SetAvailImageHandler_OnAvailImageHandler(std::shared_ptr<Viewfinder> spViewfinder,
      const std::vector<BYTE>& vecImage);

   /// local histogram viewfinder:getHistogram(Constants.Viewfinder.histogramXxx);
   std::vector<Lua::Value> ViewfinderGetHistogram(std::shared_ptr<Viewfinder> spViewfinder,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// called to close viewfinder
   std::vector<Lua::Value> ViewfinderClose(std::shared_ptr<Viewfinder> spViewfinder,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);


   // BulbReleaseControl functions

   /// initizalizes BulbReleaseControl table
   void InitBulbReleaseControlTable(std::shared_ptr<BulbReleaseControl> spBulbReleaseControl, Lua::Table& bulbReleaseControl);

   /// local elapsedTime = bulbReleaseControl:elapsedTime()
   std::vector<Lua::Value> BulbReleaseControlElapsedTime(std::shared_ptr<BulbReleaseControl> spBulbReleaseControl);

   /// bulbReleaseControl:stop()
   std::vector<Lua::Value> BulbReleaseControlStop(std::shared_ptr<BulbReleaseControl> spBulbReleaseControl);

private:
   /// Lua state
   Lua::State& m_state;

   /// CameraControl instance
   std::unique_ptr<Instance> m_upInstance;

   /// release settings stored for the script
   ShutterReleaseSettings m_releaseSettings;

   /// once Lua has connected to remote release control, a pointer is stored here
   std::shared_ptr<RemoteReleaseControl> m_spRemoteRelaseControl;

   /// when Lua opened buld release control, a pointer is stored here
   std::shared_ptr<BulbReleaseControl> m_spBulbReleaseControl;

   /// set of all registered property handler ids
   std::set<int> m_setAllPropertyHandlerIds;

   /// set of all registered state handler ids
   std::set<int> m_setAllStateHandlerIds;

   /// set of all registered download handler ids
   std::set<int> m_setAllDownloadHandlerIds;

   /// once Lua script started viewfinder, a pointer is stored here
   std::shared_ptr<Viewfinder> m_spViewfinder;

   /// strand to execute all Lua calls on
   asio::io_service::strand& m_strand;

   /// output debug string handler
   T_fnOutputDebugString m_fnOutputDebugString;

   /// mutex to protect AsyncWaitForCamera() handler to call Lua script multiple times
   RecursiveMutex m_mtxAsyncWaitForCamera_InScript;

   /// timer for event handling
   asio::system_timer m_timerEventHandling;

   /// event that is set when the timer should stop
   ManualResetEvent m_evtStopTimer;

   /// event that is set when the event handling timer has stopped
   ManualResetEvent m_evtTimerStopped;
};
