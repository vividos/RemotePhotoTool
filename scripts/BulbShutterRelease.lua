--
-- RemotePhotoTool - remote camera control software
-- Copyright (C) 2008-2016 Michael Fink
-- file BulbShutterRelease.lua - Demo script: Shutter release using Bulb release control
--

-- The App object is a global object, and its run() function is called by the Lua
-- scripting engine when starting the script.
App = {

	-- event that is set when transferring image has finished
	eventFinishedTransfer = nil;

	-- entry point function
	run = function(self)

		print("BulbShutterRelease.lua - Demo script: Shutter release using Bulb release control\n\n");

		local instance = Sys:getInstance();

		local allSourceInfos = instance:enumerateDevices();

		-- The enumerateDevices() function returns nil when no device is
		-- connected, so check before using
		if (allSourceInfos ~= nil and allSourceInfos.length > 0) then

			local remoteReleaseControl = self:openCamera(allSourceInfos[1]);

			if (self:checkManualShootingMode(remoteReleaseControl)) then

				self:checkBulbMode(remoteReleaseControl);

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

	-- checks if the connected camera has the correct shooting mode
	checkManualShootingMode = function(self, remoteReleaseControl)

		local capChangeShootingMode = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capChangeShootingMode);

		local manualShootingMode = remoteReleaseControl:getShootingModeImageProperty(Constants.ShootingMode.shootingModeM);

		if (capChangeShootingMode) then

			-- we can change the shooting mode ourselves
			print("Setting Shooting mode M...\n");

			-- TODO not implemented yet
			remoteReleaseControl:setImageProperty(manualShootingMode);

		else

			-- we can't change the shooting mode, so just check
			local shootingMode = remoteReleaseControl:getImagePropertyByType(Constants.ImageProperty.shootingMode);

			if (shootingMode.asString == manualShootingMode.asString) then
				print("Shooting mode M correctly set.\n");
			else
				print("Wrong shooting mode set! Please set your camera to Manual mode " ..
					"(current mode is " .. shootingMode.asString .. ").\n");
				return false;
			end;

		end;

		return true;
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
