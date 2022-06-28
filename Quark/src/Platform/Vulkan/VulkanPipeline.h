#pragma once

#include "VulkanBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanShader.h"

#include <vulkan/vulkan.hpp>

namespace Quark {

	// TODO: remove
	struct Vertex
	{
		glm::vec2 Position;
		glm::vec3 Color;

		static vk::VertexInputBindingDescription GetBindingDescription()
		{
			vk::VertexInputBindingDescription bindingDescription;
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = vk::VertexInputRate::eVertex;

			return bindingDescription;
		}

		static std::array<vk::VertexInputAttributeDescription, 2> GetAttributeDescriptions()
		{
			std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = vk::Format::eR32G32Sfloat;
			attributeDescriptions[0].offset = offsetof(Vertex, Position);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
			attributeDescriptions[1].offset = offsetof(Vertex, Color);

			return attributeDescriptions;
		}
	};

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

		vk::PipelineLayout m_VkPipelineLayout;
		vk::Pipeline m_VkGraphicsPipeline;
		vk::RenderPass m_VkRenderPass;

		std::vector<vk::Framebuffer> m_VkSwapChainFramebuffers;
		std::vector<vk::Fence> m_VkInFlightFences;
		std::vector<vk::Semaphore> m_VkImageAvailableSemaphores;

		vk::CommandBuffer m_ActiveCommandBuffer;
		uint32_t m_ActiveFrameIndex = std::numeric_limits<uint32_t>::max();
		uint32_t m_NextImageIndex = 0;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	};
}
