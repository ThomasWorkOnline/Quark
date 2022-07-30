#pragma once

#include "Quark/Core/Core.h"

#if defined(QK_DEBUG) || defined(QK_RELEASE)
#	define QK_ENABLE_VULKAN_VALIDATION_LAYERS
#endif

#define QK_VULKAN_VERBOSE_LOG 0

#include <vulkan/vulkan.hpp>

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	static inline const std::vector<const char*> g_ValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	static inline const std::vector<const char*> g_DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	namespace Utils {

		std::vector<const char*> GetRequiredVkExtensions();
		void                     EnumerateVkExtensions();
		bool                     CheckVkValidationLayerSupport();

		VkSurfaceFormatKHR       ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR         ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D               ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
		VkSurfaceKHR             CreateSurfaceForPlatform(VkInstance instance, GLFWwindow* window);

		VkBuffer                 AllocateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory);
		void                     CopyBuffer(VkBuffer dstBuffer, VkBuffer srcBuffer, size_t size);
	}
}
