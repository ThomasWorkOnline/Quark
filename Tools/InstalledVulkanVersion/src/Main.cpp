#include <cstdio>
#include <cstdlib>

#include <vulkan/vulkan.h>

static int Main(int argc, char** argv)
{
	auto FN_vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion");
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

#if defined(_WIN32)
#include <Windows.h>

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT nShowCmd)
{
	return Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Main(argc, argv);
}

#endif /* _WIN32 */
