#include "qkpch.h"
#include "VulkanGraphicsAPI.h"

#include <GLFW/glfw3.h>
#include <sstream>

namespace Quark {

	static const std::vector<const char*> s_ValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: QK_CORE_TRACE(pCallbackData->pMessage); return VK_FALSE;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:	  QK_CORE_INFO(pCallbackData->pMessage);  return VK_FALSE;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: QK_CORE_WARN(pCallbackData->pMessage);  return VK_FALSE;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:	  QK_CORE_FATAL(pCallbackData->pMessage); return VK_FALSE;
		}

		QK_CORE_FATAL("Unknown severity level in Vulkan message callback");
		return VK_FALSE;
	}

	namespace Utils {

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

			for (const char* layerName : s_ValidationLayers)
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

		static vk::Result CreateVkDebugUtilsMessengerEXT(
			vk::Instance instance,
			const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const vk::AllocationCallbacks* pAllocator,
			vk::DebugUtilsMessengerEXT* pDebugMessenger)
		{
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)instance.getProcAddr("vkCreateDebugUtilsMessengerEXT");

			if (func != nullptr)
				return (vk::Result)func((VkInstance)instance, (const VkDebugUtilsMessengerCreateInfoEXT*)pCreateInfo, (const VkAllocationCallbacks*)pAllocator, (VkDebugUtilsMessengerEXT*)pDebugMessenger);
			else
				return vk::Result::eErrorExtensionNotPresent;
		}

		static void DestroyVkDebugUtilsMessengerEXT(vk::Instance instance,
			vk::DebugUtilsMessengerEXT debugMessenger,
			const vk::AllocationCallbacks* pAllocator)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT");

			QK_CORE_ASSERT(func != nullptr, "Could not load function vkDestroyDebugUtilsMessengerEXT");
			func((VkInstance)instance, (VkDebugUtilsMessengerEXT)debugMessenger, (const VkAllocationCallbacks*)pAllocator);
		}

		static vk::DebugUtilsMessengerCreateInfoEXT CreateVkDebugUtilsMessengerCreateInfoEXT()
		{
			vk::DebugUtilsMessengerCreateInfoEXT vkMessengerCreateInfo;
			vkMessengerCreateInfo.setMessageSeverity(
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
			);

			vkMessengerCreateInfo.setMessageType(
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
				vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
				vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
			);

			vkMessengerCreateInfo.setPfnUserCallback(VkDebugCallback);
			return vkMessengerCreateInfo;
		}
	}

	VulkanGraphicsAPI::VulkanGraphicsAPI()
	{
#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		QK_CORE_ASSERT(Utils::CheckVkValidationLayerSupport(), "Some Vulkan validation layers are not supported");
#endif

		vk::ApplicationInfo vkAppInfo;
		vkAppInfo.setPApplicationName("Quark App");
		vkAppInfo.setPEngineName("Quark Engine");

		vk::InstanceCreateInfo vkCreateInfo;
		vkCreateInfo.setPApplicationInfo(&vkAppInfo);

		auto extensions = Utils::GetRequiredVkExtensions();
		vkCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
		vkCreateInfo.setPpEnabledExtensionNames(extensions.data());

		{
#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS

			auto vkMessengerCreateInfo = Utils::CreateVkDebugUtilsMessengerCreateInfoEXT();

			vkCreateInfo.setEnabledLayerCount(static_cast<uint32_t>(s_ValidationLayers.size()));
			vkCreateInfo.setPpEnabledLayerNames(s_ValidationLayers.data());
			vkCreateInfo.setPNext(&vkMessengerCreateInfo);
#endif
		}

		m_VkInstance = vk::createInstance(vkCreateInfo);

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		auto vkMessengerCreateInfo = Utils::CreateVkDebugUtilsMessengerCreateInfoEXT();

		vk::Result vkRes = Utils::CreateVkDebugUtilsMessengerEXT(m_VkInstance, &vkMessengerCreateInfo, nullptr, &m_VkDebugMessenger);
		QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Failed to create a Vulkan debug messenger");
#endif
	}

	VulkanGraphicsAPI::~VulkanGraphicsAPI()
	{
#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		Utils::DestroyVkDebugUtilsMessengerEXT(m_VkInstance, m_VkDebugMessenger, nullptr);
#endif

		m_VkInstance.destroy();
	}
}
