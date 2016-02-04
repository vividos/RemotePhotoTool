REM
REM RemotePhotoTool - remote camera control software
REM Copyright (C) 2008-2016 Michael Fink
REM
REM runs Coverity analysis build
REM

rmdir .\cov-int /s /q 2> nul
del cov-int.zip 2> nul

call "%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\Common7\Tools\vsvars32.bat"
set PATH=%PATH%;D:\devel\tools\cov-analysis-win64-7.7.0.4\bin

cov-build --dir cov-int msbuild RemotePhotoTool.sln /m /property:Configuration=Release /property:Platform=Win32 /target:Rebuild
"%ProgramFiles%\7-Zip\7z.exe" a cov-int.zip .\cov-int

pause
