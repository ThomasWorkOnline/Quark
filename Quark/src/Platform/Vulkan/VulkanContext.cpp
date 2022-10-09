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
	}

	VulkanContext::VulkanContext(void* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
		QK_CORE_ASSERT(m_WindowHandle, "Window handle is nullptr");
	}

	void VulkanContext::Init()
	{
		QK_PROFILE_FUNCTION();

		auto extensions = Utils::GetRequiredVkExtensions();
		CreateInstance("Quark App", extensions);

		glfwCreateWindowSurface(m_Instance, m_WindowHandle, nullptr, &m_Surface);
		m_Device = VulkanDevice::CreateDefaultForSurface(m_Instance, m_Surface);

		int width, height;
		glfwGetFramebufferSize(m_WindowHandle, &width, &height);

		VkSurfaceFormatKHR surfaceFormat{};
		surfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
		surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		
		VulkanSwapChainSpecification spec;
		spec.Surface       = m_Surface;
		spec.MinImageCount = GetSwapChainImageCount();
		spec.Extent        = ChooseSwapExtent(width, height);
		spec.SurfaceFormat = ChooseSwapSurfaceFormat(surfaceFormat);
		spec.PresentMode   = ChooseSwapPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);

		m_SwapChain = CreateScope<VulkanSwapChain>(m_Device.get(), spec);

		VulkanContextBase::Init();
	}
}
