#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

#include <vulkan/vulkan.h>

#if defined(QK_DEBUG)
#	ifndef QK_ENABLE_VULKAN_VALIDATION_LAYERS
#		define QK_ENABLE_VULKAN_VALIDATION_LAYERS
#	endif
#endif

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanContext final : public GraphicsContext
	{
	public:
		static constexpr uint32_t FramesInFlight = 2;

		VulkanContext(void* windowHandle);
		virtual ~VulkanContext() override;

		virtual void Init() override;
		virtual void StartFrame() override;
		virtual void Submit() override;

		virtual void SwapBuffers() override;

		virtual const Ref<CommandBuffer>& GetCommandBuffer() const override
		{
			return m_Data.CommandBuffers[m_Data.CurrentFrameIndex];
		}

		virtual void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight) override;

		VulkanSwapChain* GetSwapChain() { return m_SwapChain.get(); }
		uint32_t GetCurrentFrameIndex() const { return m_Data.CurrentFrameIndex; }

		static VulkanDevice* GetCurrentDevice() { return Get()->m_Device.get(); }
		static VulkanContext* Get() { return GraphicsContext::Get<VulkanContext>(); }

	private:
		struct VulkanData
		{
			VkFence            InFlightFences[FramesInFlight];
			VkSemaphore        RenderFinishedSemaphores[FramesInFlight];
			VkSemaphore        ImageAvailableSemaphores[FramesInFlight];
			Ref<CommandBuffer> CommandBuffers[FramesInFlight];

			uint32_t CurrentFrameIndex = static_cast<uint32_t>(-1);
		};

		GLFWwindow* m_WindowHandle;
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		Scope<VulkanDevice> m_Device;
		Scope<VulkanSwapChain> m_SwapChain;

		VulkanData m_Data;

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		VkDebugUtilsMessengerEXT m_VkDebugMessenger = VK_NULL_HANDLE;
#endif
	};
}
