#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include "Vulkan.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

#include <vector>

namespace Quark {

	inline const char* g_ValidationLayers[] = {
		"VK_LAYER_KHRONOS_validation"
	};

	class VulkanContextBase : public GraphicsContext
	{
	public:
		VulkanContextBase();
		virtual ~VulkanContextBase() override;

		virtual void Init() override;
		virtual void WaitUntilDeviceIdle() final override;

		virtual void BeginFrame(uint32_t frameIndex) final override;
		virtual void Flush(CommandBuffer* commandBuffer, uint32_t frameIndex) final override;

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override;

		virtual Framebuffer* GetFramebuffer() const final override;

		VkInstance GetInstance() const { return m_Instance; }
		VulkanSwapChain* GetSwapChain() { return m_SwapChain.get(); }

	protected:
		void CreateInstance(const char* appName, const std::vector<const char*>& extensions);

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(VkSurfaceFormatKHR preferred);
		VkPresentModeKHR ChooseSwapPresentMode(VkPresentModeKHR preferred);
		VkExtent2D ChooseSwapExtent(uint32_t width, uint32_t height);
		uint32_t GetSwapChainImageCount();

	protected:
		VkInstance m_Instance = nullptr;
		VkSurfaceKHR m_Surface = nullptr;

		Scope<VulkanDevice> m_Device;
		Scope<VulkanSwapChain> m_SwapChain;

	private:
		struct FrameData
		{
			VkFence InFlightFence;
			VkSemaphore RenderFinishedSemaphore;
			VkSemaphore ImageAvailableSemaphore;
		};

		std::vector<FrameData> m_Frames;
		bool m_SwapchainValid = false;

#if QK_ENABLE_VULKAN_VALIDATION_LAYERS
		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
#endif
	};
}
