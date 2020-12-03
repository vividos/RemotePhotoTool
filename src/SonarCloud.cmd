@echo off
REM
REM RemotePhotoTool - remote camera control software
REM Copyright (C) 2008-2020 Michael Fink
REM
REM Runs SonarCloud analysis build
REM

REM set this to your Visual Studio installation folder
set VSINSTALL=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community

REM
REM Preparations
REM
call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat"

if "%SONARLOGIN%" == "" echo "Environment variable SONARLOGIN is not set! Obtain a new token and set the environment variable!"
if "%SONARLOGIN%" == "" exit 1

REM
REM Extract SonarQube build tools
REM
pushd Thirdparty\SonarQube
"%ProgramFiles%\7-Zip\7z.exe" x -y build-wrapper-win-x86.zip
"%ProgramFiles%\7-Zip\7z.exe" x -y -osonar-scanner-msbuild sonar-scanner-msbuild-5.0.3.23901-net46.zip
PATH=%PATH%;%CD%\build-wrapper-win-x86;%CD%\sonar-scanner-msbuild
popd

REM
REM Build using SonarQube scanner for MSBuild
REM
rmdir .\.sonarqube /s /q 2> nul
rmdir .\bw-output /s /q 2> nul

msbuild RemotePhotoTool.sln /m /property:Configuration=SonarCloud,Platform=Win32 /target:Clean

SonarScanner.MSBuild.exe begin ^
    /k:"RemotePhotoTool" ^
    /v:"1.6.0" ^
    /d:"sonar.cfamily.build-wrapper-output=%CD%\bw-output" ^
    /d:"sonar.host.url=https://sonarcloud.io" ^
    /o:"vividos-github" ^
    /d:"sonar.login=%SONARLOGIN%"
if errorlevel 1 goto end

REM
REM Rebuild Release|Win32
REM
build-wrapper-win-x86-64.exe --out-dir bw-output msbuild RemotePhotoTool.sln /m /property:Configuration=SonarCloud,Platform=Win32 /target:Rebuild

SonarScanner.MSBuild.exe end /d:"sonar.login=%SONARLOGIN%"

:end

pause
