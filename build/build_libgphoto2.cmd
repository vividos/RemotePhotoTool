@echo off
REM
REM RemotePhotoTool - remote camera control software
REM Copyright (c) 2008-2020 Michael Fink
REM
REM Downloads libgphoto2 and compiles it
REM

REM set this to the filename of the file to download
set PREFIX=libgphoto2-2_5_26

REM set this to your Visual Studio installation folder
set VSINSTALL=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community

REM download package
set URL=https://github.com/gphoto/libgphoto2/archive/%PREFIX%-release.zip

if not exist %PREFIX%.zip powershell -Command "& {Invoke-WebRequest -Uri %URL% -Out %PREFIX%.zip}"

REM unzip
rmdir /s /q %PREFIX%\ 2> nul
rmdir /s /q libgphoto2-%PREFIX%-release\  2> nul
"c:\Program Files\7-Zip\7z.exe" x %PREFIX%.zip
move libgphoto2-%PREFIX%-release %PREFIX%

REM copy additional files
xcopy /s /y libgphoto2-msvc\*.* %PREFIX%\

REM set up Visual Studio
call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"

pushd %PREFIX%\

REM compile
..\..\tools\nuget.exe restore libgphoto2.sln
msbuild libgphoto2.sln /m /p:Configuration=Release /p:Platform=Win32 /t:Rebuild

popd

REM copy artifacts
xcopy /y %PREFIX%\gphoto2\*.h ..\src\Thirdparty\gphoto2\include\gphoto2\
xcopy /y %PREFIX%\libgphoto2_port\gphoto2\*.h ..\src\Thirdparty\gphoto2\include\gphoto2\

xcopy /y %PREFIX%\bin\Release\*.lib ..\src\Thirdparty\gphoto2\lib\

xcopy /y %PREFIX%\bin\Release\*.dll ..\src\Thirdparty\gphoto2\bin\
xcopy /y %PREFIX%\bin\Release\*.pdb ..\src\Thirdparty\gphoto2\bin\
xcopy /y %PREFIX%\bin\Release\camlibs\*.dll ..\src\Thirdparty\gphoto2\bin\camlibs\
xcopy /y %PREFIX%\bin\Release\camlibs\*.pdb ..\src\Thirdparty\gphoto2\bin\camlibs\
xcopy /y %PREFIX%\bin\Release\iolibs\*.dll ..\src\Thirdparty\gphoto2\bin\iolibs\
xcopy /y %PREFIX%\bin\Release\iolibs\*.pdb ..\src\Thirdparty\gphoto2\bin\iolibs\

move ..\src\Thirdparty\gphoto2\bin\iolibs\libusb-1.0.dll ..\src\Thirdparty\gphoto2\bin\libusb-1.0.dll

pause
