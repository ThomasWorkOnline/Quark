@echo off
pushd %~dp0\..\..\

set glslc="C:/VulkanSDK/1.3.216.0/Bin/glslc.exe"
set flags=-o
set outputDir=.\bin-spirv

if exist %outputDir% (
	echo Are you sure you want to delete the shader bin directory?
	del /s %outputDir%
) else (
	mkdir %outputDir%
)

set searchPath=Quark\assets\shaders
set /a shadersCompiled=0

echo Search path: '%cd%\%searchPath%'

for /r %searchPath% %%i in (*.vert *.frag) do (
	echo Compiling: '%%i' to '%outputDir%\%%~nxi.spv'
	call %glslc% %%i %flags% %outputDir%\%%~nxi.spv
	set /a shadersCompiled=shadersCompiled+1
)

echo.
echo Shaders compiled: %shadersCompiled%

pause
