--
-- RemotePhotoTool - remote camera control software
-- Copyright (C) 2008-2016 Michael Fink
-- file ListenEvents.lua - Demo script: Listens to events from camera
--

-- The App object is a global object, and its run() function is called by the Lua
-- scripting engine when starting the script.
App = {

	-- event that is set when transferring image has finished
	eventFinishedTransfer = nil;

	-- entry point function
	run = function(self)

		print("ListenEvents.lua - Demo script: Listens to events from camera\n\n");

		local instance = Sys:getInstance();

		local allSourceInfos = instance:enumerateDevices();

		-- The enumerateDevices() function returns nil when no device is
		-- connected, so check before using
		if (allSourceInfos ~= nil and allSourceInfos.length > 0) then

			local remoteReleaseControl = self:openCamera(allSourceInfos[1]);

			self:listenEvents(remoteReleaseControl);

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

	-- prints a single device or image property (they have the same table layout)
	printProperty = function(self, prop)

		print("   property: id=" .. prop.id .. ", " ..
			"name=\"" .. prop.name .. "\", " ..
			"value=\"" .. prop.asString .. "(" ..
			(prop.value ~= nil and prop.value or "nil") .. ")\", " ..
			"readonly=" .. (prop.isReadOnly and "yes" or "no") .. "\n");

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

		print("Waiting for events from camera....\n\n");

		local waitEvent = Sys:createEvent();
		waitEvent:wait(30.0);

		print("\nFinished listening to events.\n");

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

		self:printProperty(prop);

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
}
