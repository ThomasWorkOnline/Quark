#include "qkpch.h"
#include "VulkanGraphicsContext.h"

#include "VulkanUtils.h"

#include <fstream>
#include <set>
#include <sstream>

namespace Quark {

	VulkanContext* VulkanContext::s_Instance = nullptr;

	static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
		vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		vk::DebugUtilsMessageTypeFlagsEXT messageType,
		const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		if (messageType == vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral)
			return VK_FALSE;

		switch (messageSeverity)
		{
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose: QK_CORE_TRACE(pCallbackData->pMessage); return VK_FALSE;
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:    QK_CORE_INFO(pCallbackData->pMessage);  return VK_FALSE;
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning: QK_CORE_WARN(pCallbackData->pMessage);  return VK_FALSE;
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:   QK_CORE_FATAL(pCallbackData->pMessage); return VK_FALSE;
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

			vkMessengerCreateInfo.setPfnUserCallback((PFN_vkDebugUtilsMessengerCallbackEXT)VkDebugCallback);
			return vkMessengerCreateInfo;
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

		m_SwapChain.reset();
		m_Device.reset();

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		Utils::DestroyVkDebugUtilsMessengerEXT(m_VkInstance, m_VkDebugMessenger, nullptr);
#endif
		m_VkInstance.destroySurfaceKHR(m_VkSurface);
		m_VkInstance.destroy();
		s_Instance = nullptr;
	}

	void VulkanContext::Init()
	{
		QK_PROFILE_FUNCTION();

		s_Instance = this;

		// Instance creation
		{
			QK_PROFILE_SCOPE(VulkanContext::Instance);

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
		glfwCreateWindowSurface(m_VkInstance, m_WindowHandle, nullptr, reinterpret_cast<vk::SurfaceKHR::CType*>(&m_VkSurface));

		// Device creation
		m_Device = VulkanDevice::CreateDefaultForSurface(m_VkInstance, m_VkSurface);

		// Swap chain creation
		{
			Utils::SwapChainSupportDetails swapChainSupport = Utils::QuerySwapChainSupport(m_Device->GetPhysicalDevice(), m_VkSurface);

			uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
			if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
				imageCount = swapChainSupport.Capabilities.maxImageCount;

			VulkanSwapChainSpecification scSpec;
			scSpec.Extent = Utils::ChooseSwapExtent(swapChainSupport.Capabilities, m_WindowHandle);
			scSpec.SurfaceFormat = Utils::ChooseSwapSurfaceFormat(swapChainSupport.Formats);
			scSpec.PresentMode = Utils::ChooseSwapPresentMode(swapChainSupport.PresentModes);
			scSpec.FamilyIndices = m_Device->GetQueueFamilyIndices();
			scSpec.ImageCount = imageCount;

			m_SwapChain = CreateScope<VulkanSwapChain>(m_VkSurface, scSpec);
		}

		QK_CORE_TRACE("Created Vulkan graphics context!");
	}

	void VulkanContext::SwapBuffers()
	{
		m_SwapChain->Present();
	}

	VulkanContext& VulkanContext::Get()
	{
		QK_CORE_ASSERT(s_Instance, "Vulkan context is not initialized");
		return *s_Instance;
	}

	VulkanDevice& VulkanContext::GetCurrentDevice()
	{
		QK_CORE_ASSERT(s_Instance, "Vulkan context is not initialized");
		return *s_Instance->m_Device;
	}
}
