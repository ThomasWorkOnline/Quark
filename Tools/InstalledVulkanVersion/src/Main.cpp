#include <cstdio>
#include <cstdlib>

#include "Platform.h"
#include <vulkan/vulkan.h>

#if defined(QK_PLATFORM_WINDOWS)
#	define VULKAN_LIBRARY "vulkan-1.dll"
#elif defined (QK_PLATFORM_APPLE)
#	define VULKAN_LIBRARY "libMoltenVK.dylib"
#else
#	define VULKAN_LIBRARY ""
#	error "Unsupported Platform"
#endif

int Main(int argc, char** argv)
{
	PFN_vkGetInstanceProcAddr FN_vkGetInstanceProcAddr = nullptr;
	if (void* libVulkan = Platform::LoadLibrary(VULKAN_LIBRARY))
	{
		FN_vkGetInstanceProcAddr = Platform::GetProcAddress<PFN_vkGetInstanceProcAddr>(libVulkan, "vkGetInstanceProcAddr");
		Platform::FreeLibrary(libVulkan);
	}

	if (!FN_vkGetInstanceProcAddr)
		return EXIT_FAILURE;

	auto* FN_vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)FN_vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion");
	
	if (FN_vkEnumerateInstanceVersion)
	{
		uint32_t ver = 0;
		FN_vkEnumerateInstanceVersion(&ver);

		uint32_t major = VK_API_VERSION_MAJOR(ver);
		uint32_t minor = VK_API_VERSION_MINOR(ver);
		uint32_t patch = VK_API_VERSION_PATCH(ver);

		std::printf("%d.%d.%d", major, minor, patch);
	}

	return EXIT_SUCCESS;
}
