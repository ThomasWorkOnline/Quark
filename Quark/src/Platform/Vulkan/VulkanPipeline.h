#pragma once

#include "VulkanBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanShader.h"

#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanPipeline
	{
	public:
		VulkanPipeline();
		~VulkanPipeline();

		void Begin();
		void End();

		void Submit();
		void BeginRenderPass();
		void EndRenderPass();

		void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight);

	private:
		void RecreateFramebuffers();
		void RecreateGraphicsPipeline();

	private:
		VulkanShader m_VertShader;
		VulkanShader m_FragShader;

		Ref<VertexBuffer> m_VertexBuffer;

		vk::PipelineLayout m_VkPipelineLayout;
		vk::Pipeline m_VkGraphicsPipeline;
		vk::RenderPass m_VkRenderPass;

		std::vector<vk::CommandBuffer> m_VkCommandBuffers;
		std::vector<vk::Framebuffer> m_VkSwapChainFramebuffers;
		std::vector<vk::Fence> m_VkInFlightFences;
		std::vector<vk::Semaphore> m_VkImageAvailableSemaphores;

		vk::CommandBuffer m_ActiveCommandBuffer;

		uint32_t m_ActiveFrameIndex = std::numeric_limits<uint32_t>::max();
		uint32_t m_NextImageIndex = 0;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	};
}
