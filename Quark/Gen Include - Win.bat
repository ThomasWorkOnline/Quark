set version=1.0

set src=.
set dest=quark-v%version%

:: Copy Quark header files
xcopy /e %src%\src\*.h .\%dest%\include\QK\

:: Copy binaries
xcopy /e %src%\bin\x64-Release\*.lib .\%dest%\lib-x64\
xcopy /e %src%\bin\x64-Release\*.dll .\%dest%\lib-x64\

pause
