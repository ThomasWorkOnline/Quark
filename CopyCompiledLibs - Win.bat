set dest=pre-compiled libs\Windows

xcopy .\bin\Quark\x64-Debug\*.lib ".\%dest%\x64-Debug\"
xcopy .\bin\Quark\x64-Release\*.lib ".\%dest%\x64-Release\"

pause
