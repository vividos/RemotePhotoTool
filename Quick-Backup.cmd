REM 7z.exe {command} file.zip {options}
REM -r recurse
REM -x!{wildchard} exclude

"C:\Program Files\7-Zip\7z.exe" a RemotePhotoTool-backup.zip .\* -r -x!.git\* -x!bin\* -x!intermediate\* -x!lib\* -x!src\ipch\* -x!downloads\* -x!stuff\* -x!src\temp\* -x!*.ncb -x!*.sdf -x!*.suo -x!*.user

pause
