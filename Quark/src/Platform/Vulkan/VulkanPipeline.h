#pragma once

#include "Quark/Core/Core.h"

#include "VulkanBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanUniformBuffer.h"
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

	struct CameraBufferData
	{
		Mat4f View       = Mat4(1.0f);
		Mat4f Projection = Mat4(1.0f);
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

		const Ref<UniformBuffer>& GetCameraUniformBuffer() const { return m_CameraUniformBuffers[m_CurrentFrameIndex]; }

		void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight);

	private:
		void RecreateFramebuffers();
		void RecreateGraphicsPipeline();

	private:
		VulkanShader m_VertShader;
		VulkanShader m_FragShader;

		vk::DescriptorSet m_DescriptorSet;
		vk::DescriptorPool m_DescriptorPool;
		vk::DescriptorSetLayout m_DescriptorSetLayout;
		vk::PipelineLayout m_PipelineLayout;
		vk::Pipeline m_GraphicsPipeline;
		vk::RenderPass m_RenderPass;

		vk::CommandBuffer m_ActiveCommandBuffer;
		UniformBuffer* m_ActiveCameraUniformBuffer = nullptr;

		std::vector<vk::Fence> m_InFlightFences;
		std::vector<vk::Framebuffer> m_SwapChainFramebuffers;
		std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
		std::vector<Ref<UniformBuffer>> m_CameraUniformBuffers;

		uint32_t m_CurrentFrameIndex = std::numeric_limits<uint32_t>::max();
		uint32_t m_NextImageIndex = 0;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	};
}
