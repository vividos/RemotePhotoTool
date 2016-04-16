# RemotePhotoTool Lua Scripting #

This document describes the Lua scripting functionality of RemotePhotoTool.

## Introduction ##

RemotePhotoTool Lua scripting allows to control cameras connected to the host
computer via scripts written in the Lua language. The script can access all
properties and functions of the camera in the same way as the RemotePhotoTool
application. The scripting engine uses a cooperative multitasking scheduler to
asynchronously run code and wait for events sent from the camera. All these
features together allows to write Lua scripts that perform functions that
would not be possible when using the user interface of RemotePhotoTool alone.

Note that the remote control function is different to the approach to control
camera functions directly inside the device, as some projects such as CHDK and
MagicLantern allow. The Lua scripts only run on the host (the connected
computer). Of course these projects can be used in parallel to RemotePhotoTool.

### Lua scripts ###

The Lua language runtime is used inside the RemotePhotoTool to run code. The
language runtime is complemented with several additional libraries that
provide bindings to C++ code that actually accesses the camera functions. The
same C++ code also drives the functions of RemotePhotoTool. The Lua scripts
specially written to run inside RemotePhotoTool cannot be run with the
standalone Lua interpreter, since the mentioned bindings are missing.

If you want to refresh your Lua programming skills, read the "Lua quick
reference" chapter coming up next. If you're interested in how RemotePhotoTool
implements multithreading, read the chapter "Scheduler". If you want to
directly dive into programming your camera with Lua, continue with the chapter
"Camera interface".

## RemotePhotoTool integration ##

The next chapters describe the way how Lua scripting is integrated into the
RemotePhotoTool applications.

### "Scripting" Photo Mode ###

The main RemotePhotoTool application has several "photo modes" that provide
different ways to interact with the connected cameras. One of the photo modes
is the "Scripting" photo mode that lets you load pre-written Lua scripts. All
other photo modes require a connection to a connected camera, whereas the
"Scripting" photo mode can be accessed without a connection, too.

The "Scripting" contextual menu band that is available in the "Scripting" photo
mode offers several commands:

- Open: Opens a Lua script file (extension .lua) into the main view.
- Reload: Reloads a Lua script, e.g. when editing with an external editor.
- Run: Starts the currently loaded script.
- Stop: Stops the currently loaded script, resetting script execution.
- Edit: Starts the RemoteScriptingEditor application.

The main view of the photo mode shows the read-only Lua script that is loaded.
Editing of the script is possible using the "Edit" menu command that starts the
RemoteScriptingEditor application, or with any other editor of your choice.
Remember to reload the script when editing the file with another editor. Note
that after editing using the "Edit" menu command, the file is reloaded
automatically. 

Below the main view there is an "Output" window that shows script output and
error messages. It can be resized to see more of the output text.

The status bar of the application offers a pane on the right end of the bar, to
show the status of the script. The status of the script represents exactly the
states that the Scheduler is in. Refer to the chapter "Scheduler" above about
what possible states there are.

### RemoteScriptingEditor ###

The RemoteScriptingEditor is a standalone application that can be used to
write, run and test Lua scripts. The application can be started in the
"Scripting" photo mode using the "Edit" command, or can be started using the
Program menu short cut in the Windows Start menu.

The menu band (also called "ribbon") offer several commands. Possible keyboard
short cuts for the commands are shown below the command text.

The "File" tab offers a menu to access files, and also offers an "Exit" option
to exit the application. You can also exit the application using the [x] in the
title bar, or by double-clicking on the application icon in the title bar. The
tab also shows a list of "Recent Files" that were opened before. 

The "Home" tab contains multiple groups of commands. The commands in the "File"
group do the following:

- New: Creates a new empty Lua script in the main view.
- Open: Opens a Lua script file (extension .lua) into the main view.
- Save: Saves a modified Lua script with the name used while opening. When the
  file wasn't saved before, the file name must be entered in the file save
  dialog.
- Save As: Saves a modified Lua script with another file name.

The menu band commands in the "Edit" group do the following:

- Paste: Inserts previously copied or cut text from the clipboard.
- Cut: Cuts out text and moves it to the clipboard.
- Copy: Copies text to the clipboard.
- Select All: Selects all text in the editor.
- Undo: Undoes the last editing step, such as clipboard commands or key
  strokes.
- Redo: Re-does previously undone editing steps, effectively maintaining a
  sort of "editing history".

The menu band commands in the "Find" group do the following:

- Find: Starts a dialog where text to find can be entered.
- Find Next: After finding the first occurrence of a text, use this to find
  further occurrences.
- Replace: Starts a dialog where text to replace with alternative text can be
  entered.

The menu band commands in the "Run" group do the following:

- Run: Starts the currently loaded script.
- Stop: Stops the currently loaded script, resetting script execution.

The menu band commands in the "View" group do the following:

- Menu band: Switches from the menu band ("ribbon") view to the classic
  application menu.
- Output pane: Toggles showing/hiding the output window below the main view.

The main view of the application shows the Lua script that is currently loaded.
It can be edited using the keyboard, context menu commands and the menu band
commands. Line numbers are shown on the left side, so that error messages with
Lua script line numbers can be matched to the correct script line.

Below the main view there is an "Output" window that shows script output and
error messages. It can be resized to see more of the output text.

The status bar of the application offers a pane on the right end of the bar, to
show the status of the script. The status of the script represents exactly the
states that the Scheduler is in. Refer to the chapter "Scheduler" above about
what possible states there are.

## Lua quick reference ##

Here's a quick introduction to the Lua scripting language, in case you want to
refresh your skills or just want to see the syntax of the language. For a
complete introduction to Lua, see the official manual:
[http://www.lua.org/manual/5.3/](http://www.lua.org/manual/5.3/)

Lua is a dynamically typed language; this means that variables don't have
types, only values do. The available types are: nil, boolean, number, string,
function, userdata, thread and table. The types work as you would expect them
in other languages.

    -- this is a comment
    i = 3                          -- assignment to global variable
    local text = "Hello World!"    -- local variable inside a function

The function type can be used to pass around functions to be called, and are
also used for bindings to C++ functions. Functions assigned to variables
capture the values from their environment they use, so that the values are
available once the function is called.

    function transmogrify(a, b) -- specifies a function
       local resultSum = a + b;
       return resultSum, a * b; -- Lua supports multiple return values
    end

    local transform = function(a, b) -- stores function in local var
       return a * b + 1;
    end

    local result = transform(5, 4); -- calls stored function

The userdata type is an opaque value for managing data passed from C++. It
usually is used to pass around data blocks to other functions. The thread type
can be used for running multiple threads of execution (see more in the
"Scheduler" chapter).

Finally, the table type is used to represent objects, that means collections
of values and functions associated with that data. Tables can also be used as
other data-structuring mechanisms.

    Transmogrifier = { -- specifies a table with that name
       magicFactor = 4.0;
       objectToModify = { -- a nested table
          name = "Calvin";
       };
       transmogrify = function(self) -- receives the outer tabl
          print("magicFactor = " .. self.magicFactor .. "!\n");
          return 42;
       end;
    }

    -- syntactic sugar for calling a function inside a table,
    -- with passing the table as first argument
    Transmogrifier:transmogrify();

Lua has the usual control structures available:

    while i > 0 do
       print("number: " .. i .. "\n"); -- the dot-dot operator concatenates strings
       i = i - 1;
    end

    local i = 5;
    repeat
       print("modulo: " .. (i % 3) .. "\n");
       i = i - 1;
    until i == 0;

    for i = 1, 100 do
       local text = "";
       if i % 3 == 0 then text = text .. "Fizz"; end;
       if i % 5 == 0 then text = text .. "Buzz"; end;
       if text == "" then text = text .. i; end;
       print(text .. ", ");
    end

There is also a for-statement for collections, and a goto statement.

Lua uses garbage collection as automatic memory management. Values that are
not referenced anymore are collected at an unspecified moment in time.

Some notes about the language:

- The equality operator is ==, but the inequality operator is ~=

- There is no ternary ?: operator as in other languages, but you can use this
  construct:

    local text = (i % 1) and "odd" or "even"

You can also look at the following example Lua script, and you can use it to
play around with the Lua language itself:

[LuaQuickReference.lua](https://github.com/vividos/RemotePhotoTool/blob/master/scripts/LuaQuickReference.lua "")

### Scheduler ###

The Lua language runtime uses a scheduler to run scripts. This is done to
implement cooperative multitasking that is needed to handle concurrent
execution of code. The "cooperative" means that there is only one piece of
code running at any time, and that the code must give up on its execution to
allow other code to run, e.g. in response to events triggered by the camera.

The scheduler executes Lua script code in several threads. Since the
multithreading used by the scheduler is cooperative, only one "thread of
execution" can be run at the same time. There is one main thread, which is the
thread where code execution starts at the first line.

Some functions available in the Lua scripting environment allow to register
handler functions for different camera events. These are Lua functions that
you can pass as parameters to these functions. Once the event is triggered,
the function is called in an additional thread (which is never the main
thread).

The scheduler knows several states it can be in. These are:

- Idle: There is no Lua script code executing, and there are no handlers
  waiting that could be triggered by a camera event. This is the initial state
  when no script was started yet, or the last run script stopped executing
  without having handler waiting. When a script is started, the scheduler
  changes to the Running state.

- Running: Lua script code is currently running; this state makes no
  distinction if the main thread or a handler is running. If the script exits
  without registering handlers to wait for events, the scheduler changes to
  the Idle state. If handlers were registered, the scheduler changes to the
  Waiting state.

- Waiting: There is no Lua script code running, and the main thread isn't
  running. There are one or more handlers waiting for a camera event. When an
  event occurs and the handler is called, the scheduler changes back to the
  Running state. If all handlers for events are un-registered, the scheduler
  changes back to the Idle state when the script code ends.

- Debug: This is a special state that the scheduler enters when a debugger is
  attached to the scheduler, and either a breakpoint is hit, the debugger
  pauses execution during the Running state, or when stepping through the
  code.
  Note that there's currently no debugger available in RemotePhotoTool.

- Error: This is a special state that the scheduler enters when the Lua
  function error() is called. Note that this state currently can't be entered,
  and the error() call does nothing.

In summary, the scheduler provides running the Lua code, and handlers
registered for events are run when the main thread gives up its execution.
When there are no handler registered anymore, the scheduler stops running
again.

### Camera interface ###

The camera interface is provided as several additional tables and functions
that can be accessed in the script.

The Lua scripting starts up running all global Lua code, then tries to find
the table-object called App. In it there must exist a function named run()
that is called. The table should look like this:

    App = {
       function = run(self)
          -- write your code here
       end;
    }

The first step to work with the camera interface is to get an instance of the
camera interface, then call functions on it.

    local instance = Sys:getInstance();

    local allCameras = instance:enumerateDevices();

    -- The enumerateDevices() function returns nil when no device is connected
    if (allCameras and allCameras.length > 0) then
       print("Number of cameras: " .. allCameras.length .. "\n");

       for index = 1, allCameras.length do
          local cameraInfo = allCameras[index]
          print("Camera #" .. index .. ": " .. cameraInfo.name .. "\n");
       end;
       print("\n");
    end;

You can also register a function that should be called once a camera has been
connected and is available.

    instance:asyncWaitForCamera(App.onConnected);

The onConnected function is called with the "App" as only parameter. You can
then retrieve the list of devices using enumerateDevices(), as shown before.
The callback function may be also called when a disconnects, so be sure to
check the length of the list of cameras.

The list of cameras consist of a list of SourceInfo tables that list some
information about the device, such as name and deviceId:

    for idx = 1, allSourceInfos.length, 1 do
    	local sourceInfo = allSourceInfos[idx];
    	print("Camera #" .. idx .. ": " .. sourceInfo.name ..
			", device id: " .. deviceId .. "\n");
	end;

At this stage the camera isn't connected yet and can't be queried. The open()
function of the SourceInfo table object opens the camera for access:

	local sourceDevice = sourceInfo:open();

Now the script can access infos about the camera, such as capabilities, device
properties and other infos. 

	local capRemoteReleaseControl = sourceDevice:getDeviceCapability(
		Constants.SourceDevice.capRemoteReleaseControl);

	print("   can remote release shutter: " ..
		(capRemoteReleaseControl and "yes" or "no") .. "\n");

Here's how to enumerate device properties:

	local devicePropertyList = sourceDevice:enumDeviceProperties();

	for idx = 1, devicePropertyList.length, 1 do
		local devicePropertyId = devicePropertyList[idx];

		local deviceProperty = sourceDevice:getDeviceProperty(devicePropertyId);

		-- do something with deviceProperty

		end;
	end;

To actually control the camera, you first have to enter "remote release
control". Some cameras extend their lens only at this stage of connection:

	local remoteReleaseControl = sourceDevice:enterReleaseControl();

	remoteReleaseControl:release();

The RemoteReleaseControl offers many informations and ways to control the
camera, including:

- Reading and setting image properties (properties that affect the final image)
- sending commands, e.g. to adjust the auto focus
- setting "shutter release" settings
- pressing the shutter release button
- accessing live viewfinder images
- starting bulb shooting (if supported by the camera)

See the reference description for the RemoteReleaseControl table object for a
complete overview of the functions available, or study the examples available
with RemotePhotoTool. 

## Examples ##

The example scripts provided with RemotePhotoTool show all functions that are
currently possible with Lua scripting. You can use these as starting places and
can extend the scripts, or you can learn about how the many functions available
to the Lua scripts are used correctly.

Note that you can always look up functions and table-object structures that are
used in the examples, in the Reference chapter below.

The examples shown here as excepts are also available as complete and
standalone Lua scripts that you can download and modify. The scripts are
stored at GitHub:
[https://github.com/vividos/RemotePhotoTool/tree/master/scripts](https://github.com/vividos/RemotePhotoTool/tree/master/scripts "")

The remainder of this chapter describes the different Lua example scripts and
highlight the code that is essential for using the scripts.

### Waiting for cameras to connect ###

[WaitForCamera.lua](https://github.com/vividos/RemotePhotoTool/blob/master/scripts/WaitForCamera.lua "")

Usually the camera may not be connected via USB yet, so there's the possibility
to wait for an updated of the connected cameras list.

This creates an event to wait for an actual camera to connect, and starts
waiting for a camera by registering a handler (onConnected):

	self.eventCameraConnect = Sys:createEvent();

	instance:asyncWaitForCamera(App.onConnected);

Note that the handler is only called when our Lua script isn't running code,
but does the following:

- the script is waiting on an event
- the script's App.run() function has exited, or
- the script has yielded

As we have set up an event to wait for a camera to connect, wait for it:

	-- only waits for some seconds, then exists
	local ret = self.eventCameraConnect:wait(15.0);
	if (ret == false) then
		print("No camera connected for 15.0 seconds; exiting.\n");
	end;

In case we wait for 15 seconds, but the event is never "signaled" (e.g. was set
by another thread), print a message before exiting.

The handler function can be defined like this:

	onConnected = function(self)

		local instance = Sys:getInstance();
		local allSourceInfos = instance:enumerateDevices();

		-- more code here

	end;

Since the list of source infos that enumerateDevices() can be empty, we have
to check for this case: 

	if (allSourceInfos ~= nil and allSourceInfos.length > 0) then

		-- more code here

	else
		print("No cameras connected. Continue waiting...\n");
	end;

In the case where the list is empty, we just continue waiting. When the list
isn't empty, we can examine the list further. However, we first un-register the
handler again, so that the handler doesn't get called multiple times:

	instance:asyncWaitForCamera(); 

	-- now do something with the list of cameras, e.g. printing properties

	print("Finished.\n\n");

As last step in the onConnect() function, we signal our main thread of
execution that there was actually a camera connected:

	-- signal finished handler
	self.eventCameraConnect:signal();

When our handler exits, the wait() call on the eventCameraConnect object
returns, and the return code can be examined, as mentioned above. 

### Reading device and image properties ###

[PrintProperties.lua](https://github.com/vividos/RemotePhotoTool/blob/master/scripts/PrintProperties.lua "")

Once you have a SourceDevice table object, you can check the capabilities of
the device and enumerate the device properties.

	local sourceDevice = sourceInfo:open();

	print("   model name: " .. sourceDevice.modelName .. "\n");
	print("   serial no: " .. sourceDevice.serialNumber .. "\n");

The device capabilities describe if a camera has some specific function: 

	local capRemoteReleaseControl = sourceDevice:getDeviceCapability(
		Constants.SourceDevice.capRemoteReleaseControl);

	print("   can remote release shutter: " ..
		(capRemoteReleaseControl and "yes" or "no") .. "\n");

	local capRemoteViewfinder = sourceDevice:getDeviceCapability(
		Constants.SourceDevice.capRemoteViewfinder);

	print("   can show viewfinder: " ..
		(capRemoteViewfinder and "yes" or "no") .. "\n");

Based on the device capabilities you should decide if a camera can use a
functionality or not.

The device properties can be enumerated and listed this way:

	local devicePropertyList = sourceDevice:enumDeviceProperties();

	if (devicePropertyList and devicePropertyList.length > 0) then

		print("   number of device properties: " ..
			devicePropertyList.length .. "\n");

		for idx = 1, devicePropertyList.length, 1 do

			local devicePropertyId = devicePropertyList[idx];

			local prop = sourceDevice:getDeviceProperty(devicePropertyId);

			print("   property: id=" .. prop.id ..
				", name=\"" .. prop.name ..
				"\", value=\"" .. prop.asString ..
				"\", readonly=" .. (prop.isReadOnly and "yes" or "no") .. "\n");
		end;
	end;

Device properties are values that are stored in the camera, but usually don't
have an influence in the parameters when taking a photo. See image properties
below.


### Take an image ###

[TakeImage.lua](https://github.com/vividos/RemotePhotoTool/blob/master/scripts/TakeImage.lua "")

To actually take an image using the connected camera, get the "remote release
control" table object first, when you have a SourceDevice object:

	local remoteReleaseControl = sourceDevice:enterReleaseControl();

In this script, we have a dedicated openCamera() function that takes a
sourceInfo table object, checks the device capabilities, enters the "release
control" mode and returns the resulting RemoteReleaseControl table object. You
can use this convenience function to save a bit of typing.

	-- opens the camera to take images
	openCamera = function(self, sourceInfo)

		print("Opening camera " .. sourceInfo.name .. "\n");

		local sourceDevice = sourceInfo:open();

		local capRemoteReleaseControl = sourceDevice:getDeviceCapability(
			Constants.SourceDevice.capRemoteReleaseControl);

		if (not capRemoteReleaseControl) then
			print("Camera can't be remote controlled!");
			return nil;
		end;

		local remoteReleaseControl = sourceDevice:enterReleaseControl();

		return remoteReleaseControl;
	end;

Now that you have the remoteReleaseControl object, you can prepare taking an
image by setting "release settings"; first get the current settings:

	local releaseSettings = remoteReleaseControl:getReleaseSettings();

Setting saveTarget determines where the images are stored; see the reference
for possible values of this constant:

	releaseSettings.saveTarget = Constants.RemoteReleaseControl.saveToHost;

We also need a file name that the output file should get, when transferred to
the host. Note that when the image is only stored on the camera, the file name
is ignored.  

	local randomName = math.random(1, 9999);
	releaseSettings.outputFilename = "IMG_" .. randomName .. ".jpg";

As last value before storing the settings, we register a handler function that
is called when transfer of an image has finished:

	releaseSettings.onFinishedTransfer = App.onFinishedTransfer;

	remoteReleaseControl:setReleaseSettings(releaseSettings);

Now we can send commands, e.g. for adjusting auto focus:

	remoteReleaseControl:sendCommand(
		Constants.RemoteReleaseControl.commandAdjustFocus);

And finally we can virtually press the shutter release button: 

	remoteReleaseControl:release();

Note that the release() function immediately returns, as the "shutter release"
is done asynchronously.

The handler function we previously set, can look like this:

	onFinishedTransfer = function(self, releaseSettings)

		print("Received image: " .. releaseSettings.outputFilename .. "\n");

		self.eventFinishedTransfer:signal();

	end;

The releaseSettings object is a copy of the settings we previously set, above.
The function uses an event to signal that the image has been transferred, in
the same way as when waiting for a camera. For this to work, we create an event
object (it's best to create it before the release() call:

	self.eventFinishedTransfer = Sys:createEvent();

After the release() call (which returns immediately, not waiting for the photo
to be taken, we just wait for the event to be signaled:

	local result = self.eventFinishedTransfer:wait(10.0);

	print(result and "Finished.\n" or "Failed waiting for image.\n");

After waiting, we could finish the script, or we could take another image, or
whatever is being desired to do. Once you're not interested in the event that
an image has been transferred, reset the handler function like this:

	local releaseSettings = remoteReleaseControl:getReleaseSettings();

	releaseSettings.onFinishedTransfer = nil;

	remoteReleaseControl:setReleaseSettings(releaseSettings);

When you're done with controlling the camera, you can quit the "release
control" mode again:
 
	remoteReleaseControl:close();

On some cameras, this causes the lens to retract into the camera case again. 

### Receiving viewfinder image data ###

[Viewfinder.lua](https://github.com/vividos/RemotePhotoTool/blob/master/scripts/Viewfinder.lua "")

The live viewfinder of most cameras can be used to fetch live images from the
camera. On DSLR cameras the mirror is locked up, which may consume more battery
power than just taking images.

Note that the viewfinder can be used to implement "mirror lockup" on DSLR
cameras, by activating viewfinder, waiting for some fractions of seconds, then
take a photo. This may be useful for long exposure images.

To use the viewfinder, you must already have entered the "release control" mode
by getting the RemoteReleaseControl table object:

	local remoteReleaseControl = sourceDevice:enterReleaseControl();

Once again you could use the openCamera() function mentioned above.

We first check the capabilities, if viewfinder operations are supported at all:

	local capViewfinder = remoteReleaseControl:getCapability(
		Constants.RemoteReleaseControl.capViewfinder);

	if (not capViewfinder) then
		print("Viewfinder not supported by this camera.");
		return;
	end;

Then we can start the live viewfinder:

	local viewfinder = remoteReleaseControl:startViewfinder();

Again there is a handler function involved that is called when a new viewfinder
image is transferred from the camera. We also create an event to signal our
main thread once an image arrived:

	self.eventViewfinder = Sys:createEvent();

	viewfinder:setAvailImageHandler(App.onViewfinderImageAvail);

The handler function we previously set, can look like this:

	onViewfinderImageAvail = function(self, viewfinder, imageData)

		print("Received an image from viewfinder\n");

		-- unregister handler
		viewfinder:setAvailImageHandler();

		self.eventViewfinder:signal();

	end;

It receives the App object in the first parameter, a copy of the viewfinder
table object in the second, and the image data in the third.

The viewfinder object can be used to unregister the handler, as shown.

The imageData object is a Lua userdata object that wraps the actual data bytes.
Unfortunately at the moment you can't do much with this data. The data block
contains JPEG image data.

The remainder of the main function (after calling startViewfinder()) just waits
for the event that we registered before:

	local imageWasAvail = self.eventViewfinder:wait(10.0);

	print("Captured " .. (imageWasAvail and "a viewfinder image!" or "no viewfinder image.") .. "\n");

To stop the viewfinder, you can do this:

	viewfinder:close();

After the close call no handler can be set anymore.

### Cooperative multithreading ###

[Multithreading.lua](https://github.com/vividos/RemotePhotoTool/blob/master/scripts/Multithreading.lua "")

This script doesn't interact with a camera, but should demonstrate the behavior
of the Lua scripting Scheduler object when running multiple threads of
execution, which are executed co-operatively.

There is a distinction if we are running on the main thread (the one that
starts by running in App.run():

	App = {
		run = function(self)

			print("on main thread: " ..
				(Sys:isMainThread() and "yes" or "no") .. "\n");

			-- more code here

		end;
	}

First we create an event object to play around with it:

	self.event = Sys:createEvent();

The self parameter of the run() function (and all further functions) is always
the App object itself.

We also define another function that should run on another thread of execution:

	threadFunc = function(self)

		print("on main thread: " .. (Sys:isMainThread() and "yes" or "no") .. "\n");

		self.event:signal();

		if (not Sys:isMainThread()) then
			coroutine.yield(self.thread);
		end;

	end;

The function first outputs if it's on the main thread (which depends on how we
call it). Then it signals the event we created before. The last statement
leaves the function by yielding when not on the main thread. We get to that
soon.

Now when we would call the threadFunc() function on our main thread (go ahead
and try it out in the RemoteScriptingEditor!), it would first print "yes", then
would signal the event, then return normally.

Lua has some built-in libraries, and one of them is called "coroutine". It
enables us to start "threads of execution" separately from the main thread.
First, a coroutine object is created, by passing a function to
coroutine.create(), like this: 

	self.thread = coroutine.create(self.threadFunc);

The returned object is a thread object that can be resumed:

	coroutine.resume(self.thread, self);

The second parameter is passed on to the threadFunc() function. The resume()
call pauses the main thread and starts to execute the thread by calling
threadFunc(), but not on the main thread anymore.

Our function would print "no", as it's not on the main thread anymore, then
would signal the event by calling signal(), and then calling coroutine.yield(),
which temporarily stops the thread again. The thread becomes "yielded" and can
be activated again using coroutine.resume().

When we would instead exited the threadFunc() function normally, the thread
would become "finished", and when resumed with coroutine.resume() would restart
the function at the beginning.

The next script code in the main thread is:

	local ret = self.event:wait(10.0);

We just wait there for the event to be signaled, which should be done when the
threadFunc() function runs.

Internally the wait() function also uses the same function that the coroutine
library uses to start, resume and yield threads.

### The all-in-one script ###

[RemotePhotoTool.lua](https://github.com/vividos/RemotePhotoTool/blob/master/scripts/RemotePhotoTool.lua "")

This script is a combination of all previous scripts and ties all functions
together. The example script isn't discussed further, see the different scripts
above for examples of the individual functions.

## Reference ##

This reference lists all Lua functions and tables available in the scripting
environment of RemotePhotoTool.

Note that all function are specified with their return value first, if there
is one, the function name, and potential function arguments in parenthesis.

    print(text)                function returning nul, with one parameter
    string getVersion()        function returning a string value

If the arguments are optional, square brackets are placed around the
arguments, too.

    wait([waitTimeInMilliseconds])

If the function is to be called as part of an object, the name of the table is
set before the function, with a colon between table name and function name.

    Sys:getInstance()          function of an instance of table Sys

When using the function in code, you have to use the actual table object to
call it.

### global functions ###

#### print(text) ####

This function prints the passed text to the console window. Note that this
function overwrites the global Lua function also called print().

### Sys table ###

The global Sys table provides access to system-specific functions.

    Sys = {
      getInstance = function() { ... };
      isMainThread = function() { ... };
      createEvent = function() { ... };
    }

#### Sys:getInstance() ####

This is the entry point to the camera interface. The function returns a table
to interact with the camera functions. See table Instance for more infos.

#### Sys:isMainThread() ####

Returns a boolean value (true or false) that determines if the code that is
calling this function is running on the main thread. Depending on being on the
main thread or not, you can call methods that asynchronously wait for events
to happen. Generally, when not being on the main thread, such calls would
result in an error and would stop the running script.

For a discussion of the scheduler, see the chapter about the Scheduler above.

#### Sys:createEvent() ####

Creates a new event object and returns it, in the form of a table object. The
table object has no values, but several functions. See table Event for more
infos.

### Event table ###

An event table object is created using Sys:createEvent(). The event object is
used for signalling events in the cooperative multithreaded scripting
environment that is running the scripts. The event table has no specified
values, but it has some functions:

    event = {
      signal = function() { ... };
      reset = function() { ... };
      wait = function(...) { ... };
    }

For a discussion of the scheduler, see the chapter about the Scheduler above.

#### Event:signal() ####

Signals the event. A thread that is waiting on this event using the wait()
function is resumed to be run, after the function that called signal() has
returned from execution.

#### Event:reset() ####

Resets the event to be non-signaled. This is used to prepare an event object
to be reused again. This makes the event a manual-reset event.

#### boolean Event:wait([waitTimeInSeconds]) ####

Waits for the event to be signaled from another thread. The parameter passed
to this function is optional, and can be omitted. It specifies the number of
seconds to wait for the event. When omitted, it is waiting indefinitely.

If the event wasn't signalled yet, the function returns after the waiting time
has elapsed; it returns false in this case. If the event was signaled during
waiting, it returns before the waiting time has elapsed, and it returns true.

### Instance table ###

The instance table object is the starting point to interact with the cameras
attached to the computer. The instance table is returned by the function
Sys:getInstance(). Each call to the function returns a new object, but that
object provides the same functions, and so can be considered a "singleton".
The Instance table has several functions:

    instance = {
      getVersion = function() { ... };
      enumerateDevices = function() { ... };
      asyncWaitForCamera = function(...) { ... };
    }

#### string Instance:getVersion()

Returns a version string that can be printed. It has the same content as the
about box text of the RemotePhotoTool application.

#### devices-array-table Instance:enumerateDevices() ####

Enumerates all currently connected camera devices and returns it in a table
that is used as an array. It has values with a 0-based index as key, and a
length value that specifies how many values there are. The returned values
are SourceInfo table objects.

Here's an example table:

    devices = {
      length = 2;
      [1] = sourceInfoFirstDevice;
      [2] = sourceInfoSecondDevice
    }

The SourceInfo table object can be used to interact with the cameras further.

#### Instance:asyncWaitForCamera([callbackFunction]) ####

Waiting for cameras to be connected is done asynchronously. This function lets
you register a Lua function that is called as soon as a new camera has been
connected, or when a camera is switched off. The function you pass (along with
all needed closure values) is stored internally and are not garbage collected.

The passed function is only called when the scheduler is in the Waiting state;
the code calling this function must either wait on an event or return from
execution, in order to let the callback function be called. For more infos
about waiting states and asynchronous functions, see the chapter about the
Scheduler above.

The callback function can be omitted; the effect is that waiting for the event
is canceled, and your previously registered function and its closure variables
are removed, and may be garbage collected.

Note that as soon as you opened a camera object, you have other means to
detect if the opened camera disconnects. This function should not be used for
this reason.

### SourceInfo table ###

The source info table describes the found camera object, without opening it.
Opening the object usually has a delay of a fraction of a second. The table
has some values and some functions available:

    sourceInfo = {
      name = "Canon PowerShot G9";
      deviceId = "{EEC5AD98-8080-425f-922A-DABF3DE3F69A}\\0002";
      open = function(){ ...}
    }

#### SourceDevice-table SourceInfo:open() ####

Opens the camera device for reading properties and performing actions with it.
Executing this normally takes a fraction of a second. On some camera models
the lens is extended. The returned value is a SourceDevice table.

### SourceDevice table ###

The source device table has the following values and functions:

    sourceDevice = {
      modelName = "Canon PowerShot G9";
      serialNumber = "1234567890";
    
      getDeviceCapability = function(deviceCapability) { ... };
      enumDeviceProperties = function() { ... };
      getDeviceProperty = function(devicePropertyId) { ... };
      enterReleaseControl = enterReleaseControl() { ... };
    }

The connection to the camera is closed by setting the sourceDevice value to
nul and waiting for the garbage collector to collect the object. Currently
there is no deterministic point in time when the camera connection is closed.

#### boolean SourceDevice:getDeviceCapability(deviceCapability) ####

Queries for a device capability, which can either be true or false. If a
capability is false, some functions of the camera can't be carried out.

The following capabilities exists, as constants:

- Constants.SourceDevice.capRemoteReleaseControl:
  Determines if remote release control is available for this camera; this
  value determines if SourceDevice:enterReleaseControl() can be called. This
  capability should be available for all cameras, except maybe the very old
  Canon PowerShot models.

- Constants.SourceDevice.capRemoteViewfinder:
  Determines if remote viewfinder is available for this camera; this value
  determines if RemoteReleaseControl::startViewfinder() can be called.

#### property-ID-array-table SourceDevice:enumDeviceProperties() ####

Enumerates all device properties of the camera and passes it in an array of
device property IDs, stored in an array table. The device properties can be
accessed by calling SourceDevice:getDeviceProperty() with the device property
ID.

Device properties are values stored in the camera that describe camera device
properties, such as model name, owner or battery status. The values don't
change often.  For a detailed list of all device property values, use
RemotePhotoTool's "Device Properties Photo Mode" that lists all values
supported by your camera.

Here's an example table:

    deviceProerties = {
      length = 21;
      [1] = devicePropertyIdForModelName;
      [2] = devicePropertyIdForOwner;
      ...
      [21] = devicePropertyIdForBatteryStatus;
    }

#### DeviceProperty SourceDevice:getDeviceProperty(devicePropertyId) ####

For a given device property ID, returns a DeviceProperty table with infos
about the property. See description of table DeviceProperty to see which
infos are returned.

#### RemoteReleaseControl-table SourceDevice:enterReleaseControl() ####

Enters into a mode where you can actually control the remote camera. A
RemoteReleaseControl table object is returned. On most camera devices without
changeable lenses (e.g. PowerShot cameras), this extends the lens and switches
off the live viewfinder on the device.

### RemoteReleaseControl table ###

The RemoteReleaseControl table contains the following functions:

    remoteReleaseControl = {
      -- miscellaneous
      getCapability = function(remoteReleaseCapability) { ... };
      getReleaseSettings = function(releaseSettings) { ... };
      setReleaseSettings = function(releaseSettings) { ... };
    
      -- event handling
      addPropertyEventHandler = function(callbackFunction) { ... };
      removePropertyEventHandler = function(handlerId) { ... };
    
      addStateEventHandler = function(callbackFunction) { ... };
      removeStateEventHandler = function(handlerId) { ... };
    
      addDownloadEventHandler = function(callbackFunction) { ... };
      removeDownloadEventHandler = function(handlerId) { ... };
    
      -- properties
      enumImageProperties = function() { ... };
      getImageProperty = function(imagePropertyId) { ... };
      getImagePropertyByType = function(imagePropertyType) { ... };
      getShootingModeImageProperty = function(shootingMode) { ... };
      numAvailableShots = function() { ... };
    
      -- viewfinder related
      startViewfinder = function() { ... };
    
      -- remote release related
      sendCommand = function(cameraCommand) { ... };
      release = function() { ... };
      startBulb = function() { ... };

      -- cleanup
      close = function() { ... };
    }

#### boolean RemoteReleaseControl:getCapability(remoteReleaseCapability) ####

Queries for capabilities of remote release control, which can either be true
or false. If a capability is false, some functions of the camera can't be
carried out.

The following capabilities exists, as constants:

- Constants.RemoteReleaseControl.capChangeShootingParameter:
  Determines if any shooting paramter can be changed using remote release
  control. This capability may be false for the very old PowerShot camera
  models only.

- Constants.RemoteReleaseControl.capChangeShootingMode:
  Determines if the "Shooting mode" property can be changed using remote
  release control. Usually on DSLR cameras the shooting mode can only be
  changed by using the dial on the camera. The Powershot cameras allow to
  change the shooting mode independent of the dial position on camera.
  This can be used to notify the script user that the dial must be put to the
  proper position.

- Constants.RemoteReleaseControl.capZoomControl:
  Determines if the zoom setting can be controlled using remote release
  control. For all bridge cameras this determines if you can move the lens
  using the zoom image properties. For all DSLR cameras this means if the
  digital magnification of the live viewfinder image is possible. The actual
  zoom must be operated on the lens itself.

- Constants.RemoteReleaseControl.capViewfinder:
  Determines if the viewfinder can be used to stream live preview images to
  the host PC. This capability may be false for the very old PowerShot camera
  models only.

- Constants.RemoteReleaseControl.capReleaseWhileViewfinder:
  Determines if the shutter release can be operated while the viewfinder is
  active. This capability may be false for the very old PowerShot camera
  models only.

- Constants.RemoteReleaseControl.capAFLock:
  Determines if the auto focus lock camera function can be used. The AF lock
  prevents re-focusing of the image when the shutter is released.
  Note that the AF lock cannot currently be controlled using Lua scripting.

- Constants.RemoteReleaseControl.capBulbMode:
  Determines if the camera supports bulb mode shooting. Most newer DSLR
  cameras support this mode. All of the bridge cameras don't support bulb
  shooting. See RemoteReleaseControl:startBulb() for more infos.

- Constants.RemoteReleaseControl.capUILock:
  Determines if the camera UI (the buttons and dials on the camera) can be
  locked, so that changes on the camera are ineffective to the actual release
  settings.
  Note that the UI lock cannot currently be controlled using Lua scripting.

#### ReleaseSettings-table RemoteReleaseControl:getReleaseSettings() ####

Returns the current release settings that are used when taking images. The
table returned by this function can be used to call setReleaseSettings() with
modified table values.

See the description of the ReleaseSettings table to see what values can be
used in this table.

#### RemoteReleaseControl:setReleaseSettings(releaseSettings) ####

Sets new release settings that are used from this call on. The settings are
stored internally and can be retrieved with getReleaseSettings() again.

See the description of the ReleaseSettings table to see what values can be
used in this table.

Note that on most cameras, pressing the shutter release button on the camera
itself also triggers taking photos. To also get notified when these images
are taken, set a default ReleaseSettings value with a handler to process these
out-of-bounds images.

#### local handlerId = remoteReleaseControl:addPropertyEventHandler(callbackFunction) ####

Adds an event handler to notify script when an image property of the camera
has been changed. The callback function should look as follows:

    callbackFunction = function(self, propertyEvent, eventParam) { ... }

The propertyEvent is a number value and can be compared with the following
constants:

- Constants.RemoteReleaseControl.propEventPropertyChanged:
  The value of a property has changed; either because the user changed a dial
  on the camera or due to the camera changing a parameter. The eventParam
  parameter is equal to the image property ID that was updated. If the
  eventParam value is 0, it is unknown which property changed; in this case
  it's best to get current values of all properties that are of interest.

- Constants.RemoteReleaseControl.propEventPropertyDescChanged
  The property description of a property has changed; this means that e.g. the
  read-only status of a property has changed, or the list of available values
  for a property has changed. The eventParam value contains the image property
  ID of the property that has changed, or 0 if it is not known.

The function returns a handler ID of the callback handler that can be used to
unregister the handler again. See method removePropertyEventHandler().

Note that all event handlers are automatically removed when the remote release
control object is closed.

#### remoteReleaseControl:removePropertyEventHandler(handlerId) ####

Removes a property event handler from the remote release control. After
calling this function, the handler previously registered with
addPropertyEventHandler() won't be called again.

#### local handlerId = remoteReleaseControl:addStateEventHandler(callbackFunction) ####

Adds an event handler to notify script when an state event happened on the
camera. The callback function should look as follows:

    callbackFunction = function(self, stateEvent, eventParam) { ... }

The stateEvent is a number value and can be compared with the following
constants:

- Constants.RemoteReleaseControl.stateEventCameraShutdown:
  This event is sent when the camera is being shut down. No more operations on
  remote release control are possible.

- Constants.RemoteReleaseControl.stateEventRotationAngle:
  This event is sent when the camera is rotated, changing from landscape to
  portrait view or vice versa. The eventParam may or may not tell the new
  direction of the camera.

- Constants.RemoteReleaseControl.stateEventMemoryCardSlotOpen:
  This event is sent when the memory card slot was opened; when this event
  happens, no more actions should be performed, since pictures may not be
  written to the camera memory card.

- Constants.RemoteReleaseControl.stateEventReleaseError:
  This event is sent when an attempt at releasing the shutter was made, but
  there was an error; e.g. the auto focus couldn't determine focus, or other
  errors while releasing.

- Constants.RemoteReleaseControl.stateEventBulbExposureTime:
  This event is sent during bulb operations, to tell the number of seconds
  already elapsed. The eventParam value is the number of seconds since start.
  See BulbReleaseControl table for how to control camera in Bulb mode.

- Constants.RemoteReleaseControl.stateEventInternalError:
  This event is sent when an internal error occured.

Note that not all cameras send all the existing state events.

The function returns a handler ID of the callback handler that can be used to
unregister the handler again. See method removeStateEventHandler().

Note that all event handlers are automatically removed when the remote release
control object is closed.

#### remoteReleaseControl:removeStateEventHandler(handlerId) ####

Removes a state event handler from the remote release control. After
calling this function, the handler previously registered with
addStateEventHandler() won't be called again.

#### local handlerId = remoteReleaseControl:addDownloadEventHandler(callbackFunction) ####

Adds an event handler to notify script when an image is downloaded from the
camera. The callback function should look as follows:

    callbackFunction = function(self, downloadEvent, eventParam) { ... }

The downloadEvent is a number value and can be compared with the following
constants:

- Constants.RemoteReleaseControl.downloadEventStarted:
  Indicates that a download of an image has started, either because
  RemoteReleaseControl:release() was called, or because the user pressed the
  shutter release button on the camera. The eventParam value is set to 0
  always.

- Constants.RemoteReleaseControl.downloadEventInProgress:
  Is called when progress is made on downloading the image from camera to PC.
  The eventParam value is set to the percentage of completion of transfer.
  Note: If you're connected to a very old Canon Powershot camera that uses
  the CD-SDK, the value is set to 0, 1 or 2, and not the percentage.

- Constants.RemoteReleaseControl.downloadEventFinished:
  Indicates that a download of an image has finished. The eventParam value is
  set to 0 always. After this download event was set, the onFinishedTransfer
  handler from ReleaseSettings is called.

Note that download events only occur when the relase settings (see table
ReleaseSettings) are configured to save the image on the PC (saveTarget has to
be set to saveToCamera or saveToBoth).

The function returns a handler ID of the callback handler that can be used to
unregister the handler again. See method removeDownloadEventHandler().

Note that all event handlers are automatically removed when the remote release
control object is closed.

#### remoteReleaseControl:removeDownloadEventHandler(handlerId) ####

Removes a download event handler from the remote release control. After
calling this function, the handler previously registered with
addDownloadEventHandler() won't be called again.

#### image-property-ID-array-table RemoteReleaseControl:enumImageProperties()
####

Enumerates all image properties of the camera and passes it in an array of
image property IDs, stored in an array table. The image properties can be
accessed by calling RemoteReleaseControl:getImageProperty() with the image
property ID.

Image properties are values used during taking images to determine how the
image is taken and processed. Among the values are "shooting mode", aperture
Av value, shutter speed Tv value, ISO value, and many others. For a detailed
list of all image property values, use RemotePhotoTool's "Image Properties
Photo Mode" that lists all values supported by your camera.

Note that some values might not be available during some shooting modes, e.g.
in Av mode the Tv value can't be set, or in M mode the EV value can't be set.

Here's an example table:

    imageProperties = {
      length = 80;
      [1] = imagePropertyIdShootingMode;
      [2] = imagePropertyIdAperture;
      ...
      [80] = imagePropertyIdWhiteBalance;
    }

#### RemoteReleaseControl:getImageProperty(imagePropertyId) ####

For a given image property ID, returns a ImageProperty table with infos
about the property. See description of table ImageProperty to see which
infos are returned.

#### local imageProperty = remoteReleaseControl:getImagePropertyByType(imagePropertyType) ####

Returns an image property, by given type. Due to the different cameras, models
and SDKs used, the image property IDs don't have the same numbers. To access
most common properties, this method can be used to use a type to get the image
property. See description of table ImageProperty to see which infos are
returned.

Note that when the camera doesn't support the image property, the function
returns nil. Also note that the image property may be read-only, so that you
can't set a new value.

The folowing constants can be used for imagePropertyType:

- Constants.ImageProperty.shootingMode:
  Returns the shooting mode, e.g. Auto, Program, Av, Tv, M, or any other modes
  that the camera supports. Note that often this property is not settable.

- Constants.ImageProperty.driveMode:
  Returns the drive mode, e.g. Single, Continuous, Self-timer.

- Constants.ImageProperty.ISOSpeed:
  Returns the current ISO speed; some cameras have a dial to adjust this on
  the fly.

- Constants.ImageProperty.meteringMode:
  Returns the current exposure metering mode, e.g. Center-weight, Spot,
  Average, etc.

- Constants.ImageProperty.AFMode:
  Returns the current auto focus mode, e.g. One-Shot AF, Continuous AF, Manual
  focus, etc.

- Constants.ImageProperty.Av:
  Returns the currently set aperture value; depending on what shooting mode is
  set, this may or may not be settable and/or enumerable (e.g. in Av you can
  set an aperture, whereas in Tv the value is automatically chosen by the
  camera).

- Constants.ImageProperty.Tv:
  Returns the currently set shutter speed value; depending on what shooting
  mode is set, this may or may not be settable and/or enumerable (e.g. in Tv
  you can set a shutter speed, whereas in Av the value is automatically chosen
  by the camera).

- Constants.ImageProperty.exposureCompensation:
  Returns the current exposure compensation value that adjusts how light or
  dark the image will be; on most cameras you can adjust this from +2 to -2,
  in 1/3 steps.

- Constants.ImageProperty.flashExposureComp:
  Returns the current flash exposure compensation value; when flash is
  enabled, the camera chooses an adequate flash intensity, and with this
  property you can adjust the value down or up; on most cameras you can adjust
  this from +2 to -2, in 1/3 steps.

- Constants.ImageProperty.focalLength:
  Returns the current focal length of the lens of the camera, measured in mm.
  This value is read-only for most cameras, and zoom is set with image
  property type currentZoomPos instead.

- Constants.ImageProperty.flashMode:
  Returns the current flash mode, e.g. Off, Auto, On, Red-eye, Slow sync, etc.

- Constants.ImageProperty.whiteBalance:
  Returns the current white balance setting, e.g. Auto, Sunny, Cloudy, or any
  other white balance settings the camera supports.

- Constants.ImageProperty.AFDistance:
  Returns the current auto focus distance after focusing.

- Constants.ImageProperty.currentZoomPos:
  Returns the current zoom position; this property can also be used to set a
  new zoom position on some cameras. On Canon DSLR cameras, this sets the
  magnification of the live viewfinder image, which can be 1x, 5x or 10x.

- Constants.ImageProperty.maxZoomPos:
  Returns the maximum zoom position on the camera.

- Constants.ImageProperty.availableShots:
  Returns the number of available shots to store, depending on the size of the
  memory card in the camera.

- Constants.ImageProperty.saveTo:
  Returns where images taken by RemoteReleaseControl:release(), or when the
  user presses the shutter button, are stored. Possible values are:

  - Constants.RemoteReleaseControl.saveToCamera:
    Images are only stored on camera memory card; no download events are
    generated, as the images are not transferred to PC.

  - Constants.RemoteReleaseControl.saveToHost:
    Images are only transferred to PC, but not stored on camera memory card.

  - Constants.RemoteReleaseControl.saveToBoth:
    Images are stored on camera memory card and are transferred to PC.

- Constants.ImageProperty.batteryLevel:
  Returns the current battery level. The last value always is the one describing the
  most empty state.

- Constants.ImageProperty.imageFormat:
  Returns the image format that is currently set.

#### local imageProperty = remoteReleaseControl:getShootingModeImageProperty(shootingMode) ####

Returns an image property value that can be used to set a new shooting mode.
Note that for some cameras, shooting mode can't be set (e.g. Canon DSLRs). In
this case you have to notify the user that he has to change the shooting mode
dial to the appropriate mode.

The folowing constants can be used for shootingMode:

- Constants.ShootingMode.shootingModeP:
  Sets Program mode. Av and Tv can't be controlled in this mode and is chosen
  by the camera, depending on metering mode. Exposure compensation can be used
  to darken or lighten the image, though.

- Constants.ShootingMode.shootingModeTv:
  Sets "Tv" or shutter priority mode. Shutter speed can be adjusted in this
  mode, and aperture (Av) is chosen by the camera. Exposure compensation can
  be also be used here.

- Constants.ShootingMode.shootingModeAv:
  Sets "Av" or aperture priority mode. Aperture can be adjusted in this mode,
  and shutter speed (Tv) is chosen by the camera. Exposure compensation can
  be also be used here.

- Constants.ShootingMode.shootingModeM:
  Sets manual mode; both aperture (Av) and shutter speed (Tv) can be
  controlled in this mode. Exposure compensation is ignored and is read-only.
  Note: Use this mode to shoot in bulb mode, if the camera supports it. Bulb
  shooting is done using the RemoteReleaseControl:startBulb() function.

The image property returned here can be set using the
RemoteReleaseControl.setImageProperty() function.
(Note: function doesn't exist yet).

#### integer RemoteReleaseControl:numAvailableShots() ####

Returns the number of shots available on the storage medium of the camera,
using the current image format.

#### Viewfinder-table RemoteReleaseControl:startViewfinder() ####

Starts viewfinder. The viewfinder is a source of thumbnail images that are
normally shown on the LCD screen on the back of the camera. These thumbnails
are sent continuously and are normally displayed by RemotePhotoTool. Using
the viewfinder in scripting can let you analyze the viewfinder image.

On some cameras the LCD screen is turned off when starting the viewfinder.

See the Viewfinder table for more details how to use the viewfinder.

#### RemoteReleaseControl:sendCommand(cameraCommand) ####

Sends a command to the camera to be carried out. The call to this function
returns immediately. The following camera commands are available:

- Constants.RemoteReleaseControl.commandAdjustFocus:
  Starts auto-adjusting focus of current image.

- Constants.RemoteReleaseControl.commandAdjustWhiteBalance:
  Starts auto-adjusting white balance of current image.

- Constants.RemoteReleaseControl.commandAdjustExposure
  Starts auto-adjusting exposure of current image.

#### RemoteReleaseControl:release() ####

Starts taking an image asynchronously. The call to this function returns
immediately, and the callback function that was set using setReleaseSettings()
can be used to wait for taking and downloading the image.

There are some error conditions, e.g. the camera cannot focus. The error
conditions must be monitored using the state event handler. See the function
addStateEventHandler() for more infos.

Note that on most cameras, pressing the shutter release button on the camera
itself also triggers taking photos.

#### BulbReleaseControl-table RemoteReleaseControl:startBulb() ####

Starts Bulb release control. This lets you control how long the shutter of the
camera is opened to take a picture. Usually the camera must be set to shutter
mode M and a Tv value of "Bulb" to enable the Bulb mode.

The call returns a BulbReleaseControl table that lets you control the length
of the shutter opening. See description of the BulbReleaseControl table for
more details.

Note that only some of the Canon DSLR cameras support Bulb mode. Use the
function RemoteReleaseControl:getCapability() with the constant
Constants.RemoteReleaseControl.capBulbMode to determine if the camera supports
this mode.

#### RemoteReleaseControl:close() ####

Closes access to remote release control. On some cameras the lens is retracted
back into the case. Access to the other functions of the RemoteReleaseControl
table isn't possible anymore.

### DeviceProperty table ###

The DeviceProperty table contains the following functions:

    deviceProperty = {
      id = 12345;
      name = "Camera Model";
      asString = "Canon PowerShot G9";
      isReadOnly = true;
    }

The values have the following meaning:

- id: The device property ID that was passed to the getDeviceProperty()
  function.
- name: The display name of the property.
- asString: The value of the property, as displayable text.
- isReadOnly: Determines if the value can currently be set.

There are no functions that can be called.

### ImageProperty table ###

The ImageProperty table contains the following values:

    imageProperty = {
      id = 23456;
      name = "Aperture";
      asString = "f/2.8";
      isReadOnly = false;
    }

The values have the following meaning:

- id: The image property ID that was passed to the getImageProperty()
  function.
- name: The display name of the property.
- asString: The value of the property, as displayable text.
- isReadOnly: Determines if the value can currently be set.

There are no functions that can be called.

### ReleaseSettings table ###

The ReleaseSettings table that is used by the functions getReleaseSettings()
and setReleaseSettings() contains the following values:

    releaseSettings = {
      saveTarget = Constants.RemoteReleaseControl.saveToCamera;
      onFinishedTransfer = function(self, releaseSettings) { ... };
      outputFilename = "images\IMG_0001.jpg";
    }

The saveTarget value determines where images taken are stored. See the values
that can be used, below.

The onFinishedTransfer is a callback function that is called when an image has
been transferred completely. The callback function receives a copy of the
release settings that were used to take the image. The output filename then
contains the actual absolute filename. The callback function should look as
follows:

    callbackFunction = function(self, releaseSettings) { ... }

The outputFilename is the next desired filename that is used when images
should be transferred to host. The path is always relative to the currently
running script and can't leave outside the folder where the script is stored.
Non-existing subfolders are created before the image is stored. Forward
slashes are converted to the Windows-style backward slashes before using the
path. When the onFinishedTransfer callback is called, it contains the full
path of the image stored.

The following constants can be used for saveTarget:

- Constants.RemoteReleaseControl.saveToCamera:
  The images taken are saved on the camera only. The onFinishedTransfer
  callback function isn't called when the image has been stored on camera
  media.

- Constants.RemoteReleaseControl.saveToHost:
  The images are saved on the host only. The onFinishedTransfer callback is
  called when the transfer of the image has finished. No images are stored on
  the camera media.

- Constants.RemoteReleaseControl.saveToBoth:
  The images are saved on the camera and on the host. The onFinishedTransfer
  callback is called when the transfer of the image has finished. The image is
  also stored on the camera media. The camera chooses the next filename in the
  DCIM schema for the image, which is independent of the value that was passed
  as outputFilename.

### Viewfinder table ###

The Viewfinder table contains the following functions:

    viewfinder = {
      getCapability = function(viewfinderCapability) { ... };
      setOutputType = function(outputType) { ... };
      setAvailImageHandler = function([callbackFunction]) { ... };
      getHistogram = function(histogramType) { ... };
      close = function() { ... };
    }

#### boolean Viewfinder:getCapability(viewfinderCapability) ####

Queries for capabilities of the live viewfinder, which can either be true or
false. If a capability is false, some functions of the camera can't be carried
out.

The following capabilities exists, as constants:

- Constants.Viewfinder.capOutputTypeVideoOut:
  Determines if the Video Out output type can be set, using the function
  Viewfinder:setOutputType(). Some SDKs may not provide a way to redirect the
  viewfinder output to the Video Out connector.

- Constants.Viewfinder.capGetHistogram:
  Determines if the camera supports retrieving histogram data for viewfinder
  images. See Viewfinder:getHistogram() for more infos.

#### Viewfinder:setOutputType(outputType) ####

This function selects where the viewfinder image should be shown, in addition
to transfer it to the PC.

The following constants can be used:

- Constants.Viewfinder.outputTypeLCD:
  The live viewfinder image is also shown on the camera's LCD screen.

- Constants.Viewfinder.outputTypeVideoOut:
  The live viewfinder image is also sent to the camera's Video Out connector,
  to be viewed on an external screen.
  Note: Doesn't work on Canon EOS based cameras, since the function can be
  controlled remotely.

- Constants.Viewfinder.outputTypeOff:
  The live viewfinder image is only sent to the PC.

#### Viewfinder:setAvailImageHandler([callbackFunction]) ####

This function sets a handler function that is called every time a new
viewfinder preview image arrives. The callback function receives the preview
image. The callback function should look as follows:

    callbackFunction = function(self, viewfinder, image-as-userdata) { ... }

The first argument is the App object. The second argument is the viewfinder
instance object; it can be used to unregister the handler. The third argument
is image data as Lua userdata object. Unfortunately you can't do much with the
userdata value at the moment.

If no or a nil callback function is passed, the handler is unregistered. To
receive calls to this callback function, the main thread must give up its
execution. For more infos about asynchronous functions, see the chapter about
the Scheduler above.

#### Histogram-table Viewfinder:getHistogram(histogramType) ####

Returns the histogram of the last transferred viewfinder image. The histogram
is the distribution of light and dark parts of the image. The histogram is
only available when the camera has the capability, which you can check with:

    viewfinder:getCapability(Constants.Viewfinder.capGetHistogram)

The histogram type determines which histogram data is returned:

- Constants.Viewfinder.histogramLuminance
  Returns the histogram data for overall luminance of the image.

- Constants.Viewfinder.histogramRed
  Returns the histogram data only for the red channel of the image.

- Constants.Viewfinder.histogramGreen
  Returns the histogram data only for the green channel of the image.

- Constants.Viewfinder.histogramBlue
  Returns the histogram data only for the blue channel of the image.

The histogram table returned has the following layout

    devices = {
      length = 256;
      [1] = intensityValue1;
      [2] = intensityValue2;
      ...
      [256] = intensityValue256;
    }

Values at the low indexes represent intensities for dark colors, and values
at high indexes represent intensities for light colors. Seen in another way
the values are the number of colors in the image with this luminance or
channel value.

The values are not normalized to any range. For that you have to find the
highest value and divide all other values by it.

#### Viewfinder:close() ####

The function stops the live viewfinder, and the registered callback function
isn't called anymore. No other operations on the viewfinder object can be done
either.

### BulbReleaseControl table ###

The BulbReleaseControl table is used to control how long the bulb time of the
image will be. The camera shutter is kept as long as the table-object is valid
and the stop() function isn't called. With this, you can implement arbitrary
shutter times.

The table contains the following functions:

    bulbReleaseControl = {
      elapsedTime = function() { ... }
      stop = function() { ... }
    }

#### float BulbReleaseControl:elapsedTime() ####

Returns the number of seconds elapsed since opening bulb release control. The
float value type allows to pass fractional values of seconds. The resolution
of the timer is about 15 ms, or 0.015 seconds.

#### BulbReleaseControl:stop() ####

Stops bulb release control and closes the shutter. The image is then written
to storage card, sent to the host PC or both, depending on the settings. After
this function is called, the table-object isn't usable anymore, and to start a
new bulb release, RemoteReleaseControl:startBulb() must be called again.
