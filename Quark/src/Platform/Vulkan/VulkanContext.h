#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include "VulkanDevice.h"
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

		static VulkanDevice* GetCurrentDevice() { return static_cast<VulkanContext&>(Get()).m_Device.get(); }

	private:
		GLFWwindow* m_WindowHandle;

		vk::Instance m_Instance;
		vk::SurfaceKHR m_Surface;

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		vk::DebugUtilsMessengerEXT m_VkDebugMessenger;
#endif
		Scope<VulkanDevice> m_Device;
	};
}
