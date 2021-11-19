set version=1.0

set dest=quark-v%version%

:: Copy Quark header files
xcopy /e .\Quark\src\*.h .\%dest%\include\

:: Copy binaries
xcopy .\bin\windows-Debug-x86_64\Quark\*.lib .\%dest%\lib\Windows-x64-Debug\
xcopy .\bin\windows-Release-x86_64\Quark\*.lib .\%dest%\lib\Windows-x64-Release\

pause
