#include "qkpch.h"
#include "VulkanContext.h"

#include "VulkanSwapChain.h"

#include <fstream>
#include <set>
#include <sstream>

#include <GLFW/glfw3.h>

namespace Quark {

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
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: QK_CORE_TRACE(pCallbackData->pMessage); return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    QK_CORE_INFO(pCallbackData->pMessage);  return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: QK_CORE_WARN(pCallbackData->pMessage);  return VK_FALSE;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   QK_CORE_FATAL(pCallbackData->pMessage); return VK_FALSE;
			default:
				QK_CORE_FATAL("Unknown severity level in Vulkan message callback");
				return VK_FALSE;
		}

		return VK_FALSE;
	}

	namespace Utils {

		static bool CheckVkValidationLayerSupport()
		{
			uint32_t layerCount;
			VkResult vkRes = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Error enumerating Vulkan validation layer properties");

			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkRes = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Error enumerating Vulkan validation layer properties");

			for (const char* layerName : g_ValidationLayers)
			{
				bool layerFound = false;
				for (const auto& layerProperties : availableLayers)
				{
					if (std::strcmp(layerName, layerProperties.layerName) == 0)
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

		static VkSurfaceKHR CreateSurfaceForPlatform(VkInstance instance, GLFWwindow* window)
		{
			VkSurfaceKHR surface;
			glfwCreateWindowSurface(instance, window, nullptr, &surface);
			return surface;
		}

		static VkResult CreateVkDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger)
		{
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

			if (func != nullptr)
				return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
			else
				return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		static void DestroyVkDebugUtilsMessengerEXT(VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

			QK_CORE_ASSERT(func != nullptr, "Could not load function vkDestroyDebugUtilsMessengerEXT");
			func(instance, debugMessenger, pAllocator);
		}

		static VkDebugUtilsMessengerCreateInfoEXT CreateVkDebugUtilsMessengerCreateInfoEXT()
		{
			VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
			messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			messengerCreateInfo.messageSeverity =
				  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
#if QK_VULKAN_VERBOSE_LOG
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
#endif
				;

			messengerCreateInfo.messageType =
				  VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

			messengerCreateInfo.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)VkDebugCallback;
			return messengerCreateInfo;
		}
	}

	VulkanContext::VulkanContext(void* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
		QK_CORE_ASSERT(windowHandle, "Window handle is nullptr");
	}

	VulkanContext::~VulkanContext()
	{
		QK_PROFILE_FUNCTION();

		m_Device->WaitIdle();

		m_SwapChain.reset();
		m_Device.reset();

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		Utils::DestroyVkDebugUtilsMessengerEXT(m_Instance, m_VkDebugMessenger, nullptr);
#endif
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanContext::Init()
	{
		QK_PROFILE_FUNCTION();

		QK_PROFILE_FUNCTION();

		// Instance creation
		{
			QK_PROFILE_SCOPE(VulkanContext::Instance);

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
			QK_CORE_ASSERT(Utils::CheckVkValidationLayerSupport(), "Some Vulkan validation layers are not supported");
#endif

			VkApplicationInfo appInfo{};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Quark App";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "Quark Engine";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_0;

			VkInstanceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			auto extensions = Utils::GetRequiredVkExtensions();
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
			auto messengerCreateInfo = Utils::CreateVkDebugUtilsMessengerCreateInfoEXT();

			createInfo.enabledLayerCount = sizeof_array(g_ValidationLayers);
			createInfo.ppEnabledLayerNames = g_ValidationLayers;
			createInfo.pNext = &messengerCreateInfo;
#endif
			VkResult vkRes = vkCreateInstance(&createInfo, nullptr, &m_Instance);
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Failed to create the Vulkan instance");

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
			vkRes = Utils::CreateVkDebugUtilsMessengerEXT(m_Instance, &messengerCreateInfo, nullptr, &m_VkDebugMessenger);
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Failed to create a Vulkan debug messenger");
#endif
		}

		// Window surface creation
		m_Surface = Utils::CreateSurfaceForPlatform(m_Instance, m_WindowHandle);

		// Device creation
		m_Device = VulkanDevice::CreateDefaultForSurface(m_Instance, m_Surface);

		// Swap chain creation
		m_SwapChain = CreateScope<VulkanSwapChain>(m_WindowHandle, m_Surface);

		QK_CORE_TRACE("Created Vulkan graphics context!");
	}

	void VulkanContext::SwapBuffers()
	{
		m_SwapChain->Present();
	}
}
