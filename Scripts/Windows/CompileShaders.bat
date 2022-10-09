@echo off
pushd %~dp0\..\..\Quark\

if not exist %VULKAN_SDK% (
	echo VulkanSDK was not found, try reinstalling the VulkanSDK using the Setup.py script
	exit
) else (
	echo Located VulkanSDK at %VULKAN_SDK%
)

set glslc=%VULKAN_SDK%\Bin\glslc.exe
set flags=-o
set outputDir=.\cache\spirv

if exist %outputDir% (
	echo Do you want to delete the shader bin cache?
	del /s %outputDir%
) else (
	mkdir %outputDir%
)

set searchPath=assets\shaders
set /a shadersCompiled=0

echo glslc.exe located at %glslc%
echo Search path: '%cd%\%searchPath%'

for /r %searchPath% %%i in (*.vert *.frag) do (
	echo Compiling: '%%i' to '%outputDir%\%%~nxi.spv'
	call %glslc% %%i %flags% %outputDir%\%%~nxi.spv
	set /a shadersCompiled=shadersCompiled+1
)

echo.
echo Shaders compiled: %shadersCompiled%

pause
