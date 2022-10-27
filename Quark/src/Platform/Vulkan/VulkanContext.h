#pragma once

#include "Vulkan.h"
#include "VulkanContextBase.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanContext final : public VulkanContextBase
	{
	public:
		VulkanContext(void* windowHandle);
		virtual ~VulkanContext() = default;

		virtual void Init() override;
		virtual void CreateSwapChain(const SwapChainSpecification& spec) final override;

		virtual ViewportExtent GetViewportExtent() const final override;

		static VulkanContext* Get() { return GraphicsContext::Get<VulkanContext>(); }
		static VulkanDevice*  GetCurrentDevice() { return Get()->m_Device.get(); }

	private:
		GLFWwindow* m_WindowHandle;
	};
}
