@ echo off

del /s /q ..\Build
rd /s /q ..\Build
mkdir ..\Build

rem @ set PATH=D:\Program Files\cmake-3.16.3-win64-x64\bin;%PATH%

cmake -B ..\Build -S . -G "Visual Studio 16 2019" -A x64 -D USE_DX11=1

pause