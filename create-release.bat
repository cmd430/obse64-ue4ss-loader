@echo off
"C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\msbuild" "%~dp0/vsxmake2022/ue4ss_Loader.sln" /property:Configuration=releasedbg
rd /s /q "%~dp0/dist"
mkdir "%~dp0/dist/OblivionRemastered/Binaries/Win64"
robocopy "%~dp0/build/windows/x64/releasedbg" "%~dp0/dist/OblivionRemastered/Binaries/Win64" ue4ss_Loader.pdb ue4ss_Loader.dll
cd "%~dp0/dist"
tar.exe acvf "ue4ss_Loader.zip" "OblivionRemastered"
rd /s /q "%~dp0/dist/OblivionRemastered"
