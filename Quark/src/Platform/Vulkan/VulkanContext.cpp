#include "qkpch.h"
#include "VulkanContext.h"

#include <GLFW/glfw3.h>

namespace Quark {

	namespace Utils {

		static std::vector<const char*> GetRequiredVkExtensions()
		{
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#if QK_ENABLE_VULKAN_VALIDATION_LAYERS
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
	}

	VulkanContext::VulkanContext(void* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
		QK_CORE_ASSERT(windowHandle, "Window handle is nullptr");
	}

	void VulkanContext::Init()
	{
		QK_PROFILE_FUNCTION();

		{
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

			CreateInstance(createInfo);
		}

		m_Surface = Utils::CreateSurfaceForPlatform(m_Instance, m_WindowHandle);
		m_Device = VulkanDevice::CreateDefaultForSurface(m_Instance, m_Surface);
		m_SwapChain = CreateScope<VulkanSwapChain>(m_Device.get(), m_WindowHandle, m_Surface);

		VulkanContextBase::Init();
	}
}
