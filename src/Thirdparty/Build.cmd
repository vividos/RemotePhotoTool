REM
REM RemotePhotoTool - remote camera control software
REM Copyright (C) 2008-2016 Michael Fink
REM
REM Build script for Thirdparty libraries
REM

set OUTDIR=%1

mkdir %OUTDIR% 2> nul
copy CDSDK\Redist\*.* %OUTDIR%
copy EDSDK\dll\*.* %OUTDIR%
copy PSREC\PSReCSDK\redist\*.* %OUTDIR%
copy Scintilla\bin\*.* %OUTDIR%
xcopy gphoto2\bin\*.* %OUTDIR% /S /Y
