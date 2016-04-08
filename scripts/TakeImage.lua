--
-- RemotePhotoTool - remote camera control software
-- Copyright (C) 2008-2016 Michael Fink
-- file TakeImage.lua - Demo script: Takes an image
--

-- The App object is a global object, and its run() function is called by the Lua
-- scripting engine when starting the script.
App = {

	-- event that is set when transferring image has finished
	eventFinishedTransfer = nil;

	-- entry point function
	run = function(self)

		print("TakeImage.lua - Demo script: Takes an image\n\n");

		local instance = Sys:getInstance();

		local allSourceInfos = instance:enumerateDevices();

		-- The enumerateDevices() function returns nil when no device is
		-- connected, so check before using
		if (allSourceInfos ~= nil and allSourceInfos.length > 0) then

			local remoteReleaseControl = self:openCamera(allSourceInfos[1]);

			self:printCurrentShootingSettings(remoteReleaseControl);
			self:takeImage(remoteReleaseControl);

			remoteReleaseControl:close();
			print("Finished.\n\n");

		else
			print("No cameras connected. Exiting.\n");
		end;
	end;

	-- opens the camera to take images
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

	-- called when a download event occurs
	onDownloadEvent = function(self, downloadEventType, eventParam)

		local downloadEventName =
			downloadEventType == Constants.RemoteReleaseControl.downloadEventStarted and "downloadEventStarted" or
			downloadEventType == Constants.RemoteReleaseControl.downloadEventInProgress and "downloadEventStarted" or
			downloadEventType == Constants.RemoteReleaseControl.downloadEventFinished and "downloadEventStarted" or "???";

		print("onDownloadEvent! type=" .. downloadEventName ..
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
}
