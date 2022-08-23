REM
REM RemotePhotoTool - remote camera control software
REM Copyright (C) 2008-2016 Michael Fink
REM
REM runs CppCheck to check sourcecode
REM

set PATH=%PATH%;"C:\Program Files\Cppcheck\"

REM when started via command line, assume "no xml" and "current folder"
set INTDIR=%1
if "%INTDIR%" == "" set INTDIR=%CD%\

REM when xml is passed as second param, set format and output file
set FORMAT=
set OUTFILE=%INTDIR%cppcheck.txt
if "%2" == "xml" set FORMAT=--xml
if "%2" == "xml" set OUTFILE=%INTDIR%cppcheck-Results.xml

REM run cppcheck
REM -I <dir>            Include path
REM -i <dir>            Ignore path
REM --suppressions-list=<file>   File with suppressed warnings
REM -j 4                Multithreading
REM --platform=win32W   Platform specific types
REM --language=c++      Language (file extensions)
REM --std=c++11         Language (syntax)%
REM --enable=all        Enable warnings
REM --template vs       Output format for warnings
REM --check-config
cppcheck.exe ^
	..\Base\ ..\CameraControl\ ..\RemotePhotoTool\ ..\LuaScripting\ ..\Location\ ..\Logic\ ..\RemotePhotoToolCmdline\ ..\RemoteScriptingEditor\ ^
	-I ..\Base\ -I ..\LuaScripting\ -I ..\Location\ -I ..\Logic\ -I ..\CameraControl\exports\ ^
	-I ..\packages\wtl.10.0.10320\lib\native\include ^
	-i ..\Thirdparty ^
	-i ..\LuaScripting\lua-5.4.1 ^
	-i ..\Logic\Logic.UnitTest\ ^
	-i ..\Location\Location.UnitTest\ ^
	-i ..\LuaScripting\LuaScripting.UnitTest\ ^
	-DWIN32 -D_WINDOWS -DNDEBUG -D_UNICODE -D__cplusplus -D_MSC_VER=1900 ^
	-D__ATLAPP_H__ -DNTDDI_WIN7=0x06010000 -DNTDDI_VERSION=0x06010000 ^
	--suppressions-list=cppcheck-suppress.txt ^
	-j 4 --platform=win32W --language=c++ --std=c++17 %FORMAT% --enable=all --template vs 2> %OUTFILE%
