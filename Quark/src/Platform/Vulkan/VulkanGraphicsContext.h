#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include "VulkanCore.h"
#include "VulkanRenderPass.h"
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

	private:
		void RecordCommandBuffer(uint32_t imageIndex);

	private:
		GLFWwindow* m_WindowHandle;

		vk::Instance m_VkInstance;

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		vk::DebugUtilsMessengerEXT m_VkDebugMessenger;
#endif
		vk::SurfaceKHR m_VkSurface;
		vk::PhysicalDevice m_VkPhysicalDevice;
		vk::Device m_VkDevice;

		vk::Queue m_VkPresentQueue;
		Scope<VulkanSwapChain> m_SwapChain;
		Scope<VulkanRenderPass> m_RenderPass; // TODO: not allocate them on the heap

		vk::PipelineLayout m_VkPipelineLayout;
		vk::Pipeline m_VkGraphicsPipeline;
		std::vector<vk::Framebuffer> m_VkSwapChainFramebuffers;

		vk::CommandPool m_VkCommandPool;
		vk::CommandBuffer m_VkCommandBuffer;
	};
}
