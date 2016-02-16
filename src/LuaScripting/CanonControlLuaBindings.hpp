//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file CanonControlLuaBindings.hpp Lua bindings for the CanonControl library
//
#pragma once

// includes
#include "Lua.hpp"
#include "Instance.hpp"
#include "RecursiveMutex.hpp"
#include "Asio.hpp"
#include "ShutterReleaseSettings.hpp"

// forward references
class SourceDevice;
class SourceInfo;
class DeviceProperty;
class RemoteReleaseControl;
class ImageProperty;
class Viewfinder;
class BulbReleaseControl;

/// \brief Lua bindings for CanonControl library
/// \details Provides bindings for all classes and functions in the CanonControl
/// library. As soon as the object is destroyed, the bindings are deregistered. All
/// callback handlers registered are reset and won't be called anymore.
class CanonControlLuaBindings : public std::enable_shared_from_this<CanonControlLuaBindings>
{
public:
   /// function type to output debug strings
   typedef std::function<void(const CString&)> T_fnOutputDebugString;

   /// ctor; inits bindings
   CanonControlLuaBindings(Lua::State& state, boost::asio::io_service::strand& strand);

   /// dtor; cleans up bindings
   virtual ~CanonControlLuaBindings() throw();

   /// sets output debug string handler
   void SetOutputDebugStringHandler(T_fnOutputDebugString fnOutputDebugString)
   {
      m_fnOutputDebugString = fnOutputDebugString;
   }

   /// inits bindings to CanonControl; since the this parameter is needed in
   /// the bindings, call this immediately after the ctor
   void InitBindings();

   /// cancels all handlers of async operations
   void CancelHandlers();

private:
   /// returns Lua state object
   Lua::State& GetState() throw() { return m_state; }

   /// inits constants used in various calls
   void InitConstants();

   /// inits constants for SourceDevice table
   void InitSourceDeviceConstants(Lua::Table& constants);

   /// inits constants for ShutterReleaseSettings table
   void InitShutterReleaseSettingsConstants(Lua::Table& constants);

   /// inits constants for RemoteReleaseControl
   void InitRemoteReleaseControlConstants(Lua::Table& constants);

   /// cleans up all bindings
   void CleanupBindings();

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
   void AddDeviceProperty(Lua::Table& table, const DeviceProperty& deviceProperty, std::shared_ptr<SourceDevice> spSourceDevice);

   /// enters release control and returns RemoteReleaseControl table
   std::vector<Lua::Value> SourceDeviceEnterReleaseControl(std::shared_ptr<SourceDevice> spSourceDevice,
      Lua::State& state);


   // RemoteReleaseControl functions

   /// initializes RemoteReleaseControl table
   void InitRemoteReleaseControlTable(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, Lua::Table& remoteReleaseControl);

   /// local capability = remoteReleaseControl:getCapability()
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

   /// local imagePropertiesList = remoteReleaseControl:enumImageProperties()
   std::vector<Lua::Value> RemoteReleaseControlEnumImageProperties(
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, Lua::State& state);

   /// local imageProperty = remoteReleaseControl:getImageProperty()
   std::vector<Lua::Value> RemoteReleaseControlGetImageProperty(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// adds ImageProperty table
   void AddImageProperty(Lua::Table& table, const ImageProperty& imageProperty,
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

   // Viewfinder functions

   /// initializes viewfinder table
   void InitViewfinderTable(std::shared_ptr<Viewfinder> spViewfinder, Lua::Table& viewfinder);

   /// viewfinder:setAvailImageHandler(callbackFunction)
   std::vector<Lua::Value> ViewfinderSetAvailImageHandler(std::shared_ptr<Viewfinder> spViewfinder,
      Lua::State& state, const std::vector<Lua::Value>& vecParams);

   /// called when a new viewfinder image is available
   void SetAvailImageHandler_OnAvailImageHandler(std::shared_ptr<Viewfinder> spViewfinder,
      const std::vector<BYTE>& vecImage);

   // Bulb functions

   /// initizalizes BulbReleaseControl table
   void InitBulbReleaseControlTable(std::shared_ptr<BulbReleaseControl> spBulbReleaseControl, Lua::Table& bulbReleaseControl);

   /// local elapsedTime = bulbReleaseControl:elapsedTime()
   std::vector<Lua::Value> BulbReleaseControlElapsedTime(std::shared_ptr<BulbReleaseControl> spBulbReleaseControl);

   /// bulbReleaseControl:stop()
   std::vector<Lua::Value> BulbReleaseControlStop(std::shared_ptr<BulbReleaseControl> spBulbReleaseControl);

private:
   /// Lua state
   Lua::State& m_state;

   /// CanonControl instance
   std::unique_ptr<Instance> m_upInstance;

   /// release settings stored for the script
   ShutterReleaseSettings m_releaseSettings;

   /// once Lua has connected to remote release control, a pointer is stored here
   std::shared_ptr<RemoteReleaseControl> m_spRemoteRelaseControl;

   /// once Lua script started viewfinder, a pointer is stored here
   std::shared_ptr<Viewfinder> m_spViewfinder;

   /// strand to execute all Lua calls on
   boost::asio::io_service::strand& m_strand;

   /// output debug string handler
   T_fnOutputDebugString m_fnOutputDebugString;

   /// mutex to protect AsyncWaitForCamera() handler to call Lua script multiple times
   RecursiveMutex m_mtxAsyncWaitForCamera_InScript;
};
