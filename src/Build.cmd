REM
REM RemotePhotoTool - remote camera control software
REM Copyright (C) 2008-2022 Michael Fink
REM
REM Builds the software
REM

call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
msbuild RemotePhotoTool.sln /m /property:Configuration=Release /property:Platform=Win32 /target:Rebuild
pause
