set PATH=%PATH%;D:\devel\tools\cov-analysis-win64-7.0.2\bin
cov-build --dir cov-int msbuild RemotePhotoTool.sln /property:Configuration=Release /target:Rebuild
"C:\Program Files\7-Zip\7z.exe" a cov-int.zip .\cov-int
