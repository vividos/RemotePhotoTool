@echo off
REM
REM RemotePhotoTool - remote camera control software
REM Copyright (C) 2008-2018 Michael Fink
REM
REM runs SonarCloud analysis build
REM

call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"

REM
REM Extract SonarQube build tools
REM
cd Thirdparty\SonarQube

"%ProgramFiles%\7-Zip\7z.exe" x -y build-wrapper-win-x86.zip
"%ProgramFiles%\7-Zip\7z.exe" x -y -osonar-scanner-msbuild sonar-scanner-msbuild-3.0.2.656.zip
cd ..\..

PATH=%PATH%;%CD%\Thirdparty\SonarQube\build-wrapper-win-x86;%CD%\Thirdparty\SonarQube\sonar-scanner-msbuild

REM
REM Build using SonarQube scanner for MSBuild
REM
rmdir .\bw-output /s /q 2> nul

msbuild RemotePhotoTool.sln /m /property:Configuration=Release,Platform=Win32 /target:Clean

SonarQube.Scanner.MSBuild.exe begin ^
    /k:"RemotePhotoTool" ^
    /v:"1.6.0" ^
    /d:"sonar.cfamily.build-wrapper-output=%CD%\bw-output" ^
    /d:"sonar.host.url=https://sonarcloud.io" ^
    /d:"sonar.organization=vividos-github" ^
    /d:"sonar.login=%SONARLOGIN%"

build-wrapper-win-x86-64.exe --out-dir bw-output msbuild RemotePhotoTool.sln /m /property:Configuration=Release,Platform=Win32 /target:Build

SonarQube.Scanner.MSBuild.exe end /d:"sonar.login=%SONARLOGIN%"

pause
