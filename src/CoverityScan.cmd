REM
REM RemotePhotoTool - remote camera control software
REM Copyright (C) 2008-2014 Michael Fink
REM
REM runs Coverity analysis build
REM

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat"
set PATH=%PATH%;D:\devel\tools\cov-analysis-win64-7.5.0\bin
cov-build --dir cov-int msbuild RemotePhotoTool.sln /property:Configuration=Release /target:Rebuild
"C:\Program Files\7-Zip\7z.exe" a cov-int.zip .\cov-int
pause
