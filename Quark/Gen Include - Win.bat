set src=.
set dest=Quark

:: Copy Quark header files
xcopy /e %src%\src\*.h .\%dest%\include\Quark\
:: Copy dependencies header files
xcopy /e %src%\vendor\*.* .\%dest%\include\vendor\

:: Copy binaries
xcopy /e %src%\bin\x64-Release\*.lib .\%dest%\lib-x64\
xcopy /e %src%\bin\x64-Release\*.dll .\%dest%\lib-x64\

pause
