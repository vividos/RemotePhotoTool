--
-- RemotePhotoTool - remote camera control software
-- Copyright (C) 2008-2016 Michael Fink
-- file Viewfinder.lua - Demo script: Getting viewfinder images
--

-- The App object is a global object, and its run() function is called by the Lua
-- scripting engine when starting the script.
App = {

	-- event that is used to get notified that a viewfinder preview image arrived
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

	-- checks viewfinder functions
	checkViewfinder = function(self, remoteReleaseControl)

		local capViewfinder = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capViewfinder);

		if (not capViewfinder) then
			print("Viewfinder not supported by this camera.");
			return;
		end;

		print("Starting viewfinder...\n");

		local viewfinder = remoteReleaseControl:startViewfinder();

		-- print capabilities
		print("Viewfinder capabilities:\n");
		local capOutputTypeVideoOut = viewfinder:getCapability(Constants.Viewfinder.capOutputTypeVideoOut);
		print("   can set 'Video Out' output type: " .. (capOutputTypeVideoOut and "yes" or "no") .. "\n");

		local capGetHistogram = viewfinder:getCapability(Constants.Viewfinder.capGetHistogram);
		print("   can retrieve histogram: " .. (capGetHistogram and "yes" or "no") .. "\n");

		-- send viewfinder to LCD, too
		viewfinder:setOutputType(Constants.Viewfinder.outputTypeLCD);

		self.eventViewfinder = Sys:createEvent();

		viewfinder:setAvailImageHandler(App.onViewfinderImageAvail);

		-- wait for image to arrive, then continue
		print("Waiting for viewfinder image...\n");
		local imageWasAvail = self.eventViewfinder:wait(10.0);

		print("Captured " .. (imageWasAvail and "a viewfinder image!" or "no viewfinder image.") .. "\n");

		if (imageWasAvail and capGetHistogram) then
			self:readHistogram(viewfinder);
		end;

		viewfinder:close();

	end;

	-- called when a viewfinder image has been sent
	onViewfinderImageAvail = function(self, viewfinder, imageData)

		print("Received an image from viewfinder\n");

		-- unregister handler
		viewfinder:setAvailImageHandler();

		self.eventViewfinder:signal();

	end;

	-- reads histogram and finds maximum luminance value
	readHistogram = function(self, viewfinder)

		local histogram = viewfinder:getHistogram(Constants.Viewfinder.histogramLuminance);

		if (histogram ~= nil) then

			print("Histogram length: " .. histogram.length .. " entries.\n");

			local maxIndex = 0;
			local maxValue = 0;

			for index = 1, histogram.length do

				if (histogram[index] > maxValue) then
					maxIndex = index;
					maxValue = histogram[index];
				end;
			end;

			print("maximum luminance of viewfinder image at " ..
				(maxIndex * 100.0 / histogram.length) .. "% intensity\n");
		end;

	end;
}
