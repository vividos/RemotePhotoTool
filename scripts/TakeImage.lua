--
-- RemotePhotoTool - remote camera control software
-- Copyright (C) 2008-2015 Michael Fink
-- file TakeImage.lua - Demo script: Takes an image
--

-- The App object is a global object, and its run() function is called by the Lua
-- scripting engine when starting the script.
App = {

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

		print("Taking an image ...\n");

		-- TODO register a download handler
		-- TODO set release settings

		-- auto focus on current view
		remoteReleaseControl:sendCommand(Constants.RemoteReleaseControl.commandAdjustFocus);

		remoteReleaseControl:release();

		-- TODO wait for photo to be downloaded

		print("Finished.\n");

	end;
}
