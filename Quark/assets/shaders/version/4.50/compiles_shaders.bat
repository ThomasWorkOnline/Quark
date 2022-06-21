set outputDir="bin"

mkdir %outputDir%
C:/VulkanSDK/1.3.216.0/Bin/glslc.exe shader.vert -o %outputDir%\vert.spv
C:/VulkanSDK/1.3.216.0/Bin/glslc.exe shader.frag -o %outputDir%\frag.spv
pause
