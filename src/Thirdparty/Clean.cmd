REM
REM RemotePhotoTool - remote camera control software
REM Copyright (C) 2008-2016 Michael Fink
REM
REM Clean script for Thirdparty libraries
REM

set OUTDIR=%1

REM CDSDK
del %OUTDIR%cdDcd.dll 2> nul
del %OUTDIR%cdDvlp.dll 2> nul
del %OUTDIR%cdParse.dll 2> nul
del %OUTDIR%CDPROC.exe 2> nul
del %OUTDIR%CDPROCMN.exe 2> nul
del %OUTDIR%CDPTPCLS.dll 2> nul
del %OUTDIR%CDPTPCLT.dll 2> nul
del %OUTDIR%CDRAPCLS.dll 2> nul
del %OUTDIR%CDSDK.dll 2> nul
del %OUTDIR%EWatch.exe 2> nul
del %OUTDIR%GetPicture.exe 2> nul
del %OUTDIR%IWrap.dll 2> nul
del %OUTDIR%psCamDat.dll 2> nul

REM EDSDK
del %OUTDIR%EDSDK.dll 2> nul
del %OUTDIR%EDSDK_25.dll 2> nul
del %OUTDIR%EdsImage.dll 2> nul

REM PSREC
del %OUTDIR%PRLIB.dll 2> nul
del %OUTDIR%PRSDK.dll 2> nul

REM Scintilla
del %OUTDIR%SciLexer.dll 2> nul
del %OUTDIR%lua.properties 2> nul

REM gPhoto2 files
del %OUTDIR%libgphoto2.dll 2> nul
del %OUTDIR%libgphoto2_port.dll 2> nul
del %OUTDIR%camlibs\canon.dll 2> nul
del %OUTDIR%camlibs\ptp2.dll 2> nul
del %OUTDIR%iolibs\disk.dll 2> nul
del %OUTDIR%libusb-1.0.dll 2> nul
del %OUTDIR%iolibs\libusb1.dll 2> nul
del %OUTDIR%iolibs\ptpip.dll 2> nul
rmdir %OUTDIR%camlibs 2> nul
rmdir %OUTDIR%iolibs 2> nul

exit 0
