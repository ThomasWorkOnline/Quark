pushd %~dp0\..\..\
call vendor\bin\premake5\Windows\premake5.exe gmake2
popd

pause
