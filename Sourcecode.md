# Sourcecode Documentation #

This document describes how the source code of RemotePhotoTool is organized.

## Source code ##

The application is developed using C++. Features from C++20 are used where possible.

The app uses classes from the Boost library where no appropriate class from the Standard
C++ Library could be used. The application also uses other libraries. See the
[Credits.md](Credits.md) file for more infos.

The app uses SDKs from Canon to connect and remote control cameras. The SDKs and documentation can
be downloaded via the "Digital Image Developer Programme". All needed headers, libs and
redistributable files are included in this package, so you don't need to apply for this program.

## Folder structure ##

The main package folder contains the following folders (most of them created by Visual Studio when
building the app):

- bin: Contains built binaries, including the setup.
- images: Contains images for the Screenshot.md file
- intermediate: Contains object files needed by the build to create the app.
- lib: Contains library files needed by the build to create the app.
- pages: Contains the github pages html files.
- scripts: Contains Lua scripts to demo the Lua scripting functions.
- src: Contains the source code.
- tools: Contains tools useful while developing.

### src folder ###

The `"src"` folder has the following projects:

- Base: Contains a base static library with classes common to several other of my projects
- Base\Base.UnitTest: Unit tests for Base library.
- CameraControl: Contains a static library to control the various cameras. See below for details.
- CameraControl\exports: Contains header files with public classes of CameraControl library.
- CanonEOSShutterCount: Contains a command line tool to read out shutter count of EOS cameras.
- cppcheck: Tool project to run installed cppcheck tool on all source code to check for coding errors.
- doxygen: Tool project to run doxygen tool on all source code to generate source documentation.
- Logic: Contains business-logic classes used by RemotePhotoTool and RemoteScriptingEditor.
- LuaScripting: Contains a static library to enable scripting of remote controlling cameras.
- LuaScripting\lua-x.x.x: Contains Lua C sourcecode for a specific Lua version.
- LuaScripting\LuaScripting.UnitTest: Unit tests for LuaScripting library.
- RemotePhotoTool: The main application.
- RemotePhotoTool.CameraControl: A C# bindings project (using C++/CLI) to provide functions of the
  CameraControl library to .NET users.
- RemotePhotoToolCmdline: Command line application to remote control a camera.
- RemoteScriptingEditor: Lua scripting editor to debug scripts to remote control a camera.
- Setup: Contains the setup project.
- Thirdparty: Contains all third party code, including the SDKs and the JPEG library.

### CameraControl ###

The CameraControl library is an encapsulation for the SDKs used in the main application. You
can find all public header files in the `"exports"` folder. All other files are only used inside
the library. The library contains several files with the Edsdk, Cdsdk and Psrec prefix that
implement the interfaces from the `"exports"` folder. The main starting point is the static
function

    `Instance Instance::Get();`

that returns the current instance of the library to access remote cameras. All further operations
should be obvious from the public functions and classes.

### Main application ###

The main application uses the public interface of the `CameraControl` library to remote control
cameras and is unaware of different SDKs and camera models. The application uses the ribbon menu
band interface for UI. It supports several "photo modes" that can be switched between, as long
as the camera is connected. The photo modes have different scope and don't present all properties
the remote camera can be set to. The application supports calling installed instances of the
programs "Hugin" (for panorama stitching) and "Photomatix Pro" (for HDR generation).

# Installation of components #

## Visual Studio Community 2026 ##

Install Visual Studio 2026. Any version will do, including Visual Studio Community, which
I'm using. Be sure to install the "Microsoft Foundation Classes for C++" feature, that can be found
under "Programming Languages > Visual C++" node when installing Visual Studio.
When installed into the default folders, the example batch files can be used to build the
application.

## WTL 10 ##

The application uses WTL, on top of ATL, for the user interface. WTL is automatically downloaded
when restoring NuGet packages.

## Boost ##

The application uses the Boost libraries. Boost is automatically downloaded when restoring NuGet
packages. Thhis happens the first time you're building the solution, which may take a while, since
Boost consists of many files.

## WiX 3.11.2 ##

To build the setup, you need to install the "Windows Installer for XML 3.11.1" package. Go to this
website:
[http://wixtoolset.org/](http://wixtoolset.org/)

Go to "Downloads" and download the "Wix311.exe". Install the package on your system.

To get Visual Studio integration, also install the "Wix Toolset Visual Studio 2022 Extension":
[https://marketplace.visualstudio.com/items?itemName=WixToolset.WixToolsetVisualStudio2022Extension](https://marketplace.visualstudio.com/items?itemName=WixToolset.WixToolsetVisualStudio2022Extension)

## RemotePhotoTool ##

Just open the file `"src\RemotePhotoTool.slnx"` and select `"Release"` and `"Win32"` in the toolbar.
Rebuild the application and run it.

## Release check list ##

Here's a checklist of what to do before each release.:

- Update Changelog.md file with all changes since last release
- Update all external libraries and external tools if needed
- Check all Documentation files for needed updates, especially Features.md
- Check in or stash all other changes
- Build all projects in Win32 | Release
- Fix cppcheck errors and doxygen warnings
- Compile all project with /analyze and fix all warnings
- Run all unit tests and fix all errors
- Use SonarCloud build to find and fix even more bugs
- Test built .msi setup if it installs properly
- Do a short smoke test with available cameras
- Fix all errors, check them in and push all changes
- Tag the git repository with tag "remotephototool-x.y.z" and push the tag
- Add a release and upload result .msi file, to releases on github.com
- Adjust index.html and download.html with link to new release and push it
- Done

Note that some changes to the code, e.g. build number, are now done
automatically by the AppVeyor build.
