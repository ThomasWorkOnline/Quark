#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include "Vulkan.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

#include <span>

namespace Quark {

	inline const char* g_ValidationLayers[] = {
		"VK_LAYER_KHRONOS_validation"
	};

	class VulkanContext : public GraphicsContext
	{
	public:
		virtual ~VulkanContext() override;

		virtual void WaitUntilDeviceIdle() final override;

		virtual uint32_t          QuerySwapChainImageCount() const final override;
		virtual SwapSurfaceFormat ChooseSurfaceFormat(SwapSurfaceFormat preferred) const final override;
		virtual SwapPresentMode   ChooseSwapPresentMode(SwapPresentMode preferred) const final override;

		VkInstance                GetInstance() const { return m_Instance; }

		static VulkanDevice*      GetCurrentDevice() { return Get<VulkanContext>()->m_Device.get(); }

	protected:
		void CreateInstance(const char* appName, std::span<const char*> extensions);

	protected:
		VkInstance m_Instance = nullptr;
		Scope<VulkanDevice> m_Device;

#if QK_ENABLE_VULKAN_VALIDATION_LAYERS
		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
#endif
	};
}
