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

// forward references
class SourceDevice;
class SourceInfo;
class DeviceProperty;
class RemoteReleaseControl;
class ImageProperty;
class Viewfinder;
class BulbReleaseControl;

/// Lua bindings for CanonControl library
class CanonControlLuaBindings : public std::enable_shared_from_this<CanonControlLuaBindings>
{
public:
   /// function type to output debug strings
   typedef std::function<void(const CString&)> T_fnOutputDebugString;

   /// ctor; inits bindings
   CanonControlLuaBindings(Lua::State& state, boost::asio::io_service::strand& strand);

   /// dtor; cleans up bindings
   virtual ~CanonControlLuaBindings();

   /// sets output debug string handler
   void SetOutputDebugStringHandler(T_fnOutputDebugString fnOutputDebugString)
   {
      m_fnOutputDebugString = fnOutputDebugString;
   }

   /// inits bindings to CanonControl
   void InitBindings();

   /// cancels all handlers of async operations
   void CancelHandlers();

private:
   /// returns Lua state object
   Lua::State& GetState() throw() { return m_state; }

   /// inits constants used in various calls
   void InitConstants();

   void InitSourceDeviceConstants(Lua::Table& constants);

   void InitShutterReleaseSettingsConstants(Lua::Table& constants);

   void InitRemoteReleaseControlConstants(Lua::Table& constants);

   void CleanupBindings();

   // Sys functions

   /// local instance = Sys:getInstance()
   /// instance:getVersion()
   /// instance:enumerateDevices()
   /// instance:asyncWaitForCamera(callbackFunction)
   std::vector<Lua::Value> SysGetInstance();

   // Instance functions

   /// local version = Instance:getVersion()
   std::vector<Lua::Value> InstanceGetVersion();

   /// local sourceInfoArray = instance:enumerateDevices()
   std::vector<Lua::Value> InstanceEnumerateDevices();

   /// instance:asyncWaitForCamera(callbackFunction)
   std::vector<Lua::Value> InstanceAsyncWaitForCamera(const std::vector<Lua::Value>& vecParams);

   void AsyncWaitForCamera_OnCameraConnected();

   // SourceInfo functions

   /// adds a source info table to given table
   /// { name = "camera name", function open() ... end }
   void AddSourceInfo(Lua::Table& table, size_t uiIndex, std::shared_ptr<SourceInfo> spSourceInfo);

   /// local sourceDevice = sourceInfo:open()
   std::vector<Lua::Value> SourceInfoOpen(std::shared_ptr<SourceInfo> spSourceInfo,
      const std::vector<Lua::Value>& vecParams);

   // SourceDevice functions

   void InitSourceDeviceTable(std::shared_ptr<SourceDevice> spSourceDevice, Lua::Table& sourceDevice);

   /// local isCapable = sourceDevice:getDeviceCapability(Constants.SourceDevice.capXxx)
   std::vector<Lua::Value> SourceDeviceGetDeviceCapability(std::shared_ptr<SourceDevice> spSourceDevice,
      const std::vector<Lua::Value>& vecParams);

   /// local arrayDeviceProps = sourceDevice:enumDeviceProperties()
   std::vector<Lua::Value> SourceDeviceEnumDeviceProperties(std::shared_ptr<SourceDevice> spSourceDevice);

   std::vector<Lua::Value> SourceDeviceGetDeviceProperty(std::shared_ptr<SourceDevice> spSourceDevice,
      const std::vector<Lua::Value>& vecParams);

   /// adds a device property values to given table
   /// { id = "property id", name = "name", asString = "value", isReadOnly = true/false end }
   void AddDeviceProperty(Lua::Table& table, const DeviceProperty& deviceProperty, std::shared_ptr<SourceDevice> spSourceDevice);

   std::vector<Lua::Value> SourceDeviceEnterReleaseControl(std::shared_ptr<SourceDevice> spSourceDevice);


   // RemoteReleaseControl functions

   void InitRemoteReleaseControlTable(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl, Lua::Table& remoteReleaseControl);

   std::vector<Lua::Value> RemoteReleaseControlGetCapability(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      const std::vector<Lua::Value>& vecParams);

   std::vector<Lua::Value> RemoteReleaseControlSetReleaseSettings(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      const std::vector<Lua::Value>& vecParams);

   std::vector<Lua::Value> RemoteReleaseControlEnumImageProperties(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   std::vector<Lua::Value> RemoteReleaseControlGetImageProperty(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      const std::vector<Lua::Value>& vecParams);

   void AddImageProperty(Lua::Table& table, const ImageProperty& imageProperty,
      std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   std::vector<Lua::Value> RemoteReleaseControlStartViewfinder(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   std::vector<Lua::Value> RemoteReleaseControlNumAvailableShots(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   std::vector<Lua::Value> RemoteReleaseControlSendCommand(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl,
      const std::vector<Lua::Value>& vecParams);

   std::vector<Lua::Value> RemoteReleaseControlRelease(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   std::vector<Lua::Value> RemoteReleaseControlStartBulb(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl);

   // Viewfinder functions

   void InitViewfinderTable(std::shared_ptr<Viewfinder> spViewfinder, Lua::Table& viewfinder);

   /// viewfinder:setAvailImageHandler(callbackFunction)
   std::vector<Lua::Value> ViewfinderSetAvailImageHandler(std::shared_ptr<Viewfinder> spViewfinder,
      const std::vector<Lua::Value>& vecParams);

   void SetAvailImageHandler_OnAvailImageHandler(const std::vector<BYTE>& /*vecImage*/);

   // Bulb functions

   void InitBulbReleaseControlTable(std::shared_ptr<BulbReleaseControl> spBulbReleaseControl, Lua::Table& bulbReleaseControl);

   std::vector<Lua::Value> BulbReleaseControlElapsedTime(std::shared_ptr<BulbReleaseControl> spBulbReleaseControl);

   std::vector<Lua::Value> BulbReleaseControlStop(std::shared_ptr<BulbReleaseControl> spBulbReleaseControl);

private:
   /// Lua state
   Lua::State& m_state;

   /// CanonControl instance
   std::unique_ptr<Instance> m_upInstance;

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
