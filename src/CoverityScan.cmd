REM
REM RemotePhotoTool - remote camera control software
REM Copyright (C) 2008-2017 Michael Fink
REM
REM runs Coverity analysis build
REM

rmdir .\cov-int /s /q 2> nul
del cov-int.zip 2> nul

call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
set PATH=%PATH%;D:\devel\tools\cov-analysis-win64-8.7.0\bin

cov-build --dir cov-int msbuild RemotePhotoTool.sln /property:Configuration=Release /property:Platform=Win32 /target:Rebuild
"%ProgramFiles%\7-Zip\7z.exe" a cov-int.zip .\cov-int

pause
