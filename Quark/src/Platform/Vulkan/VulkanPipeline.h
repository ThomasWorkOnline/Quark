#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/Pipeline.h"

#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanShader.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanPipeline final : public Pipeline
	{
	public:
		VulkanPipeline(VulkanDevice* device, const PipelineSpecification& spec);
		virtual ~VulkanPipeline() override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) override;
		virtual UniformBuffer* GetUniformBuffer() const override;

		virtual bool operator==(const Pipeline& other) const override
		{
			return m_Pipeline == reinterpret_cast<const VulkanPipeline&>(other).m_Pipeline;
		}

		VkPipeline GetVkHandle() const { return m_Pipeline; }
		VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
		VkDescriptorSet GetDescriptorSet() const;

	private:
		void Invalidate();

	private:
		VulkanDevice* m_Device;

		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_Pipeline = VK_NULL_HANDLE;

		VkDescriptorSet m_DescriptorSets[VulkanContext::FramesInFlight];
		Scope<UniformBuffer> m_CameraUniformBuffers[VulkanContext::FramesInFlight];
	};
}
