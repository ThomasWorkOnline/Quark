#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include "Vulkan.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanCommandBuffer.h"

namespace Quark {

	inline const char* g_ValidationLayers[] = {
		"VK_LAYER_KHRONOS_validation"
	};

	class VulkanContextBase : public GraphicsContext
	{
	public:
		static constexpr uint32_t FramesInFlight = 2;

	public:
		virtual ~VulkanContextBase() override;

		virtual void Init() override;
		virtual void WaitUntilDeviceIdle() final override;

		virtual void BeginFrame() final override;
		virtual void Flush() final override;

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override;

		virtual uint32_t GetCurrentImageIndex() const final override;
		virtual uint32_t GetSwapChainImageCount() const final override;
		virtual FramebufferAttachment* GetColorAttachment(uint32_t index) const final override;

		virtual CommandBuffer* GetCommandBuffer() final override;

		VkInstance GetInstance() const { return m_Instance; }
		VulkanSwapChain* GetSwapChain() { return m_SwapChain.get(); }
		uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

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
			VkFence InFlightFence{};
			VkSemaphore RenderFinishedSemaphore{};
			VkSemaphore ImageAvailableSemaphore{};
			VulkanCommandBuffer CommandBuffer;
		};

		FrameData m_Frames[FramesInFlight];
		uint32_t m_CurrentFrameIndex = 0;
		bool m_SwapchainValid = false;

#if QK_ENABLE_VULKAN_VALIDATION_LAYERS
		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
#endif
	};
}
