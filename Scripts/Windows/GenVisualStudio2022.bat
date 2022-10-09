@echo off

pushd %~dp0\..\..\
call Vendor\Bin\premake5\Windows\premake5.exe vs2022
popd

pause
