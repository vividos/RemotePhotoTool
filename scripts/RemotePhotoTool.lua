--
-- RemotePhotoTool - remote camera control software
-- Copyright (C) 2008-2016 Michael Fink
-- file RemotePhotoTool.lua - Demo script for Lua scripting
--

-- The App object is a global object, and its run() function is called by the Lua
-- scripting engine when starting the script.
App = {

	-- event that is used to wait for connecting camera
	eventCameraConnect = nil;

	-- event that is set when transferring image has finished
	eventFinishedTransfer = nil;

	-- event that is used to get notified that a viewfinder preview image arrived
	eventViewfinder = nil;

	-- Main entry point of the RemotePhotoTool scripting environment.
	-- Be sure to register at least callback here before returning, or
	-- the script execution stops.
	run = function(self)

		print("RemotePhotoTool.lua - Demo script for Lua scripting\n\n");

		print("on main thread: " .. (Sys:isMainThread() and "yes" or "no") .. "\n");

		local instance = Sys:getInstance();

		print(instance:getVersion());

		self.eventCameraConnect = Sys:createEvent();

		instance:asyncWaitForCamera(App.onConnected);

		print("Waiting for camera to connect...\n");

		-- only waits for some seconds, then exists
		local ret = self.eventCameraConnect:wait(15.0);
		if (ret == false) then
			print("No camera connected for 15.0 seconds; exiting.\n");
		end;
	end;

	-- This function is called when a camera has been connected or
	-- disconnected; the next step would be to check if some devices
	-- can be enumerated.
	onConnected = function(self)

		print("List of connected cameras changed\n\n");

		print("on main thread: " .. (Sys:isMainThread() and "yes" or "no") .. "\n");

		local instance = Sys:getInstance();

		local allSourceInfos = instance:enumerateDevices();

		-- The enumerateDevices() function returns nil when no device is
		-- connected, so check before using
		if (allSourceInfos ~= nil and allSourceInfos.length > 0) then

			-- be sure to unregister the callback to not get
			-- any more calls to onConnected().
			instance:asyncWaitForCamera();

			print("\n");

			self:printSourceInfos(allSourceInfos);

			print("Finished.\n\n");

			-- signal finished handler
			self.eventCameraConnect:signal();

		else
			-- There was no device; maybe an already connected camera was
			-- turned off. Wait until another call to onConnected() is made.
			print("No cameras connected. Continue waiting...\n");
		end;
	end;

	-- opens the camera to take images; can be used to directly open release control
	-- of the camera; not used in this script.
	openCamera = function(self, sourceInfo)

		print("Opening camera " .. sourceInfo.name .. "\n");

		local sourceDevice = sourceInfo:open();

		local capRemoteReleaseControl = sourceDevice:getDeviceCapability(Constants.SourceDevice.capRemoteReleaseControl);

		if (not capRemoteReleaseControl) then
			print("Camera can't be remote controlled!");
			return nil;
		end;

		local remoteReleaseControl = sourceDevice:enterReleaseControl();

		return remoteReleaseControl;
	end;

	-- We call this function at onConnected() to print all infos about
	-- connected cameras, stored in SourceInfo objects.
	printSourceInfos = function(self, allSourceInfos)

		print("Number of cameras: " .. allSourceInfos.length .. "\n");

		-- Enumerate all source info objects
		for idx = 1, allSourceInfos.length, 1 do

			local sourceInfo = allSourceInfos[idx];

			print("Camera #" .. idx .. ": " .. sourceInfo.name .. "\n");

			-- Source infos are unconnected cameras, so open it and print
			-- some more infos.
			local sourceDevice = sourceInfo:open();
			self:printSourceDevice(sourceDevice);

			print("\n");
		end;
	end;

	-- We call this function in printSourceInfos() where we opened a source info
	-- object. Now print some more infos about the camera.
	printSourceDevice = function(self, sourceDevice)

		print("   model name: " .. sourceDevice.modelName .. "\n");
		print("   serial no: " .. sourceDevice.serialNumber .. "\n");

		-- Print all device capabilities
		local capRemoteReleaseControl = sourceDevice:getDeviceCapability(Constants.SourceDevice.capRemoteReleaseControl);
		print("   can remote release shutter: " .. (capRemoteReleaseControl and "yes" or "no") .. "\n");

		local capRemoteViewfinder = sourceDevice:getDeviceCapability(Constants.SourceDevice.capRemoteViewfinder);
		print("   can show viewfinder: " .. (capRemoteViewfinder and "yes" or "no") .. "\n");
		print("\n");

		-- Enumerate all device properties
		print("Enumerating all device properties:\n");

		local devicePropertyList = sourceDevice:enumDeviceProperties();

		if (devicePropertyList and devicePropertyList.length > 0) then

			print("   number of device properties: " .. devicePropertyList.length .. "\n");

			for idx = 1, devicePropertyList.length, 1 do

				local devicePropertyId = devicePropertyList[idx];

				local prop = sourceDevice:getDeviceProperty(devicePropertyId);

				print("   property: id=" .. prop.id ..
					", name=\"" .. prop.name ..
					"\", value=\"" .. prop.asString ..
					"\", readonly=" .. (prop.isReadOnly and "yes" or "no") .. "\n");
			end;
		end;
		print("\n");

		self:printRemoteControlInfos(sourceDevice);

	end;

	-- We call this function in printSourceDevice() to dump even more infos about
	-- the camera by entering remote release control mode.
	printRemoteControlInfos = function(self, sourceDevice)

		local capRemoteReleaseControl = sourceDevice:getDeviceCapability(Constants.SourceDevice.capRemoteReleaseControl);
		if (not capRemoteReleaseControl) then
			print("Remote release control not supported by this camera.");
			return;
		end;

		print("Remote release control infos:\n");

		local remoteReleaseControl = sourceDevice:enterReleaseControl();

		local numAvailableShots = remoteReleaseControl.numAvailableShots();
		print("   number of available shots: " .. numAvailableShots .. "\n");
		print("\n");

		self:printReleaseControlCapabilities(remoteReleaseControl);

		self:printImageProperties(remoteReleaseControl);

		self:printCurrentShootingSettings(remoteReleaseControl);
		self:takeImage(remoteReleaseControl);
		-- self:listenEvents(remoteReleaseControl);

		self:checkViewfinder(remoteReleaseControl);

		self:checkBulbMode(remoteReleaseControl);

		remoteReleaseControl:close();

	end;

	-- prints all release control capabilities
	printReleaseControlCapabilities = function(self, remoteReleaseControl)

		print("Release control capabilities:\n");
		local capChangeShootingParameter = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capChangeShootingParameter);
		print("   can change shooting parameter: " .. (capChangeShootingParameter and "yes" or "no") .. "\n");

		local capChangeShootingMode = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capChangeShootingMode);
		print("   can change shooting mode: " .. (capChangeShootingMode and "yes" or "no") .. "\n");

		local capZoomControl = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capZoomControl);
		print("   can use zoom control: " .. (capZoomControl and "yes" or "no") .. "\n");

		local capViewfinder = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capViewfinder);
		print("   can use viewfinder: " .. (capViewfinder and "yes" or "no") .. "\n");

		local capReleaseWhileViewfinder = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capReleaseWhileViewfinder);
		print("   can release shutter while viewfinder active: " .. (capReleaseWhileViewfinder and "yes" or "no") .. "\n");

		local capAFLock = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capAFLock);
		print("   can lock auto focus: " .. (capAFLock and "yes" or "no") .. "\n");

		local capBulbMode = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capBulbMode);
		print("   can use bulb mode: " .. (capBulbMode and "yes" or "no") .. "\n");

		local capUILock = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capUILock);
		print("   can lock camera UI: " .. (capUILock and "yes" or "no") .. "\n");
		print("\n");

	end;

	-- prints all image properties
	printImageProperties = function(self, remoteReleaseControl)

		print("Enumerating all image properties:\n");

		local imagePropertyList = remoteReleaseControl:enumImageProperties();

		if (imagePropertyList and imagePropertyList.length > 0) then

			print("   number of image properties: " .. imagePropertyList.length .. "\n");

			for idx = 1, imagePropertyList.length, 1 do

				local imagePropertyId = imagePropertyList[idx];

				local prop = remoteReleaseControl:getImageProperty(imagePropertyId);

				print("   property: id=" .. prop.id ..
					", name=\"" .. prop.name ..
					"\", value=\"" .. prop.asString ..
					"\", readonly=" .. (prop.isReadOnly and "yes" or "no") .. "\n");
			end;
		end;
		print("\n");

	end;

	-- prints current shooting settings
	printCurrentShootingSettings = function(self, remoteReleaseControl)

		local shootingMode = remoteReleaseControl:getImagePropertyByType(Constants.ImageProperty.shootingMode);
		local aperture = remoteReleaseControl:getImagePropertyByType(Constants.ImageProperty.Av);
		local shutterSpeed = remoteReleaseControl:getImagePropertyByType(Constants.ImageProperty.Tv);
		local isoSpeed = remoteReleaseControl:getImagePropertyByType(Constants.ImageProperty.ISOSpeed);

		print(
			"Mode: " .. shootingMode.asString .. ", " ..
			"Aperture: " .. aperture.asString .. ", " ..
			"Shutter speed: " .. shutterSpeed.asString .. ", " ..
			"ISO: " .. isoSpeed.asString .. "\n");

	end;

	-- takes an image
	takeImage = function(self, remoteReleaseControl)

		local downloadHandlerId =
			remoteReleaseControl:addDownloadEventHandler(self.onDownloadEvent);

		if (remoteReleaseControl ~= nil) then
			self:releaseShutter(remoteReleaseControl);
		end;

		remoteReleaseControl:removeDownloadEventHandler(downloadHandlerId);

	end;

	-- listens to events for a time, then returns
	listenEvents = function(self, remoteReleaseControl)

		local downloadHandlerId =
			remoteReleaseControl:addDownloadEventHandler(self.onDownloadEvent);

		local propertyHandlerId =
			remoteReleaseControl:addPropertyEventHandler(self.onPropertyEvent);

		local stateHandlerId =
			remoteReleaseControl:addStateEventHandler(self.onStateEvent);

		self.remoteReleaseControl = remoteReleaseControl;

		local waitEvent = Sys:createEvent();
		waitEvent:wait(30.0);

		print("Finished.");

		self.remoteReleaseControl = nil;

		remoteReleaseControl:removeStateEventHandler(stateHandlerId);
		remoteReleaseControl:removePropertyEventHandler(propertyHandlerId);
		remoteReleaseControl:removeDownloadEventHandler(downloadHandlerId);

	end;

	-- called when a download event occurs
	onDownloadEvent = function(self, downloadEventType, eventParam)

		local downloadEventName =
			downloadEventType == Constants.RemoteReleaseControl.downloadEventStarted and "downloadEventStarted" or
			downloadEventType == Constants.RemoteReleaseControl.downloadEventInProgress and "downloadEventStarted" or
			downloadEventType == Constants.RemoteReleaseControl.downloadEventFinished and "downloadEventStarted" or "???";

		print("onDownloadEvent! type=" .. downloadEventName ..
			" eventParam=" .. eventParam .. "\n");

	end;

	-- called when a property event occurs
	onPropertyEvent = function(self, propertyEventType, eventParam)

		local propertyEventName =
			propertyEventType == Constants.RemoteReleaseControl.propEventPropertyChanged and "propEventPropertyChanged" or
			propertyEventType == Constants.RemoteReleaseControl.propEventPropertyDescChanged and "propEventPropertyDescChanged" or "???";

		print("onPropertyEvent! type=" .. propertyEventName ..
			" eventParam=" .. eventParam .. "\n");

		local prop = self.remoteReleaseControl:getImageProperty(eventParam);

		print("   property: id=" .. prop.id ..
			", name=\"" .. prop.name ..
			"\", value=\"" .. prop.asString ..
			"\", readonly=" .. (prop.isReadOnly and "yes" or "no") .. "\n");

	end;

	-- called when a state event occurs
	onStateEvent = function(self, stateEventType, eventParam)

		local stateEventName =
			stateEventType == Constants.RemoteReleaseControl.stateEventCameraShutdown and "stateEventCameraShutdown" or
			stateEventType == Constants.RemoteReleaseControl.stateEventRotationAngle and "stateEventRotationAngle" or
			stateEventType == Constants.RemoteReleaseControl.stateEventMemoryCardSlotOpen and "stateEventMemoryCardSlotOpen" or
			stateEventType == Constants.RemoteReleaseControl.stateEventReleaseError and "stateEventReleaseError" or
			stateEventType == Constants.RemoteReleaseControl.stateEventBulbExposureTime and "stateEventBulbExposureTime" or
			stateEventType == Constants.RemoteReleaseControl.stateEventInternalError and "stateEventInternalError" or "???";

		print("onStateEvent! type=" .. stateEventName ..
			" eventParam=" .. eventParam .. "\n");

	end;

	-- releases shutter and takes a picture
	releaseShutter = function(self, remoteReleaseControl)

		print("Taking an image\n");

		self.setReleaseSettings(remoteReleaseControl);

		self.eventFinishedTransfer = Sys:createEvent();

		-- auto focus on current view
		print("Auto-focus...\n");
		remoteReleaseControl:sendCommand(Constants.RemoteReleaseControl.commandAdjustFocus);

		print("Release shutter...\n");
		remoteReleaseControl:release();

		-- wait for photo to be downloaded
		local result = self.eventFinishedTransfer:wait(10.0);

		print(result and "Finished.\n" or "Failed waiting for image.\n");

		self.resetTransferHandler(remoteReleaseControl);

	end;

	-- sets new release settings
	setReleaseSettings = function(remoteReleaseControl)

		local releaseSettings = remoteReleaseControl:getReleaseSettings();

		releaseSettings.saveTarget = Constants.RemoteReleaseControl.saveToHost;

		local randomName = math.random(1, 9999);
		releaseSettings.outputFilename = "IMG_" .. randomName .. ".jpg";

		releaseSettings.onFinishedTransfer = App.onFinishedTransfer;

		remoteReleaseControl:setReleaseSettings(releaseSettings);

	end;

	-- resets handler
	resetTransferHandler = function(remoteReleaseControl)

		local releaseSettings = remoteReleaseControl:getReleaseSettings();

		releaseSettings.onFinishedTransfer = nil;

		remoteReleaseControl:setReleaseSettings(releaseSettings);

	end;

	-- called when transfer of an image has finished
	onFinishedTransfer = function(self, releaseSettings)

		print("Received image: " .. releaseSettings.outputFilename .. "\n");

		self.eventFinishedTransfer:signal();

	end;

	-- checks viewfinder functions
	checkViewfinder = function(self, remoteReleaseControl)

		local capViewfinder = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capViewfinder);

		if (not capViewfinder) then
			print("Viewfinder not supported by this camera.");
			return;
		end;

		print("Starting viewfinder...\n");

		local viewfinder = remoteReleaseControl:startViewfinder();

		-- send viewfinder to LCD, too
		viewfinder:setOutputType(Constants.Viewfinder.outputTypeLCD);

		self.eventViewfinder = Sys:createEvent();

		viewfinder:setAvailImageHandler(App.onViewfinderImageAvail);

		-- wait for image to arrive, then continue
		print("Waiting for viewfinder image...\n");
		local imageWasAvail = self.eventViewfinder:wait(10.0);

		print("Captured " .. (imageWasAvail and "a viewfinder image!" or "no viewfinder image.") .. "\n");

		viewfinder:close();

	end;

	-- called when a viewfinder image has been sent
	onViewfinderImageAvail = function(self, viewfinder, imageData)

		print("Received an image from viewfinder\n");

		-- unregister handler
		viewfinder:setAvailImageHandler();

		self.eventViewfinder:signal();

	end;

	-- checks bulb mode functions
	checkBulbMode = function(self, remoteReleaseControl)

		local capBulbMode = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capBulbMode);

		if (not capBulbMode) then
			print("Bulb mode not supported by this camera.");
			return;
		end;

		local bulbReleaseControl = remoteReleaseControl:startBulb()

		-- wait for some seconds, then stop bulb mode
		local event = Sys:createEvent();

		event.wait(5.0);

		bulbReleaseControl:stop();

		local elapsed = bulbReleaseControl:elapsedTime();
		print("elapsed time: " .. elapsed .. " seconds.");

	end;
}
