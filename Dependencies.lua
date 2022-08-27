
VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["entt"]       = "%{wks.location}/Quark/vendor/entt/single_include"
IncludeDir["freetype"]   = "%{wks.location}/Quark/vendor/freetype/include"
IncludeDir["glad"]       = "%{wks.location}/Quark/vendor/glad/include"
IncludeDir["glfw"]       = "%{wks.location}/Quark/vendor/glfw/include"
IncludeDir["glm"]        = "%{wks.location}/Quark/vendor/glm"
IncludeDir["lodepng"]    = "%{wks.location}/Quark/vendor/lodepng"
IncludeDir["Metal"]      = "%{wks.location}/Quark/vendor/metal"
IncludeDir["OpenAL"]     = "%{wks.location}/Quark/vendor/openal/include"
IncludeDir["spdlog"]     = "%{wks.location}/Quark/vendor/spdlog/include"
IncludeDir["stb"]        = "%{wks.location}/Quark/vendor/stb"
IncludeDir["VulkanSDK"]  = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["OpenAL"]    = "%{wks.location}/Quark/vendor/openal/libs/Win64"
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["OpenAL"] = "OpenAL32.lib"
Library["OpenGL"] = "OpenGL32.lib"
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

Library["ShaderC_Debug"]         = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRVCross_Debug"]      = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRVCross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"

Library["ShaderC_Release"]         = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRVCross_Release"]      = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRVCross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
