#include "qkpch.h"
#include "VulkanUtils.h"

namespace Quark {

	namespace Utils {

		void EnumerateVkExtensions()
		{
			uint32_t extensionCount = 0;
			VkResult vkRes = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Error enumerating Vulkan extension properties");

			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkRes = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Error enumerating Vulkan extension properties");

			std::stringstream ss;
			ss << "available extensions:\n";
			for (const auto& extension : extensions)
				ss << '\t' << extension.extensionName << '\n';

			QK_CORE_INFO(ss.str());
		}
	}
}
