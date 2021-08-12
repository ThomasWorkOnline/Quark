set version=1.0

set src=.
set dest=quark-v%version%

:: Copy Quark header files
xcopy /e %src%\src\*.h .\%dest%\include\QK\

:: Copy binaries
xcopy %src%\bin\x64-Debug\*.lib .\%dest%\lib\x64-Debug\
xcopy %src%\bin\x64-Release\*.lib .\%dest%\lib\x64-Release\

pause
