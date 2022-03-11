set version=1.0

set dest=quark-v%version%

:: Copy Quark header files
xcopy /e ..\Quark\src\Quark\*.h .\%dest%\include\Quark\
xcopy ..\Quark\src\Quark.h .\%dest%\include\

:: Copy header libraries
xcopy /e ..\Quark\vendor\entt\include\* .\%dest%\include\
xcopy /e ..\Quark\vendor\glm\glm\* .\%dest%\include\glm\

:: Copy binaries
xcopy ..\bin\windows-Debug-x86_64\Quark\*.lib .\%dest%\lib\Windows-x64-Debug\
xcopy ..\bin\windows-Release-x86_64\Quark\*.lib .\%dest%\lib\Windows-x64-Release\

pause
