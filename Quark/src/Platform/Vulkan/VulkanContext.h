#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include "VulkanDevice.h"
#include "VulkanUtils.h"
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
		virtual SwapChain* GetSwapChain() override { return m_SwapChain.get(); }

		static VulkanDevice* GetCurrentDevice() { return static_cast<VulkanContext&>(Get()).m_Device.get(); }

	private:
		GLFWwindow* m_WindowHandle;

		VkInstance m_Instance = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		VkDebugUtilsMessengerEXT m_VkDebugMessenger = VK_NULL_HANDLE;
#endif
		Scope<VulkanSwapChain> m_SwapChain;
		Scope<VulkanDevice> m_Device;
	};
}
