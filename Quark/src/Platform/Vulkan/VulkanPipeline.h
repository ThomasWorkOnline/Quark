#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/Pipeline.h"

#include "VulkanContext.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanPipeline final : public Pipeline
	{
	public:
		VulkanPipeline(VulkanDevice* device, const PipelineSpecification& spec);
		virtual ~VulkanPipeline() override;

		virtual bool operator==(const Pipeline& other) const final override
		{
			return m_Pipeline == reinterpret_cast<const VulkanPipeline&>(other).m_Pipeline;
		}

		// Non-Copyable
		VulkanPipeline(const VulkanPipeline&) = delete;
		VulkanPipeline& operator=(const VulkanPipeline&) = delete;

		VkPipeline GetVkHandle() const { return m_Pipeline; }
		VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
		VkDescriptorSet GetDescriptorSet() const;

		void UpdateDescriptorSets();
		void CreateDescriptorSetLayout();
		void CreateDescriptorPoolAndSets();
		void CreatePipeline();

	private:
		VulkanDevice* m_Device = nullptr;

		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorSet m_DescriptorSets[VulkanContext::FramesInFlight]{};

		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_Pipeline = VK_NULL_HANDLE;
	};
}
