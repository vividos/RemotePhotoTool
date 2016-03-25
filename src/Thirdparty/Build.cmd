set OUTDIR=%1
mkdir %OUTDIR% 2> nul
copy CDSDK\Redist\*.* %OUTDIR%
copy EDSDK\dll\*.* %OUTDIR%
copy PSREC\PSReCSDK\redist\*.* %OUTDIR%
copy Scintilla\bin\*.* %OUTDIR%
copy gphoto2\bin\*.* %OUTDIR%
