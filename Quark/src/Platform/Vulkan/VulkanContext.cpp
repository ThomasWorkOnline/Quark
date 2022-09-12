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

		auto extensions = Utils::GetRequiredVkExtensions();
		CreateInstance("Quark App", extensions);

		m_Surface = Utils::CreateSurfaceForPlatform(m_Instance, m_WindowHandle);
		m_Device = VulkanDevice::CreateDefaultForSurface(m_Instance, m_Surface);

		int width, height;
		glfwGetFramebufferSize(m_WindowHandle, &width, &height);

		VkSurfaceFormatKHR surfaceFormat{};
		surfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
		surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

		VulkanSwapChainSpecification spec;
		spec.Width = width;
		spec.Height = height;
		spec.MinImageCount = GetSwapChainImageCount();
		spec.SurfaceFormat = surfaceFormat;
		spec.PresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		m_SwapChain = CreateScope<VulkanSwapChain>(m_Device.get(), m_Surface, spec);

		VulkanContextBase::Init();
	}
}
