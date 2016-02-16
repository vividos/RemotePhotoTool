--
-- RemotePhotoTool - remote camera control software
-- Copyright (C) 2008-2016 Michael Fink
-- file WaitForCamera.lua - Demo script: Waits for camera to connect
--

App = {

	-- event that is used to wait for connecting camera
	eventCameraConnect = nil;

	-- entry point function
	run = function(self)

		print("WaitForCamera.lua - Demo script: Waits for camera to connect\n\n");

		local instance = Sys:getInstance();

		self.eventCameraConnect = Sys:createEvent();

		instance:asyncWaitForCamera(App.onConnected);

		print("Waiting for camera to connect...\n");

		-- only waits for some seconds, then exists
		local ret = self.eventCameraConnect:wait(15.0);
		if (ret == false) then
			print("No camera connected for 15.0 seconds; exiting.\n");
		end;
	end;

	-- called when a camera has been connected
	onConnected = function(self)

		print("List of connected cameras changed\n\n");

		local instance = Sys:getInstance();

		local allSourceInfos = instance:enumerateDevices();

		-- The enumerateDevices() function returns nil when no device is
		-- connected, so check before using
		if (allSourceInfos ~= nil and allSourceInfos.length > 0) then

			-- unregister callback
			instance:asyncWaitForCamera();

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

	-- We call this function at onConnected() to print all infos about
	-- connected cameras, stored in SourceInfo objects.
	printSourceInfos = function(self, allSourceInfos)

		print("Number of cameras: " .. allSourceInfos.length .. "\n");

		-- Enumerate all source info objects
		for idx = 1, allSourceInfos.length, 1 do

			local sourceInfo = allSourceInfos[idx];

			print("Camera #" .. idx .. ": " .. sourceInfo.name .. "\n");
		end;
	end;

}
