set src=.
set dest=pre-compiled libs\Windows

xcopy %src%\bin\x64-Debug\*.lib ".\%dest%\x64-Debug\"
xcopy %src%\bin\x64-Release\*.lib ".\%dest%\x64-Release\"

pause
