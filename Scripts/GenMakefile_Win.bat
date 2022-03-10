pushd %~dp0\..\
call vendor\bin\premake5\premake5.exe gmake2
popd

pause
