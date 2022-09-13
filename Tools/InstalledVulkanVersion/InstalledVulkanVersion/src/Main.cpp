#include <cstdio>
#include <cstdlib>

#include <vulkan/vulkan.h>

int main()
{
	uint32_t ver;
	vkEnumerateInstanceVersion(&ver);

	uint32_t major = VK_API_VERSION_MAJOR(ver);
	uint32_t minor = VK_API_VERSION_MINOR(ver);
	uint32_t patch = VK_API_VERSION_PATCH(ver);

	std::printf("%d.%d.%d", major, minor, patch);

	return EXIT_SUCCESS;
}
