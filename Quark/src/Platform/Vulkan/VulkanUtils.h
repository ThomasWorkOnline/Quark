#pragma once

#include "Quark/Core/Core.h"

#if defined(QK_DEBUG) || defined(QK_RELEASE)
#	define QK_ENABLE_VULKAN_VALIDATION_LAYERS
#endif

#define QK_VULKAN_VERBOSE_LOG 0

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <optional>

namespace Quark {

	static constexpr uint32_t g_FramesInFlight = 2;

	static inline const std::vector<const char*> g_ValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	static inline const std::vector<const char*> g_DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	namespace Utils {

		struct QueueFamilyIndices
		{
			std::optional<uint32_t> GraphicsFamily;
			std::optional<uint32_t> PresentFamily;

			bool IsComplete() const { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
		};

		struct SwapChainSupportDetails
		{
			vk::SurfaceCapabilitiesKHR        Capabilities;
			std::vector<vk::SurfaceFormatKHR> Formats;
			std::vector<vk::PresentModeKHR>   PresentModes;
		};

		std::vector<const char*> GetRequiredVkExtensions();
		void EnumerateVkExtensions();
		bool CheckVkValidationLayerSupport();

		QueueFamilyIndices FindVkQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);
		SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);

		bool CheckVkDeviceExtensionSupport(vk::PhysicalDevice device);
		bool IsVkDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface, const QueueFamilyIndices& indices);

		vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
		vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
		vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

		uint32_t GetBufferMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
		vk::Buffer AllocateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::DeviceMemory& bufferMemory);
		void CopyBuffer(vk::Buffer dstBuffer, vk::Buffer srcBuffer, size_t size);
	}
}
