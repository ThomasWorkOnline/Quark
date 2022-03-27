set version=1.0
set dest=quark-v%version%

pushd %~dp0\..\..\

:: Copy Quark header files
xcopy /e .\Quark\src\Quark\*.h .\%dest%\include\Quark\
xcopy .\Quark\src\Quark.h .\%dest%\include\

:: Copy header libraries
xcopy /e .\Quark\vendor\entt\* .\%dest%\vendor\entt\
xcopy /e .\Quark\vendor\glm\* .\%dest%\vendor\glm\

:: Copy binaries
xcopy .\bin\windows-Debug-x86_64\Quark\*.lib .\%dest%\lib\windows-x64-Debug\
xcopy .\bin\windows-Release-x86_64\Quark\*.lib .\%dest%\lib\windows-x64-Release\

popd
pause
