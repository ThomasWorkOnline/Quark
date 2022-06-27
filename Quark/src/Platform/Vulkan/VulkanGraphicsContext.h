#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include <vulkan/vulkan.hpp>

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanContext final : public GraphicsContext
	{
	public:
		VulkanContext(void* windowHandle);
		virtual ~VulkanContext() override;

		virtual void Init() override;
		virtual void SwapBuffers() override;

		static VulkanDevice& GetCurrentDevice();
		static VulkanSwapChain& GetSwapChain();

	private:
		GLFWwindow* m_WindowHandle;

		vk::Instance m_VkInstance;
		vk::SurfaceKHR m_VkSurface;

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		vk::DebugUtilsMessengerEXT m_VkDebugMessenger;
#endif
		Scope<VulkanDevice> m_Device;
		Scope<VulkanSwapChain> m_SwapChain;

		static VulkanContext* s_Instance;
	};
}
