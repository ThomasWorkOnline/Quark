#include "qkpch.h"
#include "VulkanContext.h"

#include "VulkanUtils.h"
#include "VulkanEnums.h"

///////////////////////////////////////
/// Vulkan API customizations
///
#define QK_VULKAN_DEBUG_UTILS_VERBOSE 0


#if QK_ASSERT_API_VALIDATION_ERRORS
#	define QK_VULKAN_ERROR_CALLBACK(message) QK_CORE_ASSERT(false, message)
#else
#	define QK_VULKAN_ERROR_CALLBACK(message) QK_CORE_ERROR(message)
#endif

#if QK_ENABLE_VULKAN_VALIDATION_LAYERS
#	define AssureDebugValidationLayerSupport ::Quark::AssureValidationLayerSupport
#	define CreateDebugUtilsMessengerEXT ::Quark::CreateVkDebugUtilsMessengerEXT
#	define DestroyDebugUtilsMessengerEXT ::Quark::DestroyVkDebugUtilsMessengerEXT
#else
#	define AssureDebugValidationLayerSupport(...)
#	define CreateDebugUtilsMessengerEXT(...) VK_SUCCESS
#	define DestroyDebugUtilsMessengerEXT(...)
#endif

#if QK_VULKAN_DEBUG_UTILS_VERBOSE
#	define _VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
#	define _VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
#else
#	define _VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT 0
#	define _VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 0
#endif

namespace Quark {

	namespace Utils {

		static ColorFormat VulkanFormatToColorFormat(VkFormat format)
		{
			switch (format)
			{
				case VK_FORMAT_R8G8B8_UNORM:        return ColorFormat::RGB8;
				case VK_FORMAT_R16G16B16_UNORM:     return ColorFormat::RGB16;
				case VK_FORMAT_R32G32B32_UINT:      return ColorFormat::RGB32UInt;

				case VK_FORMAT_R8G8B8A8_UNORM:      return ColorFormat::RGBA8;
				case VK_FORMAT_R16G16B16A16_UNORM:  return ColorFormat::RGBA16;

				case VK_FORMAT_B8G8R8A8_UNORM:      return ColorFormat::None; // TODO: implement color format

				case VK_FORMAT_R8G8B8_SRGB:         return ColorFormat::RGB8SRGB;
				case VK_FORMAT_R8G8B8A8_SRGB:       return ColorFormat::RGBA8SRGB;

				case VK_FORMAT_B8G8R8_SRGB:         return ColorFormat::BGR8SRGB;
				case VK_FORMAT_B8G8R8A8_SRGB:       return ColorFormat::BGRA8SRGB;

				case VK_FORMAT_R16G16B16_SFLOAT:    return ColorFormat::RGB16f;
				case VK_FORMAT_R32G32B32_SFLOAT:    return ColorFormat::RGB32f;
				case VK_FORMAT_R16G16B16A16_SFLOAT: return ColorFormat::RGBA16f;
				case VK_FORMAT_R32G32B32A32_SFLOAT: return ColorFormat::RGBA32f;

				case VK_FORMAT_R8_UNORM:            return ColorFormat::Red8;

				case VK_FORMAT_D16_UNORM:           return ColorFormat::Depth16;
				case VK_FORMAT_D24_UNORM_S8_UINT:   return ColorFormat::Depth24Stencil8;

				case VK_FORMAT_D32_SFLOAT:          return ColorFormat::Depth32f;

				QK_ASSERT_NO_DEFAULT("Unknown VkFormat");
			}

			return ColorFormat::None;
		}
	}

	static void AssureValidationLayerSupport()
	{
		QK_PROFILE_FUNCTION();

		uint32_t layerCount;
		VkResult vkRes = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Error enumerating Vulkan validation layer properties");

		AutoRelease<VkLayerProperties> availableLayers = StackAlloc(layerCount * sizeof(VkLayerProperties));
		vkRes = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Error enumerating Vulkan validation layer properties");

		for (const char* layerName : g_ValidationLayers)
		{
			bool layerFound = false;
			for (size_t i = 0; i < layerCount; i++)
			{
				if (std::strcmp(layerName, availableLayers[i].layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			QK_CORE_ASSERT(layerFound, "Validation layer not found: {0}", layerName);
		}
	}

	static VkResult CreateVkDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		return func ? func(instance, pCreateInfo, pAllocator, pDebugMessenger) : VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	static void DestroyVkDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		QK_CORE_ASSERT(func != nullptr, "Could not load function vkDestroyDebugUtilsMessengerEXT");
		func(instance, debugMessenger, pAllocator);
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
			return VK_FALSE;

		switch (messageSeverity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   QK_VULKAN_ERROR_CALLBACK(pCallbackData->pMessage); return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: QK_CORE_WARN(pCallbackData->pMessage);             return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    QK_CORE_INFO(pCallbackData->pMessage);             return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: QK_CORE_TRACE(pCallbackData->pMessage);            return VK_FALSE;

			QK_ASSERT_NO_DEFAULT("Unknown severity level in Vulkan message callback");
		}

		return VK_FALSE;
	}

	static VkDebugUtilsMessengerCreateInfoEXT CreateVkDebugUtilsMessengerCreateInfoEXT()
	{
		VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
		messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		messengerCreateInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| _VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
			| _VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

		messengerCreateInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		messengerCreateInfo.pfnUserCallback = VkDebugCallback;
		return messengerCreateInfo;
	}

	VulkanContext::~VulkanContext()
	{
		QK_PROFILE_FUNCTION();

		m_Device.reset();

		DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanContext::WaitUntilDeviceIdle()
	{
		m_Device->WaitUntilIdle();
	}

	uint32_t VulkanContext::QuerySwapChainImageCount() const
	{
		auto& supportDetails = m_Device->GetSupportDetails();
		uint32_t imageCount = supportDetails.Capabilities.minImageCount + 1;
		if (imageCount > supportDetails.Capabilities.maxImageCount)
			imageCount = supportDetails.Capabilities.maxImageCount;

		return imageCount;
	}

	SwapSurfaceFormat VulkanContext::ChooseSurfaceFormat(SwapSurfaceFormat preferred) const
	{
		VkFormat targetFormat = DataFormatToVulkan(preferred.Format);
		VkColorSpaceKHR targetColorSpace = (VkColorSpaceKHR)preferred.ColorSpace;

		auto& availableFormats = m_Device->GetSupportDetails().Formats;
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == targetFormat && availableFormat.colorSpace == targetColorSpace)
			{
				SwapSurfaceFormat selectedFormat{};
				selectedFormat.ColorSpace = (ColorSpace)availableFormat.colorSpace;
				selectedFormat.Format = Utils::VulkanFormatToColorFormat(availableFormat.format);
				return selectedFormat;
			}
		}

		QK_CORE_WARN("Swap surface format not found: fallback to default format");

		SwapSurfaceFormat defaultFormat{};
		defaultFormat.ColorSpace = (ColorSpace)availableFormats[0].colorSpace;
		defaultFormat.Format = Utils::VulkanFormatToColorFormat(availableFormats[0].format);
		return defaultFormat;
	}

	SwapPresentMode VulkanContext::ChooseSwapPresentMode(SwapPresentMode preferred) const
	{
		auto& availablePresentModes = m_Device->GetSupportDetails().PresentModes;
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == (VkPresentModeKHR)preferred)
				return (SwapPresentMode)availablePresentMode;
		}

		QK_CORE_WARN("Swap present mode not found: fallback to default FIFO present mode");
		return (SwapPresentMode)VK_PRESENT_MODE_FIFO_KHR;
	}

	ViewportExtent VulkanContext::ChooseSwapExtent(uint32_t width, uint32_t height) const
	{
		auto& capabilities = m_Device->GetSupportDetails().Capabilities;
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return { capabilities.currentExtent.width, capabilities.currentExtent.height };
		}
		else
		{
			ViewportExtent extent = { width, height };
			extent.Width = std::clamp(extent.Width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			extent.Height = std::clamp(extent.Height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			return extent;
		}
	}

	void VulkanContext::CreateInstance(const char* appName, std::span<const char*> extensions)
	{
		QK_PROFILE_FUNCTION();

		Utils::EnumerateVkExtensions();
		AssureDebugValidationLayerSupport();

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Quark Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Quark Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

#if QK_ENABLE_VULKAN_VALIDATION_LAYERS
		auto messengerCreateInfo = CreateVkDebugUtilsMessengerCreateInfoEXT();

		createInfo.enabledLayerCount = sizeof_array(g_ValidationLayers);
		createInfo.ppEnabledLayerNames = g_ValidationLayers;
		createInfo.pNext = &messengerCreateInfo;
#endif
		VkResult vkRes = vkCreateInstance(&createInfo, nullptr, &m_Instance);
		Verify(vkRes == VK_SUCCESS, "Failed to create the Vulkan instance (code: {0})", vkRes);

		vkRes = CreateDebugUtilsMessengerEXT(m_Instance, &messengerCreateInfo, nullptr, &m_DebugMessenger);
		Verify(vkRes == VK_SUCCESS, "Failed to create a Vulkan debug messenger (code: {0})", vkRes);
	}
}
