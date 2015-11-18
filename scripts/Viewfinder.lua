--
-- RemotePhotoTool - remote camera control software
-- Copyright (C) 2008-2015 Michael Fink
-- file Viewfinder.lua - Demo script: Getting viewfinder images
--

-- The App object is a global object, and its run() function is called by the Lua
-- scripting engine when starting the script.
App = {

	-- event that is later used to get notified that a viewfinder preview image arrived
	eventViewfinder = nil;

	-- entry point function
	run = function(self)

		print("Viewfinder.lua - Demo script: Getting viewfinder images\n\n");

		local instance = Sys:getInstance();

		local allSourceInfos = instance:enumerateDevices();

		-- The enumerateDevices() function returns nil when no device is
		-- connected, so check before using
		if (allSourceInfos ~= nil and allSourceInfos.length > 0) then

			local remoteReleaseControl = self:openCamera(allSourceInfos[1]);

			if (remoteReleaseControl ~= nil) then
				self:checkViewfinder(remoteReleaseControl);
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

	-- checks viewfinder functions
	checkViewfinder = function(self, remoteReleaseControl)

		local capViewfinder = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capViewfinder);

		if (not capViewfinder) then
			print("Viewfinder not supported by this camera.");
			return;
		end;

		print("Starting viewfinder...\n");

		local viewfinder = remoteReleaseControl:startViewfinder();

		self.eventViewfinder = Sys:createEvent();

		viewfinder:setAvailImageHandler(App.onViewfinderImageAvail);

		-- wait for image to arrive, then continue
		local imageWasAvail = self.eventViewfinder:wait(10.0);

		print("Captured " .. (imageWasAvail and "a viewfinder image" or "no viewfinder image") .. "\n");

	end;

	-- called when a viewfinder image has been sent
	onViewfinderImageAvail = function(viewfinder, imageData)

		print("Received an image from viewfinder\n");

		-- unregister handler
		viewfinder:setAvailImageHandler();

		App.eventViewfinder:signal();

	end;
}
