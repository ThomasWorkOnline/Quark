@echo off
pushd %~dp0\..\..\

set inputDir="Quark\assets\shaders"
set outputDir=".\bin-spirv"
set glslc="C:\VulkanSDK\1.3.216.0\Bin\glslc.exe"

if not exist %outputDir% (
	mkdir %outputDir%
)

@echo on

for /R %%i in (%inputDir%\*.vert *.frag) do (
	%glslc% %%i -o %outputDir%/%%~nxi.spv
)

pause
