#pragma once

#include "Quark/Renderer/Pipeline.h"

#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanSampler.h"
#include "VulkanTexture.h"
#include "VulkanUniformBuffer.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanPipeline final : public Pipeline
	{
	public:
		VulkanPipeline(VulkanDevice* device, const PipelineSpecification& spec);
		virtual ~VulkanPipeline() final override;

		virtual void SetTexture(const Texture* texture, uint32_t textureIndex) final override;
		virtual void PushDescriptorSets() final override;

		virtual bool operator==(const Pipeline& other) const final override;

		// Non-Copyable
		VulkanPipeline(const VulkanPipeline&) = delete;
		VulkanPipeline& operator=(const VulkanPipeline&) = delete;

		VkPipeline GetVkHandle() const { return m_Pipeline; }
		VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
		VkDescriptorSet GetDescriptorSet() const;

	private:
		void CreateDescriptorSetLayout();
		void CreateDescriptorPoolAndSets();
		void CreatePipeline();
		void CreatePipelineResources();

	private:
		struct CombinedSampler
		{
			const VulkanTexture2D* Texture = nullptr;
			Scope<VulkanSampler> Sampler;
		};

		VulkanDevice* m_Device = nullptr;

		VkDescriptorPool m_DescriptorPool = nullptr;
		VkDescriptorSetLayout m_DescriptorSetLayout = nullptr;
		VkDescriptorSet m_DescriptorSets[VulkanContext::FramesInFlight]{};

		VkPipelineLayout m_PipelineLayout = nullptr;
		VkPipeline m_Pipeline = nullptr;

		std::vector<CombinedSampler> m_CombinedSamplers;
	};
}
