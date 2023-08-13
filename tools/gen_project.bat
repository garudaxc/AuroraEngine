@ echo off

del /s /q ..\intermediate
rd /s /q ..\intermediate
mkdir ..\intermediate

rem @ set PATH=D:\Program Files\cmake-3.16.3-win64-x64\bin;%PATH%

cmake -B ..\intermediate -S ..\source -G "Visual Studio 16 2019" -A x64 -D USE_DX11=1

pause