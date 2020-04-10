@echo on
REM
REM winLAME - a frontend for the LAME encoding engine
REM Copyright (c) 2020 Michael Fink
REM
REM Downloads libwdi from sourceforge.net and compiles it
REM

REM set this to the filename of the file to download
set VERSION=1.3.1
set PREFIX=libwdi-%VERSION%
set LIBUSB_WIN32_PREFIX=libusb-win32-bin-1.2.6.0

REM set this to your Visual Studio installation folder
set VSINSTALL=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community

REM download package
set URL=https://github.com/pbatard/libwdi/archive/v%VERSION%.tar.gz

if not exist %PREFIX%.tar.gz powershell -Command "& {Invoke-WebRequest -Uri %URL% -Out %PREFIX%.tar.gz}"

REM unzip
rmdir /s /q %PREFIX%\
del %PREFIX%.tar
"c:\Program Files\7-Zip\7z.exe" x %PREFIX%.tar.gz
"c:\Program Files\7-Zip\7z.exe" x %PREFIX%.tar
del %PREFIX%.tar 2> nul
del pax_global_header 2> nul

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

REM set up Visual Studio
call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"

REM compile
msbuild %PREFIX%\libwdi.sln /m /property:Configuration=Release,Platform=Win32

REM copy artifacts
copy "%PREFIX%\libwdi\libwdi.h" ..\src\Thirdparty\libwdi\
copy "%PREFIX%\Win32\Release\lib\libwdi.lib" ..\src\Thirdparty\libwdi\
REM copy "%PREFIX%\Win32\Release\dll\libwdi.dll" ..\src\Thirdparty\libwdi\
REM copy "%PREFIX%\Win32\Release\dll\libwdi.pdb" ..\src\Thirdparty\libwdi\
REM copy "%PREFIX%\Win32\Release\dll\libwdi.lib" ..\src\Thirdparty\libwdi\

pause
