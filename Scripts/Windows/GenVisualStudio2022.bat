@echo off

pushd %~dp0\..\..\
call Vendor\bin\premake5\Windows\premake5.exe vs2022
popd

pause
