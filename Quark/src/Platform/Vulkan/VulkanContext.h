#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanUtils.h"

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

		virtual void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight) override;

		static VulkanDevice* GetCurrentDevice() { return s_Instance->m_Device.get(); }
		static VulkanSwapChain* GetSwapChain() { return s_Instance->m_SwapChain.get(); }

	private:
		GLFWwindow* m_WindowHandle;

		vk::Instance m_Instance;
		vk::SurfaceKHR m_Surface;

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		vk::DebugUtilsMessengerEXT m_VkDebugMessenger;
#endif
		Scope<VulkanDevice> m_Device;
		Scope<VulkanSwapChain> m_SwapChain;

		static VulkanContext* s_Instance;
	};
}
