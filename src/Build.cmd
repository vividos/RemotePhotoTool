REM
REM RemotePhotoTool - remote camera control software
REM Copyright (C) 2008-2016 Michael Fink
REM
REM Builds the software
REM

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\vsvars32.bat"
msbuild RemotePhotoTool.sln /m /property:Configuration=Release /property:Platform=Win32 /target:Rebuild
pause
