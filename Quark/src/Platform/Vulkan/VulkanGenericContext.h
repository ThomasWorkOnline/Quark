#pragma once

#include "Vulkan.h"
#include "VulkanContext.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanGenericContext final : public VulkanContext
	{
	public:
		VulkanGenericContext(void* windowHandle);
		virtual ~VulkanGenericContext();

		virtual void Init(const SwapChainSpecification& spec) override;
		virtual void BeginFrame(uint32_t frameIndex) final override;
		virtual void Flush(CommandBuffer* commandBuffer) final override;

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override;

		virtual SwapChain*     GetSwapChain() final override;
		virtual ViewportExtent QuerySwapExtent() const final override;

	private:
		struct FrameData
		{
			VkFence InFlightFence;
			VkSemaphore RenderFinishedSemaphore;
			VkSemaphore ImageAvailableSemaphore;
		};

		GLFWwindow* m_WindowHandle;
		VkSurfaceKHR m_Surface = nullptr;

		uint32_t m_CurrentFrameIndex = 0;
		std::vector<FrameData> m_Frames;

		Scope<VulkanSwapChain> m_SwapChain;
		bool m_SwapchainValid = false;
	};
}
