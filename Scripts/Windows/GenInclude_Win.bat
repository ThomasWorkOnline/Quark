pushd %~dp0\..\..\

:: Copy Quark header files
xcopy /e .\Quark\src\Quark\*.h .\Quark\include\Quark\
xcopy .\Quark\src\Quark.h .\Quark\include\

:: Copy binaries
xcopy .\bin\windows-Debug-x86_64\Quark\*.lib .\Quark\lib\windows-x64-Debug\
xcopy .\bin\windows-Release-x86_64\Quark\*.lib .\Quark\lib\windows-x64-Release\
xcopy .\bin\windows-Dist-x86_64\Quark\*.lib .\Quark\lib\windows-x64-Dist\

popd
pause
