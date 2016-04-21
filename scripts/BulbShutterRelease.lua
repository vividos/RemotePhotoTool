--
-- RemotePhotoTool - remote camera control software
-- Copyright (C) 2008-2016 Michael Fink
-- file BulbShutterRelease.lua - Demo script: Shutter release using Bulb release control
--

-- The App object is a global object, and its run() function is called by the Lua
-- scripting engine when starting the script.
App = {

	-- entry point function
	run = function(self)

		print("BulbShutterRelease.lua - Demo script: Shutter release using Bulb release control\n\n");

		local instance = Sys:getInstance();

		local allSourceInfos = instance:enumerateDevices();

		-- The enumerateDevices() function returns nil when no device is
		-- connected, so check before using
		if (allSourceInfos ~= nil and allSourceInfos.length > 0) then

			local remoteReleaseControl = self:openCamera(allSourceInfos[1]);

			self:checkBulbMode(remoteReleaseControl);

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


	-- checks bulb mode functions
	checkBulbMode = function(self, remoteReleaseControl)

		local capBulbMode = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capBulbMode);

		if (not capBulbMode) then
			print("Bulb mode not supported by this camera.");
			return;
		end;

		if (self:checkManualShootingMode(remoteReleaseControl)) then

			self:setBulbShutterSpeed(remoteReleaseControl);
			self:takeBulbModeImage(remoteReleaseControl);

		end;

	end;

	-- checks if the connected camera has the "Manual" shooting mode
	checkManualShootingMode = function(self, remoteReleaseControl)

		local capChangeShootingMode = remoteReleaseControl:getCapability(Constants.RemoteReleaseControl.capChangeShootingMode);

		local manualShootingMode = remoteReleaseControl:getShootingModeImageProperty(Constants.ShootingMode.shootingModeM);

		if (capChangeShootingMode) then

			-- we can change the shooting mode ourselves
			print("Set Shooting mode M...\n");

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

	-- sets Bulb shutter speed, in Manual mode, if necessary
	setBulbShutterSpeed = function(self, remoteReleaseControl)

		local shutterSpeed = remoteReleaseControl:getImagePropertyByType(Constants.ImageProperty.Tv);

		if (shutterSpeed.asString ~= "Bulb") then

			print("Set shutter speed \"Bulb\"...\n");

			-- in Canon EDSDK (for Canon DSLRs such as the 40D), shutter
			-- speed "Bulb" has the value hex 0x0C, decimal 12
			shutterSpeed.value = 12;

			remoteReleaseControl:setImageProperty(shutterSpeed);

			-- wait some time so that image property has been set
			Sys:createEvent():wait(0.5);
		end;

	end;

	-- takes an image using bulb mode of camera, if supported
	takeBulbModeImage = function(self, remoteReleaseControl)

		print("Start taking image...\n");

		local bulbReleaseControl = remoteReleaseControl:startBulb()

		-- wait for some seconds, then stop bulb mode
		local event = Sys:createEvent();

		event:wait(5.0);

		bulbReleaseControl:stop();

		local elapsed = bulbReleaseControl:elapsedTime();
		print("Elapsed time: " .. elapsed .. " seconds.\n");

	end;
}
