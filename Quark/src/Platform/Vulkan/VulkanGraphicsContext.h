#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include "VulkanCore.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanGraphicsContext final : public GraphicsContext
	{
	public:
		VulkanGraphicsContext(void* windowHandle);
		virtual ~VulkanGraphicsContext() override;

		virtual void Init() override;
		virtual void SwapBuffers() override;

		VulkanSwapChain& GetSwapChain() { return *m_SwapChain; }
		
		static VulkanGraphicsContext& Get();
		static VulkanDevice& GetCurrentDevice();

	private:
		GLFWwindow* m_WindowHandle;

		vk::Instance m_VkInstance;
		vk::SurfaceKHR m_VkSurface;

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		vk::DebugUtilsMessengerEXT m_VkDebugMessenger;
#endif
		Scope<VulkanDevice> m_Device;
		Scope<VulkanSwapChain> m_SwapChain;

		static VulkanGraphicsContext* s_Instance;
	};
}
