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
capture the values from their envoriment they use, so that the values are
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

TODO

## RemotePhotoTool integration ##

TODO

### "Scripting" Photo Mode ###

TODO

### RemoteScriptingEditor ###

TODO

## Examples ##

The examples... TODO

Note that you can always look up functions and table-object structures in the
Reference chapter, below.

The examples shown here as excepts are also available as complete and
standalone Lua scripts that you can download and modify. The scripts are
stored at GitHub:
[https://github.com/vividos/RemotePhotoTool/tree/master/scripts](https://github.com/vividos/RemotePhotoTool/tree/master/scripts "")

### Reading device and image properties ###

PrintProperties.lua

TODO

### Waiting for cameras to connect ###

WaitForCamera.lua

TODO

### Take an image ###

TakeImage.lua

TODO

### Receiving viewfinder image data ###

Viewfinder.lua

TODO

### Cooperative multithreading ###

Multithreading.lua

TODO

### The all-in-one script ###

RemotePhotoTool.lua

TODO

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
      [0] = sourceInfoFirstDevice;
      [1] = sourceInfoSecondDevice
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
      [0] = devicePropertyIdForModelName;
      [1] = devicePropertyIdForOwner;
      ...
      [20] = devicePropertyIdForBatteryStatus;
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
      -- TODO
    
      -- properties
      enumImageProperties = function() { ... };
      getImageProperty = function(imagePropertyId) { ... };
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
      length = 21;
      [0] = imagePropertyIdShootingMode;
      [1] = imagePropertyIdAperture;
      ...
      [80] = imagePropertyIdWhiteBalance;
    }

#### RemoteReleaseControl:getImageProperty(imagePropertyId) ####

For a given image property ID, returns a ImageProperty table with infos
about the property. See description of table ImageProperty to see which
infos are returned.

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
contains the actual absolute filename.

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
      setAvailImageHandler = function([callbackFunction]);
    }

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
