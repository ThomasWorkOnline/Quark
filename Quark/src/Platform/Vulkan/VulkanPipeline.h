#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/RenderPipeline.h"

#include "VulkanBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanShader.h"

#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanPipeline final : public RenderPipeline
	{
	public:
		VulkanPipeline();
		virtual ~VulkanPipeline() override;

		virtual void Begin(const Mat4f& cameraProjection, const Mat4f& cameraView) override;
		virtual void End() override;

		virtual void Submit() override;
		virtual void BeginRenderPass() override;
		virtual void EndRenderPass() override;

		virtual void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight) override;

	private:
		void RecreateFramebuffers();
		void RecreateGraphicsPipeline();

	private:
		Ref<Shader> m_VertShader;
		Ref<Shader> m_FragShader;

		CameraBufferData m_CameraData;
		UniformBuffer* m_ActiveCameraUniformBuffer = nullptr;

		vk::DescriptorSet m_DescriptorSet;
		vk::DescriptorPool m_DescriptorPool;
		vk::DescriptorSetLayout m_DescriptorSetLayout;
		vk::PipelineLayout m_PipelineLayout;
		vk::Pipeline m_GraphicsPipeline;
		vk::RenderPass m_RenderPass;

		vk::CommandBuffer m_ActiveCommandBuffer;

		std::vector<vk::Fence> m_InFlightFences;
		std::vector<vk::Framebuffer> m_SwapChainFramebuffers;
		std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
		std::vector<Ref<UniformBuffer>> m_CameraUniformBuffers;

		uint32_t m_CurrentFrameIndex = std::numeric_limits<uint32_t>::max();
		uint32_t m_NextImageIndex = 0;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	};
}
