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

	class VulkanContextBase : public GraphicsContext
	{
	public:
		static constexpr uint32_t FramesInFlight = 2;

		virtual ~VulkanContextBase() override;
		virtual void StartFrame() override;

		virtual void Submit() override;
		virtual void SwapBuffers() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t viewportWidth, uint32_t viewportHeight) override;

		virtual uint32_t GetCurrentImageIndex() const override;
		virtual uint32_t GetSwapChainImageCount() const override;
		virtual CommandBuffer* GetCommandBuffer() override;

		void CreateInstance(VkInstanceCreateInfo& createInfo);
		void CreateSyncObjects();

		VulkanSwapChain* GetSwapChain() { return m_SwapChain.get(); }
		uint32_t GetCurrentFrameIndex() const { return m_Data.CurrentFrameIndex; }

	protected:
		struct VulkanData
		{
			VkFence InFlightFences[FramesInFlight];
			VkSemaphore RenderFinishedSemaphores[FramesInFlight];
			VkSemaphore ImageAvailableSemaphores[FramesInFlight];
			VulkanCommandBuffer CommandBuffers[FramesInFlight];

			uint32_t CurrentFrameIndex = static_cast<uint32_t>(-1);
		};

		VkInstance m_Instance = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		Scope<VulkanDevice> m_Device;
		Scope<VulkanSwapChain> m_SwapChain;

		VulkanData m_Data{};

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		VkDebugUtilsMessengerEXT m_VkDebugMessenger = VK_NULL_HANDLE;
#endif
	};
}