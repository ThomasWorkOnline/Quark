#pragma once

#include "Quark/Core/Core.h"
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

		virtual void Bind(const Ref<CommandBuffer>& commandBuffer) override;
		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) override;

		virtual const Ref<UniformBuffer>& GetUniformBuffer() const override;

		VkPipeline GetVkHandle() const { return m_Pipeline; }

	private:
		void Invalidate();

	private:
		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_Pipeline = VK_NULL_HANDLE;

		std::vector<VkDescriptorSet> m_DescriptorSets;
		std::vector<Ref<UniformBuffer>> m_CameraUniformBuffers;
	};
}
