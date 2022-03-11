pushd %~dp0\..\
call vendor\bin\premake5\Windows\premake5.exe vs2022
popd

pause
