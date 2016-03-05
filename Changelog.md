# RemotePhotoTool Changelog #

Note: For the complete changelog, including internal changes, you can always go to
the git sourcecode repository at [https://github.com/vividos/RemotePhotoTool](https://github.com/vividos/RemotePhotoTool).

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
