#pragma once

#include "Quark/Renderer/Pipeline.h"

#include "Vulkan.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanSampler.h"

namespace Quark {

	class VulkanPipeline final : public Pipeline
	{
	public:
		VulkanPipeline(VulkanDevice* device, const PipelineSpecification& spec);
		virtual ~VulkanPipeline() final override;

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

	private:
		VulkanDevice* m_Device = nullptr;

		VkPipeline m_Pipeline = nullptr;
		VkPipelineLayout m_PipelineLayout = nullptr;

		VkDescriptorPool m_DescriptorPool = nullptr;
		VkDescriptorSetLayout m_DescriptorSetLayout = nullptr;
		VkDescriptorSet m_DescriptorSets[VulkanContext::FramesInFlight]{};
	};
}
