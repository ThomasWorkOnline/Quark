#include "qkpch.h"
#include "VulkanGraphicsContext.h"

#include <set>
#include <sstream>

namespace Quark {

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
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:	  QK_CORE_ERROR(pCallbackData->pMessage); return VK_FALSE;
		}

		QK_CORE_FATAL("Unknown severity level in Vulkan message callback");
		return VK_FALSE;
	}

	namespace Utils {

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
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
				| vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
#if QK_VULKAN_VERBOSE_LOG
				| vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
				| vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
#endif
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

	VulkanGraphicsContext::VulkanGraphicsContext(void* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
		QK_CORE_ASSERT(windowHandle, "Window handle is nullptr");
	}

	VulkanGraphicsContext::~VulkanGraphicsContext()
	{
		QK_PROFILE_FUNCTION();

		m_SwapChain.reset();
		m_VkDevice.destroy();

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		Utils::DestroyVkDebugUtilsMessengerEXT(m_VkInstance, m_VkDebugMessenger, nullptr);
#endif
		vkDestroySurfaceKHR(m_VkInstance, m_VkSurface, nullptr);
		m_VkInstance.destroy();

		// Detaching the current context
		//glfwMakeContextCurrent(nullptr);
	}

	void VulkanGraphicsContext::Init()
	{
		QK_PROFILE_FUNCTION();

		// Instance creation
		{
			QK_PROFILE_SCOPE(VulkanGraphicsContext::InstanceCreation);

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

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
			auto vkMessengerCreateInfo = Utils::CreateVkDebugUtilsMessengerCreateInfoEXT();

			vkCreateInfo.setEnabledLayerCount(static_cast<uint32_t>(g_ValidationLayers.size()));
			vkCreateInfo.setPpEnabledLayerNames(g_ValidationLayers.data());
			vkCreateInfo.setPNext(&vkMessengerCreateInfo);
#endif

			m_VkInstance = vk::createInstance(vkCreateInfo);

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
			vk::Result vkRes = Utils::CreateVkDebugUtilsMessengerEXT(m_VkInstance, &vkMessengerCreateInfo, nullptr, &m_VkDebugMessenger);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Failed to create a Vulkan debug messenger");
#endif
		}

		// Window surface creation
		glfwCreateWindowSurface(m_VkInstance, m_WindowHandle, nullptr, (vk::SurfaceKHR::CType*)&m_VkSurface);

		// Device creation
		Utils::QueueFamilyIndices familyIndices;

		{
			QK_PROFILE_SCOPE(VulkanGraphicsContext::DeviceCreation);

			uint32_t deviceCount = 0;
			vk::Result vkRes = m_VkInstance.enumeratePhysicalDevices(&deviceCount, nullptr);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not enumerate physical devices");
			QK_CORE_ASSERT(deviceCount, "No physical graphics devices found");

			std::vector<vk::PhysicalDevice> devices(deviceCount);
			vkRes = m_VkInstance.enumeratePhysicalDevices(&deviceCount, devices.data());
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not enumerate physical devices");

			for (const auto& device : devices)
			{
				auto indices = Utils::FindVkQueueFamilies(device, m_VkSurface);
				if (Utils::IsVkSwapChainSuitable(device, m_VkSurface, indices))
				{
					m_VkPhysicalDevice = device;
					familyIndices = indices;
					break;
				}
			}

			QK_CORE_ASSERT(m_VkPhysicalDevice, "Failed to find a suitable graphics device");

			std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
			std::set<uint32_t> uniqueQueueFamilies = {
				*familyIndices.GraphicsFamily,
				*familyIndices.PresentFamily
			};

			float queuePriorities = 1.0f;
			for (uint32_t queueFamily : uniqueQueueFamilies)
			{
				vk::DeviceQueueCreateInfo queueCreateInfo;
				queueCreateInfo.setQueueFamilyIndex(queueFamily);
				queueCreateInfo.setQueueCount(1);
				queueCreateInfo.setPQueuePriorities(&queuePriorities);
				queueCreateInfos.push_back(queueCreateInfo);
			}

			vk::PhysicalDeviceFeatures deviceFeatures;

			vk::DeviceCreateInfo createInfo;
			createInfo.setQueueCreateInfoCount(static_cast<uint32_t>(queueCreateInfos.size()));
			createInfo.setPQueueCreateInfos(queueCreateInfos.data());
			createInfo.setPEnabledFeatures(&deviceFeatures);

			createInfo.setEnabledExtensionCount(static_cast<uint32_t>(g_DeviceExtensions.size()));
			createInfo.setPpEnabledExtensionNames(g_DeviceExtensions.data());

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
			createInfo.setEnabledLayerCount(static_cast<uint32_t>(g_ValidationLayers.size()));
			createInfo.setPpEnabledLayerNames(g_ValidationLayers.data());
#endif

			m_VkDevice = m_VkPhysicalDevice.createDevice(createInfo, nullptr);
			m_VkPresentQueue = m_VkDevice.getQueue(*familyIndices.PresentFamily, 0);
		}

		// Swap chain creation
		{
			Utils::SwapChainSupportDetails swapChainSupport = Utils::QuerySwapChainSupport(m_VkPhysicalDevice, m_VkSurface);

			uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
			if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
				imageCount = swapChainSupport.Capabilities.maxImageCount;

			VulkanSwapChainSpecification scSpec;
			scSpec.Extent = Utils::ChooseSwapExtent(swapChainSupport.Capabilities, m_WindowHandle);
			scSpec.SurfaceFormat = Utils::ChooseSwapSurfaceFormat(swapChainSupport.Formats);
			scSpec.PresentMode = Utils::ChooseSwapPresentMode(swapChainSupport.PresentModes);
			scSpec.FamilyIndices = familyIndices;
			scSpec.ImageCount = imageCount;

			m_SwapChain = CreateScope<VulkanSwapChain>(m_VkDevice, m_VkSurface, scSpec);
		}

		//glfwMakeContextCurrent(m_WindowHandle);
		//QK_CORE_TRACE("Created Vulkan graphics context!");
	}

	void VulkanGraphicsContext::SwapBuffers()
	{
		m_SwapChain->SwapBuffers();
	}
}
