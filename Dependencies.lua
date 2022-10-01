
VULKAN_SDK = os.getenv("VULKAN_SDK")

VendorDir = "%{wks.location}/Quark/vendor"

IncludeDir = {}
IncludeDir["entt"]       = "%{VendorDir}/entt/single_include"
IncludeDir["FreeType"]   = "%{VendorDir}/freetype/include"
IncludeDir["Glad"]       = "%{VendorDir}/glad/include"
IncludeDir["glfw"]       = "%{VendorDir}/glfw/include"
IncludeDir["glm"]        = "%{VendorDir}/glm"
IncludeDir["ImGui"]      = "%{VendorDir}/imgui"
IncludeDir["Metal"]      = "%{VendorDir}/metal"
IncludeDir["OpenAL"]     = "%{VendorDir}/openal/include"
IncludeDir["spdlog"]     = "%{VendorDir}/spdlog/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["OpenAL"]     = "%{VendorDir}/openal/libs/Win64"
LibraryDir["VulkanSDK"]  = "%{VULKAN_SDK}/Lib"

Library = {}
Library["DXGI"]          = "DXGI.lib"
Library["Direct3D12"]    = "D3D12.lib"
Library["OpenAL"]        = "OpenAL32.lib"
Library["OpenGL"]        = "OpenGL32.lib"
Library["Vulkan"]        = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

Library["ShaderC_Debug"]           = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRVCross_Debug"]        = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRVCross_GLSL_Debug"]   = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"

Library["ShaderC_Release"]         = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRVCross_Release"]      = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRVCross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

filter "system:windows"
