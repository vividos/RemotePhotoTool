@echo on
REM
REM winLAME - a frontend for the LAME encoding engine
REM Copyright (c) 2020 Michael Fink
REM
REM Downloads libwdi from sourceforge.net and compiles it
REM

REM set this to the filename of the file to download
set PREFIX=libwdi-1.3.0
set LIBUSB_WIN32_PREFIX=libusb-win32-bin-1.2.6.0

REM set this to your Visual Studio installation folder
set VSINSTALL=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community

REM download package
set URL=https://netcologne.dl.sourceforge.net/project/libwdi/releases/%PREFIX%.tar.gz

if not exist %PREFIX%.tar.gz powershell -Command "& {Invoke-WebRequest -Uri %URL% -Out %PREFIX%.tar.gz}"

REM unzip
rmdir /s /q %PREFIX%\
del %PREFIX%.tar
"c:\Program Files\7-Zip\7z.exe" x %PREFIX%.tar.gz
"c:\Program Files\7-Zip\7z.exe" x %PREFIX%.tar
del %PREFIX%.tar

REM also download libusb-win32
set URL=https://netcologne.dl.sourceforge.net/project/libusb-win32/libusb-win32-releases/1.2.6.0/%LIBUSB_WIN32_PREFIX%.zip

if not exist %LIBUSB_WIN32_PREFIX%.zip powershell -Command "& {Invoke-WebRequest -Uri %URL% -Out %LIBUSB_WIN32_PREFIX%.zip}"

REM unzip
rmdir /s /q %LIBUSB_WIN32_PREFIX%\ 2> nul
"c:\Program Files\7-Zip\7z.exe" x %LIBUSB_WIN32_PREFIX%.zip

REM copy additional files
xcopy /s /y libwdi-msvc\*.* %PREFIX%\msvc\

REM set path to libusb-win32 folder
set LIBUSB_WIN32_PATH=%CD%\%LIBUSB_WIN32_PREFIX%\
pushd %PREFIX%\msvc
powershell -Command "& {(Get-Content config.h) -replace \"D:/libusb-win32\",$env:LIBUSB_WIN32_PATH | out-file config.h}"
popd

REM update Toolset to v142, in order to use VS2019
pushd %PREFIX%\libwdi\.msvc
powershell -Command "& {(Get-Content libwdi_dll.vcxproj) -replace \"v140\",\"v142\" | out-file libwdi_dll.vcxproj}"
powershell -Command "& {(Get-Content detect_64build.vcxproj) -replace \"v140\",\"v142\" | out-file detect_64build.vcxproj}"
powershell -Command "& {(Get-Content installer_x86.vcxproj) -replace \"v140\",\"v142\" | out-file installer_x86.vcxproj}"
powershell -Command "& {(Get-Content installer_x64.vcxproj) -replace \"v140\",\"v142\" | out-file installer_x64.vcxproj}"
powershell -Command "& {(Get-Content embedder.vcxproj) -replace \"v140\",\"v142\" | out-file embedder.vcxproj}"
popd

REM set up Visual Studio
call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"

REM compile
msbuild %PREFIX%\libwdi\.msvc\libwdi_dll.vcxproj /m /property:Configuration=Release,Platform=Win32

REM copy artifacts
copy "%PREFIX%\Win32\Debug\dll\libwdi.dll" ..\src\Thirdparty\libwdi\
copy "%PREFIX%\Win32\Debug\dll\libwdi.pdb" ..\src\Thirdparty\libwdi\
copy "%PREFIX%\Win32\Debug\dll\libwdi.lib" ..\src\Thirdparty\libwdi\

pause
