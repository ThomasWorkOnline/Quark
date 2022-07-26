set outputDir="./bin"
set glslc="C:/VulkanSDK/1.3.216.0/Bin/glslc.exe"

if not exist %outputDir% (
	mkdir %outputDir%
)
	
%glslc% shader.vert -o %outputDir%/shader.vert.spv
%glslc% shader.frag -o %outputDir%/shader.frag.spv

%glslc% sprite.vert -o %outputDir%/sprite.vert.spv
%glslc% sprite.frag -o %outputDir%/sprite.frag.spv

pause
