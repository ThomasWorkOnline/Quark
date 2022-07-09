#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/Renderer.h"
#include "Quark/Renderer/Pipeline.h"

#include "VulkanBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanShader.h"
#include "VulkanUtils.h"

#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanPipeline final : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineSpecification& spec);
		virtual ~VulkanPipeline() override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		virtual void Submit() override;
		virtual void BeginRenderPass(const Ref<RenderPass>& renderPass) override;
		virtual void EndRenderPass() override;

		virtual const Ref<CommandBuffer>& GetCommandBuffer() const override { return m_CommandBuffers[m_CurrentFrameIndex]; }
		virtual const Ref<UniformBuffer>& GetUniformBuffer() const override { return m_CameraUniformBuffers[m_CurrentFrameIndex]; }

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) override;

		virtual const PipelineSpecification& GetSpecification() const override { return m_Spec; }

	private:
		void RecreateFramebuffers();
		void RecreateGraphicsPipeline();

	private:
		vk::DescriptorPool m_DescriptorPool;
		vk::DescriptorSetLayout m_DescriptorSetLayout;
		vk::PipelineLayout m_PipelineLayout;
		vk::Pipeline m_GraphicsPipeline;
		VkCommandBuffer m_ActiveCommandBuffer = VK_NULL_HANDLE;

		std::vector<vk::DescriptorSet> m_DescriptorSets;
		std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
		std::vector<vk::Framebuffer> m_SwapChainFramebuffers;
		std::vector<Ref<UniformBuffer>> m_CameraUniformBuffers;

		Ref<CommandBuffer> m_CommandBuffers[Renderer::FramesInFlight];

		uint32_t m_CurrentFrameIndex = std::numeric_limits<uint32_t>::max();
		uint32_t m_NextImageIndex = 0;

		PipelineSpecification m_Spec;
	};
}
