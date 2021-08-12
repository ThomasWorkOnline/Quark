set version=1.0

set dest=quark-v%version%

:: Copy Quark header files
xcopy /e .\Quark\src\*.h .\%dest%\include\QK\

:: Copy binaries
xcopy .\bin\Quark\x64-Debug\*.lib .\%dest%\lib\Windows\x64-Debug\
xcopy .\bin\Quark\x64-Release\*.lib .\%dest%\lib\Windows\x64-Release\

pause
