# Sourcecode Documentation #

This document describes how the source code of RemotePhotoTool is organized.

## Source code ##

The application is developed using C++. Features from C++11 are used where possible. This means that at least Visual Studio 2013 is needed to compile the application.

The app uses classes from the Boost library where no appriopriate class from the Standard
C++ Library could be used. The application also uses other libraries. See the
[Credits.md](Credits.md) file for more infos.

The app uses SDKs from Canon to connect and remote control cameras. The SDKs and documentation can be downloaded via the "Digital Image Developer Programme". All needed headers, libs and redistributable files are included in this package, so you don't need to apply for this program.

## Folder structure ##

The main package folder contains the following folders (most of them created by Visual Studio when
building the app):

- bin: Contains built binaries, including the setup.
- intermediate: Contains object files needed by the build to create the app.
- lib: Contains library files needed by the build to create the app.
- src: Contains the source code.

### src folder ###

The `"src"` folder has the following structure:

- Base: Contains a base static library with classes common to several other of my projects
- Base.UnitTest: Unit tests for Base library.
- CanonControl: Contains a static library to control the various cameras. See below for details.
- cppcheck: Tool project to run installed cppcheck tool on all source code to check for coding errors.
- doxygen: Tool project to run doxygen tool on all source code to generate source documentation.
- Lua: Contains Lua C sourcecode.
- LuaScripting: Contains a static library to enable scripting of remote controlling cameras.
- LuaScripting.UnitTest: Unit tests for LuaScripting library.
- RemotePhotoTool: The main application.
- RemotePhotoToolCmdline: Command line application to remote control a camera.
- RemoteScriptingEditor: Lua scripting editor to debug scripts to remote control a camera.
- Setup: Contains the setup project.
- Thirdparty: Contains all third party code, including the SDKs and the JPEG library.

### CanonControl ###

The CanonControl library is an encapsulation for the SDKs used in the main application. You
can find all public header files in the `"export"` folder. All other files are only used inside
the library. The library contains several files with the Edsdk, Cdsdk and Psrec prefix that
implement the interfaces from the `"export"` folder. The main starting point is the static
function

    `Instance Instance::Get();`

that returns the current instance of the library to access remote cameras. All further operations
should be obvious from the public functions and classes.   

### Main application ###

The main application uses the public interface of the `CanonControl` library to remote control
cameras and is unaware of different SDKs and camera models. The application uses the ribbon menu
band interface for UI. It supports several "photo modes" that can be switched between, as long
as the camera is connected. The photo modes have different scope and don't present all properties
the remote contro can be set to. The application supports calling installed instances of the
programs "Hugin" (for panorama stitching) and "Photomatix Pro" (for HDR generation).

# Installation of components #

## Visual Studio 2013 ##

Install Visual Studio 2013. Any version will do, including Visual Studio 2013 Express for Windows
Desktop, which I use. When installed into the default folders, the example batch files can be used
to build the application.

When prompt (or manually via menu "Tools > Extensions and Updates"), update the Visual Studio
to "Update 4".

## ATL 7.1 ##

If you're using Visual Studio 2013 Express, you need to get ATL 7.1 to properly link the
application. If you're using the Professional or above version of Visual Studio, you don't need
to do this.

Get the Windows Driver Kit 7.1.0 from this website:
[http://msdn.microsoft.com/en-US/windows/hardware/gg487428](http://msdn.microsoft.com/en-US/windows/hardware/gg487428).
Open the .ISO file with your favourite ISO tool (I recommend 7-zip) and extract the
following folders:

- `atl71\include`
- `atl71\lib`

Put the files in a separate folder on your hard disk.

For the application to compile and link properly, you need to adjust the file `src\atl71-x86.props`
with the path of the folder you just used. There are two folders to adjust, one is the include
folder, beneath the <IncludePath> tag, and one is the `"lib\i386"` subfolder beneath the
`<LibraryPath>` tag.

## WTL 9.0 ##

The application uses WTL, on top of ATL, for the user interface. Download WTL 9.0.4140 (or any
later version) from this website:
http://sourceforge.net/projects/wtl/

You need to click on "Browse all files" and select WTL 9.0 to get to the proper folder. Here's
a link to the version I use:
http://sourceforge.net/projects/wtl/files/WTL%209.0/WTL%209.0.4140%20Final/WTL90_4140_Final.zip/download

Extract the contents of the zip archive in to a new folder. Navigate into the "AppWiz" folder and
double-click the file `"Setup.js"`.

## Boost ##

Download and extract Boost 1.57 (or any later version) into a folder. For the application to compile
you need to adjust the file `src\boost.props` with the path of the folder you just used. There are two
folders to adjust, one is the main folder, beneath the `<ClCompile>` tag, and one is the `"stage\lib"`
subfolder beneath the `<Link>` tag.

You also need to build (parts of) Boost to get to link the application correctly. You can use the
following command line commands (e.g. in a .cmd file) to compile Boost:

    call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\vsvars32.bat"
    call .\bootstrap.bat vc12

    b2 toolset=msvc-12.0 -j4 --with-system runtime-link=shared link=static threading=multi stage

This should generate the appropriate lib files in the `"stage\lib"` folder, for the library
Boost.System.

## WiX 3.7 ##

To build the setup, you need to install the "Windows Installer for XML 3.7" package. Go to this
website:
[http://wix.codeplex.com/](http://wix.codeplex.com/)

Go to "Downloads" and download the "Wix37.exe" under the "WiX Toolset v3.7" release. Later releases
of WiX may or may not work. Install the package on your system.

For the setup to compile you need to adjust the file `src\wix-v3.props` with the path of the
folder you installed the WiX Toolset. Add the path to the `"bin"` folder to the setting beneath the
`<ExecutablePath>` tag.

## RemotePhotoTool ##

Just open the file `"src\RemotePhotoTool.sln"` and select `"Release"` and `"Win32"` in the toolbar.
Rebuild the application and run it.

## Release check list ##

Here's a checklist of what to do before each release:

- Update version number in version.h
- Update version number and ProductId in Config.wxi, and BuildNumber in BuildNumber.wxi
- Update Changelog.md file with all changes since last release
- Check all Documentation files for needed updates, especially Features.md
- Check in or stash all changes
- Build all projects in Win32 | Release
- Fix cppcheck errors and doxygen warnings
- Compile all project with /analyze and fix all warnings
- Test built .msi setup if it installs properly
- Do a short smoke test with available cameras
- Fix all errors, check them in and push all changes
- Tag the git repository with tag "remotephototool-x.y.z" and push the tag
- Add a release and upload result .msi file, to releases on github.com
- Adjust README.md and index.html with link to new release and push it
- Done
