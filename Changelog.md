# RemotePhotoTool Changelog #

Note: For the complete changelog, including internal changes, you can always go to
the git sourcecode repository at [https://github.com/vividos/RemotePhotoTool](https://github.com/vividos/RemotePhotoTool).

## Version 1.7.0

### tl;dr ###

- Added Camera File System photo mode to access the camera's file system.
- Added RemotePhotoToolCmdline option `--dir`
- Added support for all PTP2 cameras that can be accessed using libgphoto2.
- Added standalone GeoTagTool to read in GPX tracks and geo-tag JPEG images.

#### Other ####

The following libraries were updated:
- Lua 5.4.4
- IJG jpeg-9e
- Canon ED-SDK version 13.16.0, released on 2022-12-07.

## 2020-06-30: Version 1.6.0 Build 129 ##

This is a major feature release.

### tl;dr ###

- Added Time Lapse photo mode
- Dropped Windows XP support

### New features ###

#### UI ####

Implemented the new photo mode "Time lapse" that allows to take many photos
and combine them to a time lapse video. The release trigger can be set to take
images in a fixed interval, immediately after transferring the previous image
or manually. There is also a scheduling option to set a start and end time.
The feature even allows to take HDR images.

Additionally there's a menu entry in the RemotePhotoTool's main menu to
combine already existing time lapse photos together.

The tool "ffmpeg" is needed to combine all time lapse photos together. The
tool can be downloaded separately, from here: https://ffmpeg.org/
The path to the tool can be configured in the Settings dialog.

#### Other ####

The following libraries were updated:
- Lua 5.3.5
- jpeg-9d
- Canon ED-SDK version 13.12.10, released on 2020-01-29.

### Bug fixes ###

- On cameras supporting mirror lockup, this is now used between taking HDR
  photos instead of using live viewfinder.
- Fixed crash when no camera is connected, switching to the "scripting photo
  mode" and then clicked on any photo mode ribbon button.

## 2016-05-13: Version 1.5.0 Build 121 ##

This is a minor feature and major bugfix release.

### tl;dr ###

There is some new UI stuff, e.g. setting Image Format and Video Out, and
storing window position. Lua scripting support was extended, including
completed Lua bindings and live Lua syntax checking in the editor. Two new
command line tools were introduced. And many many bugs were fixed.

### New features ###

#### UI ####

Implemented setting Image Format on the Camera contextual tab, so that The
user can change the image format used for the next shot.

Implemented setting Video Out on the Camera contextual ribbon, so that
viewfinder image is also shown on LCD display; only works on Powershot
cameras.

Both RemotePhotoTool and RemoteScriptingEditor now remember their last window
position and will start in the same position and maximize state.

About dialogs are now resizable.

#### Lua scripting ####

Completed implementation of Lua bindings to the internal CameraControl library;
now all functions can be carried out that can be done in the UI. Among these
are:
- Setting Video Out for viewfinder
- Adding / removing handler for download, state and property events
- Getting / setting image properties
- Special function to get shooting mode image properties
- Getting capabilities and histogram from viewfinder
- ImageProperty and DeviceProperty tables now have two new properties: value
  and validValues.

Also some example Lua scripts were added to the github repository, e.g. for
Bulb shooting or listening to camera events.

Lua scripts can now use more built-in Lua libraries, such as the Basic library
or the OS library. The security critical functions are "sandboxed"; see Lua
manual for more infos.

RemoteScriptingEditor will now show Lua syntax errors directly in the editor,
with squiggly lines in the line with the error, and an annotation box below
the line, containing the exact Lua interpreter error message. The syntax
checks are done live, when not typing for one second.

#### Tools ####

Added new command line tool RemotePhotoToolCmdline.exe that can be used to
access and control one of the connected cameras. Start the tool with the
--help option to see all command line options.

Added new command line tool CanonEOSShutterCount.exe that reads out the
shutter count of the first connected Canon EOS DSLR camera. Note that this
doesn't work with all EOS models.

#### Camera specific ####

EOS cameras that report a "will soon shut down" event will automatically kept
switched on while connected to RemotePhotoTool, or when executing a Lua
script.

EOS and Powershot cameras: More property names are available for previously
unknown properties.

### Bug fixes ###

- Exif info in the Previous Images view are now displayed more user-friendly.
- Fixed texts output in the Camera Info dialog.
- Powershot / EOS cameras: When there was an error during shutter release,
  sometimes the UI didn't show the error text.
- Fixed crashes when running Lua scripts.
- Powershot cameras: Fixed error showing when first connecting to a camera,
  but operation continues normally; this was a timing problem, reading a
  property from camera that isn't ready yet.
- Zoom in/out buttons are now disabled when at start or end of zoom range.
- Fixed bug where ribbon buttons were disabled when changing to the Lua
  scripting photo mode.
- RemoteScriptingEditor now got a proper toolbar, for those using Windows XP
  that can't or don't like to use the ribbon menu.
- Corrected ribbon bar key tips, so that the two applications can also be used
  only using a keyboard.
- Removed "Recent files" ribbon menu in RemotePhotoTool, as it makes no sense.
- Fixed taking images with some EOS cameras that want to know the "capacity"
  of the host PC to display a "remaining images" number on the camera display.
  This occured with the Canon EOS 400D while testing.
- Fixed many bugs when running Lua scripts; Lua values weren't properly
  ref-counted.
- Fixed errors in Lua scripts provided by github repository.
- Fixed other various crashes when interacting with cameras in
  RemotePhotoTool.

## 2016-03-05: Version 1.4.0 Build 120 ##

Implemented the Lua Scripting Photo Mode. This photo mode lets you load a script written in the
programming language Lua. The script can be run on the host PC to remote control the camera by
accessing camera functions and waiting for camera events. A whole API to access the camera is
implemented. For more infos, including an introduction and an API cross reference, see the
document [https://github.com/vividos/RemotePhotoTool/blob/master/LuaScripting.md](https://github.com/vividos/RemotePhotoTool/blob/master/LuaScripting.md "LuaScripting.md")
Lua scripting uses Lua version 5.3.2.

Implemented the "Previous images" view that shows all images taken with RemotePhotoTool. The view
is accessible even when no camera is connected. It shows only the images taken in the current
program session.

Redesigned the user interface to use the ribbon more:

- When a camera is connected, the Camera contextual ribbon tab is available. It lets the user
  start a shutter release and adjust various camera settings, such as shooting mode, aperture,
  save-to location and more.
- When scripting photo mode is selected, the Scripting contextual ribbon tab is available. It lets
  the user open Lua scripts, start and stop running scripts, and switch to the standalone
  scripting editor RemoteScriptingEditor.exe.
- When live viewfinder is active, the Viewfinder contextual ribbon tab is available. It lets the
  user start auto focus and auto white balance, zoom in/out, select a lines mode and toggle
  showing overexposed areas in the live viewfinder.

RemotePhotoTool now uses the Canon EOS Digital SDK version 3.2.1 that supports many newer EOS DSLR
cameras. For a full list, see [SupportedCameras.md](SupportedCameras.md).

RemotePhotoTool is now built with Visual Studio Community 2015.

## 2014-11-22: Version 1.3.0 Build 118 ##

This is mainly a bugfix and minor changes release.

Upgraded to Canon EOS Digital Camera SDK (EDSDK) 2.14, supporting new cameras EOS 700D, EOS100D.

Fixed application locking up occasionally when changing photo view modes and/or switching on/off
viewfinder on EOS cameras.

Fixed the application consuming 100% CPU on one thread (or e.g. 25% overall on a quad-core CPU).

Fixed a crash when a connected camera only had low batteries and image or device properties should be
read from the camera.

Camera properties on the "Device properties" and "Image properties" views should correctly display
all known properties now.

It is now possible to use the camera shutter release button to trigger shooting images on Powershot
cameras. This worked only on EOS cameras before.

Enabled zooming functionality on Canon EOS cameras, in viewfinder.

Older Canon Powershot cameras supported by the "CD-SDK Canon SDK" are only supported and will work
on Windows XP and below. RemotePhotoTool only checks for these cameras on the supported Windows
versions.

All source code was checked with [Coverity](http://www.coverity.com/)'s source code checker. All
found issues were subsequently fixed. Here's the link to the scan project:
[https://scan.coverity.com/projects/vividos-remotephototool](https://scan.coverity.com/projects/vividos-remotephototool).

## 2014-04-11: Version 1.2.0 Build 114 ##

Implemented showing overexposed areas in viewfinder by showing a Zebra style pattern. The
function is switchable.

Added support for CD-SDK Canon SDK, potentially supporting many more cameras manufactured up
to 2005-06-30. See [SupportedCameras.md](SupportedCameras.md) for all camera models supported.

Note: Unfortunately those cameras require installing a WIA driver from Canon, and they only
work on Windows XP. As I don't own an XP computer anymore, I could only do limited tests with
an old Powershot G2. Please report any issues when using those cameras!
Also note that the official support for Windows XP from Microsoft expired at 2014-04-08.

Fixed starting the application under Windows XP. Also fixed updating photo mode icons on
toolbar when not connected.

Added "Raw" column to Device Property photo view to show the raw camera data.

Reduced logging to only log errors in most cases.

## 2014-03-10: Version 1.1.0 Build 101 ##

Added viewfinder controls, including auto focus, auto white balance, zoom in/out,
helper lines (for rule of thirds and golden ratio).

Added crash reporting; when the application crashes, a "Crash dump file", helpful
for developers, is written to "%LOCALAPPDATA%\RemotePhotoTool\crashdumps".

Fixed a bug where connecting to EOS cameras resulted in UI lockup.

Added an error message in case image properties can't be accessed in image property
combo boxes (the drop down controls used on various photo mode pages).

## 2014-02-08: Version 1.0.0 Build 100 ##
Initial version.
