#include "qkpch.h"
#include "VulkanPipeline.h"

#include "VulkanEnums.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>

namespace Quark {

	VulkanPipeline::VulkanPipeline(VulkanDevice* device, const PipelineSpecification& spec)
		: Pipeline(spec)
		, m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		CreateDescriptorSetLayout();
		CreateDescriptorPoolAndSets();
		CreatePipeline();
		CreatePipelineResources();

#if 0
		for (uint32_t i = 0; i < m_Spec.UniformBufferCount; i++)
		{
			auto* uniformBuffer = static_cast<const VulkanUniformBuffer*>(m_Spec.UniformBuffers[i]);

			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffer->GetVkHandle();
			bufferInfo.offset = 0;
			bufferInfo.range = uniformBuffer->GetSize();

			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstBinding = uniformBuffer->GetBinding(); // TODO: group by binding and call vkUpdateDescriptorSets once per set
			writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescriptorSet.pBufferInfo = &bufferInfo;

			for (uint32_t i = 0; i < VulkanContext::FramesInFlight; i++)
			{
				writeDescriptorSet.dstSet = m_DescriptorSets[i];
				vkUpdateDescriptorSets(m_Device->GetVkHandle(), 1, &writeDescriptorSet, 0, nullptr);
			}
		}
#endif
	}

	VulkanPipeline::~VulkanPipeline()
	{
		QK_PROFILE_FUNCTION();

		vkDestroyPipeline(m_Device->GetVkHandle(), m_Pipeline, nullptr);
		vkDestroyPipelineLayout(m_Device->GetVkHandle(), m_PipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_Device->GetVkHandle(), m_DescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(m_Device->GetVkHandle(), m_DescriptorPool, nullptr);
	}

	void VulkanPipeline::SetTexture(const Texture* texture, uint32_t textureIndex)
	{
		auto* vulkanTexture = dynamic_cast<const VulkanTexture2D*>(texture);
		m_CombinedSamplers[textureIndex].Texture = vulkanTexture;
	}

	void VulkanPipeline::PushDescriptorSets()
	{
		// Uniform Buffers
		for (uint32_t i = 0; i < m_Spec.UniformBufferCount; i++)
		{
			auto* uniformBuffer = static_cast<const VulkanUniformBuffer*>(m_Spec.UniformBuffers[i]);

			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffer->GetVkHandle();
			bufferInfo.offset = 0;
			bufferInfo.range = uniformBuffer->GetSize();

			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstBinding = uniformBuffer->GetBinding(); // TODO: group by binding and call vkUpdateDescriptorSets once per set
			writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescriptorSet.pBufferInfo = &bufferInfo;
			writeDescriptorSet.dstSet = GetDescriptorSet();

			vkUpdateDescriptorSets(m_Device->GetVkHandle(), 1, &writeDescriptorSet, 0, nullptr);
		}

		// Samplers
		for (uint32_t i = 0; i < m_CombinedSamplers.size(); i++)
		{
			if (!m_CombinedSamplers[i].Texture)
				break;

			VkDescriptorImageInfo imageInfo{};
			imageInfo.sampler = m_CombinedSamplers[i].Sampler->GetVkHandle();
			imageInfo.imageView = m_CombinedSamplers[i].Texture->GetVkHandle();
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstBinding = m_CombinedSamplers[i].Sampler->GetBinding();
			writeDescriptorSet.dstArrayElement = i;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescriptorSet.pImageInfo = &imageInfo;
			writeDescriptorSet.dstSet = GetDescriptorSet();

			vkUpdateDescriptorSets(m_Device->GetVkHandle(), 1, &writeDescriptorSet, 0, nullptr);
			m_CombinedSamplers[i].Texture = nullptr;
		}
	}

	bool VulkanPipeline::operator==(const Pipeline& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_Pipeline == o->m_Pipeline;

		return false;
	}

	VkDescriptorSet VulkanPipeline::GetDescriptorSet() const
	{
		return m_DescriptorSets[VulkanContext::Get()->GetCurrentFrameIndex()];
	}

	void VulkanPipeline::CreateDescriptorSetLayout()
	{
		QK_PROFILE_FUNCTION();

		const auto& shaderResources = m_Spec.Shader->GetShaderResources();

		uint32_t bindingCount = m_Spec.Shader->GetBindingCount();
		AutoRelease<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = StackAlloc(bindingCount * sizeof(VkDescriptorSetLayoutBinding));
		VkDescriptorSetLayoutBinding* descriptorSetLayoutbindingPtr = descriptorSetLayoutBindings;

		for (auto& uniformBuffer : shaderResources.UniformBuffers)
		{
			*descriptorSetLayoutbindingPtr = {};
			descriptorSetLayoutbindingPtr->binding = uniformBuffer.Decorators.Binding;
			descriptorSetLayoutbindingPtr->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorSetLayoutbindingPtr->descriptorCount = 1;
			descriptorSetLayoutbindingPtr->stageFlags = ShaderStageToVulkan(uniformBuffer.Stage);
			descriptorSetLayoutbindingPtr++;
		}

		for (auto& samplerArray : shaderResources.SamplerArrays)
		{
			*descriptorSetLayoutbindingPtr = {};
			descriptorSetLayoutbindingPtr->binding = samplerArray.Decorators.Binding;
			descriptorSetLayoutbindingPtr->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorSetLayoutbindingPtr->descriptorCount = samplerArray.SamplerCount;
			descriptorSetLayoutbindingPtr->stageFlags = ShaderStageToVulkan(samplerArray.Stage);
			descriptorSetLayoutbindingPtr->pImmutableSamplers = nullptr;
			descriptorSetLayoutbindingPtr++;
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindingCount;
		layoutInfo.pBindings = descriptorSetLayoutBindings;

		vkCreateDescriptorSetLayout(m_Device->GetVkHandle(), &layoutInfo, nullptr, &m_DescriptorSetLayout);
	}

	void VulkanPipeline::CreateDescriptorPoolAndSets()
	{
		QK_PROFILE_FUNCTION();

		const auto& shaderResources = m_Spec.Shader->GetShaderResources();

		// Descriptor pool shared for all frames in flight
		{
			uint32_t poolSizeCount = 0;
			uint32_t ubDescriptorCount = (uint32_t)shaderResources.UniformBuffers.size();
			poolSizeCount += (ubDescriptorCount > 0);

			uint32_t samplerDescriptorCount = (uint32_t)shaderResources.SamplerArrays.size();
			poolSizeCount += (samplerDescriptorCount > 0);

			AutoRelease<VkDescriptorPoolSize> poolSizes = StackAlloc(poolSizeCount * sizeof(VkDescriptorPoolSize));
			VkDescriptorPoolSize* poolSizesPtr = poolSizes;

			// Uniform Buffers
			if (ubDescriptorCount > 0)
			{
				poolSizesPtr->type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				poolSizesPtr->descriptorCount = ubDescriptorCount * VulkanContext::FramesInFlight;
				poolSizesPtr++;
			}
			
			// Samplers
			if (samplerDescriptorCount > 0)
			{
				poolSizesPtr->type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				poolSizesPtr->descriptorCount = samplerDescriptorCount * VulkanContext::FramesInFlight;
				poolSizesPtr++;
			}

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.maxSets = VulkanContext::FramesInFlight;
			poolInfo.poolSizeCount = poolSizeCount;
			poolInfo.pPoolSizes = poolSizes;

			vkCreateDescriptorPool(m_Device->GetVkHandle(), &poolInfo, nullptr, &m_DescriptorPool);
		}

		// Descriptor sets
		{
			// Copy each layout for each frame in flight
			VkDescriptorSetLayout layouts[VulkanContext::FramesInFlight]{};
			for (uint32_t i = 0; i < VulkanContext::FramesInFlight; i++)
				layouts[i] = m_DescriptorSetLayout;

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorPool;
			allocInfo.descriptorSetCount = 1 * VulkanContext::FramesInFlight;
			allocInfo.pSetLayouts = layouts;

			vkAllocateDescriptorSets(m_Device->GetVkHandle(), &allocInfo, m_DescriptorSets);
		}
	}

	void VulkanPipeline::CreatePipeline()
	{
		AutoRelease<VkVertexInputAttributeDescription> attributeDescriptions = StackAlloc(m_Spec.Layout.GetCount() * sizeof(VkVertexInputAttributeDescription));

		for (uint32_t i = 0; i < m_Spec.Layout.GetCount(); i++)
		{
			attributeDescriptions[i] = {};
			attributeDescriptions[i].location = i;
			attributeDescriptions[i].binding = 0;
			attributeDescriptions[i].format = ShaderDataTypeToVulkan(m_Spec.Layout[i].Type);
			attributeDescriptions[i].offset = (uint32_t)m_Spec.Layout[i].Offset;
		}

		// Vertex input 
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = (uint32_t)m_Spec.Layout.GetStride();
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = m_Spec.Layout.GetCount();
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = (VkPrimitiveTopology)m_Spec.Topology;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// Rasterization
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthBiasClamp = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; // <-- Vulkan uses right-handed coordinate system so vertex winding is flipped
		rasterizer.depthBiasEnable = VK_FALSE;

		// Multisampling
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		// Blending
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;

		// Dynamic states
		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_LINE_WIDTH,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_VIEWPORT
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = sizeof_array(dynamicStates);
		dynamicState.pDynamicStates = dynamicStates;

		// Pipeline creation
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;

		vkCreatePipelineLayout(m_Device->GetVkHandle(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout);

		auto* vulkanShader = static_cast<const VulkanShader*>(m_Spec.Shader);
		auto& shaderStages = vulkanShader->GetShaderStages();
		size_t stageCount = shaderStages.size();

		AutoRelease<VkPipelineShaderStageCreateInfo> stages = StackAlloc(stageCount * sizeof(VkPipelineShaderStageCreateInfo));

		size_t stageIndex = 0;
		for (auto& [shaderStage, shaderModule] : shaderStages)
		{
			stages[stageIndex] = {};
			stages[stageIndex].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stages[stageIndex].stage = shaderStage;
			stages[stageIndex].module = shaderModule.Module;
			stages[stageIndex].pName = shaderModule.EntryPoint.c_str();
			stages[stageIndex].pSpecializationInfo = nullptr;
			stageIndex++;
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = (uint32_t)stageCount;
		pipelineInfo.pStages = stages;

		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;

		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;

		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = static_cast<const VulkanRenderPass*>(m_Spec.RenderPass)->GetVkHandle();
		pipelineInfo.subpass = 0;

		vkCreateGraphicsPipelines(m_Device->GetVkHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline);
	}

	void VulkanPipeline::CreatePipelineResources()
	{
		const auto& shaderResources = m_Spec.Shader->GetShaderResources();

		uint32_t samplerCount = 0;
		for (auto& samplerArray : shaderResources.SamplerArrays)
		{
			samplerCount += samplerArray.SamplerCount;
		}

		m_CombinedSamplers.reserve(samplerCount);
		for (auto& samplerArray : shaderResources.SamplerArrays)
		{
			for (uint32_t i = 0; i < samplerArray.SamplerCount; i++)
			{
				SamplerSpecification spec;
				spec.Binding = samplerArray.Decorators.Binding;

				CombinedSampler cSampler;
				cSampler.Sampler = CreateScope<VulkanSampler>(m_Device, spec);
				m_CombinedSamplers.emplace_back(std::move(cSampler));
			}
		}
	}
}
