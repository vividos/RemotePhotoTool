@echo off
REM
REM RemotePhotoTool - remote camera control software
REM Copyright (C) 2008-2018 Michael Fink
REM
REM Runs Unit Tests and collects coverage information
REM

REM set this to your Visual Studio installation folder
set VSINSTALL=%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community

REM and this to your OpenCppCoverage folder
set OPENCPPCOVERAGE=D:\devel\tools\OpenCppCoverage\

REM
REM Preparations
REM
call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"

set PATH=%PATH%;%OPENCPPCOVERAGE%

REM
REM Build Debug|Win32
REM
msbuild RemotePhotoTool.sln /m /property:Configuration=Debug /property:Platform=Win32 /target:Build

REM
REM Run unit tests
REM
OpenCppCoverage.exe ^
   --continue_after_cpp_exception --cover_children ^
   --sources Logic --sources Logic\Logic.UnitTest ^
   --sources Location --sources Location\Location.UnitTest --sources Location\GPS --sources Location\NMEA0183 ^
   --sources LuaScripting --sources LuaScripting\LuaScripting.UnitTest ^
   --excluded_sources packages\boost --excluded_sources LuaScripting\lua-5.3.5 ^
   --export_type cobertura:RemotePhotoTool-coverage.xml ^
   --export_type html:CoverageReport ^
   --modules Logic.UnitTest.dll --modules Location.UnitTest.dll --modules LuaScripting.UnitTest.dll ^
   -- "%VSINSTALL%\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe" ^
   "..\bin\Debug\Logic.UnitTest.dll" "..\bin\Debug\Location.UnitTest.dll" "..\bin\Debug\LuaScripting.UnitTest.dll" /Platform:x86 /InIsolation /logger:trx

pause
