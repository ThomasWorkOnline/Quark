#pragma once

#include "Quark/Core/Core.h"

#if defined(QK_DEBUG) || defined (QK_RELEASE)
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

		static std::vector<const char*> GetRequiredVkExtensions()
		{
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
			return extensions;
		}

		static void EnumerateVkExtensions()
		{
			uint32_t extensionCount = 0;
			vk::Result vkRes = vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Error enumerating Vulkan extension properties");

			std::vector<vk::ExtensionProperties> extensions(extensionCount);
			vkRes = vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Error enumerating Vulkan extension properties");

			std::stringstream ss;
			ss << "available extensions:\n";
			for (const auto& extension : extensions)
				ss << '\t' << extension.extensionName << '\n';

			QK_CORE_INFO(ss.str());
		}

		static bool CheckVkValidationLayerSupport()
		{
			uint32_t layerCount;
			vk::Result vkRes = vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Error enumerating Vulkan validation layer properties");

			std::vector<vk::LayerProperties> availableLayers(layerCount);
			vkRes = vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Error enumerating Vulkan validation layer properties");

			for (const char* layerName : g_ValidationLayers)
			{
				bool layerFound = false;
				for (const auto& layerProperties : availableLayers)
				{
					if (strcmp(layerName, layerProperties.layerName) == 0)
					{
						layerFound = true;
						break;
					}
				}

				if (!layerFound)
					return false;
			}

			return true;
		}

		static bool CheckVkDeviceExtensionSupport(vk::PhysicalDevice device)
		{
			uint32_t extensionCount;
			vk::Result vkRes = device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not enumerate device extension properties");

			std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
			vkRes = device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not enumerate device extension properties");

			for (const char* extensionName : g_DeviceExtensions)
			{
				bool extensionFound = false;
				for (const auto& extension : availableExtensions)
				{
					if (strcmp(extensionName, extension.extensionName) == 0)
					{
						extensionFound = true;
						break;
					}
				}

				if (!extensionFound)
					return false;
			}

			return true;
		}

		static QueueFamilyIndices FindVkQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface)
		{
			uint32_t queueFamilyCount = 0;
			device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

			std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
			device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

			QueueFamilyIndices indices;

			uint32_t i = 0;
			for (const auto& queueFamily : queueFamilies)
			{
				if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
					indices.GraphicsFamily = i;

				vk::Bool32 presentSupport = device.getSurfaceSupportKHR(i, surface);

				if (presentSupport)
					indices.PresentFamily = i;

				if (indices.IsComplete())
					break;

				i++;
			}

			return indices;
		}

		static SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface)
		{
			SwapChainSupportDetails details;
			details.Capabilities = device.getSurfaceCapabilitiesKHR(surface);

			uint32_t formatCount;
			vk::Result vkRes = device.getSurfaceFormatsKHR(surface, &formatCount, nullptr);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not query surface formats");

			if (formatCount != 0)
			{
				details.Formats.resize(formatCount);
				vkRes = device.getSurfaceFormatsKHR(surface, &formatCount, details.Formats.data());
				QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not query surface formats");
			}

			uint32_t presentModeCount;
			vkRes = device.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not query surface present modes");

			if (presentModeCount != 0)
			{
				details.PresentModes.resize(presentModeCount);
				vkRes = device.getSurfacePresentModesKHR(surface, &presentModeCount, details.PresentModes.data());
				QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not query surface present modes");
			}

			return details;
		}

		static bool IsVkDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface, const QueueFamilyIndices& indices)
		{
			bool extensionsSupported = CheckVkDeviceExtensionSupport(device);

			bool swapChainAdequate = false;
			if (extensionsSupported)
			{
				SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
				swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
			}

			return indices.IsComplete() && swapChainAdequate;
		}

		static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
		{
			for (const auto& availableFormat : availableFormats)
			{
				if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
					return availableFormat;
			}

			QK_CORE_WARN("Swap surface format not found: using default format 0");
			return availableFormats[0];
		}

		static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
		{
			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == vk::PresentModeKHR::eMailbox)
					return availablePresentMode;
			}

			QK_CORE_WARN("Swap present mode not found: using default FIFO present mode");
			return vk::PresentModeKHR::eFifo;
		}

		static vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
		{
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return capabilities.currentExtent;
			}
			else
			{
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);

				vk::Extent2D actualExtent = {
					static_cast<uint32_t>(width),
					static_cast<uint32_t>(height)
				};

				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

				return actualExtent;
			}
		}
	}
}
