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

			if (remoteReleaseControl ~= nil) then
				self:releaseShutter(remoteReleaseControl);
			end;

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
