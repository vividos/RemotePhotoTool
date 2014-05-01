# RemotePhotoTool Changelog #

Note: For the complete changelog, including internal changes, you can always go to
the git sourcecode repository at [https://github.com/vividos/RemotePhotoTool](https://github.com/vividos/RemotePhotoTool).

## 2014-??-??: Version 1.3.0 Build ??? ##

All source code was checked with [Coverity](http://www.coverity.com/)'s source code checker. All
found issues were subsequently fixed. Here's the link to the scan project:
[https://scan.coverity.com/projects/2058](https://scan.coverity.com/projects/2058).

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
