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

		VulkanContextBase::Init();
	}

	void VulkanContext::CreateSwapChain(const SwapChainSpecification& spec)
	{
		m_SwapChain = CreateScope<VulkanSwapChain>(m_Device.get(), m_Surface, spec);
	}

	ViewportExtent VulkanContext::GetViewportExtent() const
	{
		int viewportWidth, viewportHeight;
		glfwGetFramebufferSize(m_WindowHandle, &viewportWidth, &viewportHeight);
		return { (uint32_t)viewportWidth, (uint32_t)viewportHeight };
	}
}
