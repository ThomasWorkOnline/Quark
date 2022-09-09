#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanCommandBuffer.h"

#include <vulkan/vulkan.h>

#if defined(QK_DEBUG)
#	ifndef QK_ENABLE_VULKAN_VALIDATION_LAYERS
#		define QK_ENABLE_VULKAN_VALIDATION_LAYERS
#	endif
#endif

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
		virtual void EndFrame() final override;

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override;

		virtual uint32_t GetCurrentImageIndex() const final override;
		virtual uint32_t GetSwapChainImageCount() const final override;
		virtual FramebufferAttachment* GetColorAttachment(uint32_t index) const final override;

		virtual CommandBuffer* GetCommandBuffer() final override;

		void CreateInstance(VkInstanceCreateInfo& createInfo);

		VkInstance GetInstance() const { return m_Instance; }
		VulkanSwapChain* GetSwapChain() { return m_SwapChain.get(); }
		uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

	protected:
		struct FrameData
		{
			VkFence InFlightFence{};
			VkSemaphore RenderFinishedSemaphore{};
			VkSemaphore ImageAvailableSemaphore{};
			VulkanCommandBuffer CommandBuffer;
		};

		VkInstance m_Instance = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		Scope<VulkanDevice> m_Device;
		Scope<VulkanSwapChain> m_SwapChain;

		FrameData m_Frames[FramesInFlight];
		uint32_t m_CurrentFrameIndex = 0;
		bool m_SwapchainValid = false;

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		VkDebugUtilsMessengerEXT m_VkDebugMessenger = VK_NULL_HANDLE;
#endif
	};
}
