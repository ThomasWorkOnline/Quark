#pragma once

#include "Quark/Renderer/GraphicsContext.h"
#include "VulkanCore.h"
#include "VulkanSwapChain.h"

#include <vector>

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanGraphicsContext final : public GraphicsContext
	{
	public:
		VulkanGraphicsContext(void* windowHandle);
		virtual ~VulkanGraphicsContext() override;

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;

		vk::Instance m_VkInstance;
		vk::SurfaceKHR m_VkSurface;

		vk::PhysicalDevice m_VkPhysicalDevice;
		vk::Device m_VkDevice;
		vk::Queue m_VkPresentQueue;

		Scope<VulkanSwapChain> m_SwapChain;

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		vk::DebugUtilsMessengerEXT m_VkDebugMessenger;
#endif
	};
}
