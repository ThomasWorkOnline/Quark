#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/Pipeline.h"

#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanShader.h"
#include "VulkanUtils.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanPipeline final : public Pipeline
	{
	public:
		VulkanPipeline(VulkanDevice* device, const PipelineSpecification& spec);
		virtual ~VulkanPipeline() override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) override;

		virtual const Ref<UniformBuffer>& GetUniformBuffer() const override;

		virtual bool operator==(const Pipeline& other) const override
		{
			return m_Pipeline == reinterpret_cast<const VulkanPipeline&>(other).m_Pipeline;
		}

		void Bind(VkCommandBuffer commandBuffer);

		VkPipeline GetVkHandle() const { return m_Pipeline; }

	private:
		void Invalidate();

	private:
		VulkanDevice* m_Device;

		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_Pipeline = VK_NULL_HANDLE;

		std::vector<VkFramebuffer> m_Framebuffers;
		std::vector<VkDescriptorSet> m_DescriptorSets;
		std::vector<Ref<UniformBuffer>> m_CameraUniformBuffers;
	};
}
