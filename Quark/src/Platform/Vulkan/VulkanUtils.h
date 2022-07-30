#pragma once

#include "Quark/Core/Core.h"

#if defined(QK_DEBUG) || defined(QK_RELEASE)
#	define QK_ENABLE_VULKAN_VALIDATION_LAYERS
#endif

#define QK_VULKAN_VERBOSE_LOG 0

#include <vulkan/vulkan.hpp>

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	static inline const char* g_ValidationLayers[] = {
		"VK_LAYER_KHRONOS_validation"
	};

	namespace Utils {

		void                     EnumerateVkExtensions();

		VkSurfaceFormatKHR       ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR         ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D               ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

		VkBuffer                 AllocateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory);
		void                     CopyBuffer(VkBuffer dstBuffer, VkBuffer srcBuffer, size_t size);
	}
}
