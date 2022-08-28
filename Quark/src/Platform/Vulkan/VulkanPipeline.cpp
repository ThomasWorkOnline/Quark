#include "qkpch.h"
#include "VulkanPipeline.h"

#include "VulkanCommandBuffer.h"
#include "VulkanFormats.h"
#include "VulkanRenderPass.h"
#include "VulkanSampler.h"
#include "VulkanShader.h"
#include "VulkanUniformBuffer.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>

namespace Quark {

	namespace Utils {

		constexpr VkShaderStageFlagBits ShaderStageToVulkan(ShaderStage	stage)
		{
			switch (stage)
			{
				case ShaderStage::VertexStage:   return VK_SHADER_STAGE_VERTEX_BIT;
				case ShaderStage::GeometryStage: return VK_SHADER_STAGE_GEOMETRY_BIT;
				case ShaderStage::FragmentStage: return VK_SHADER_STAGE_FRAGMENT_BIT;
				case ShaderStage::ComputeStage:  return VK_SHADER_STAGE_COMPUTE_BIT;

				QK_ASSERT_NO_DEFAULT("Unknown shader stage");
			}

			return VK_SHADER_STAGE_ALL;
		}
	}

	VulkanPipeline::VulkanPipeline(VulkanDevice* device, const PipelineSpecification& spec)
		: Pipeline(spec)
		, m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		CreateDescriptorSetLayout();
		CreateDescriptorPoolAndSets();
		UpdateDescriptors();

		// Vertex input 
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = (uint32_t)m_Spec.Layout.GetStride();
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		AutoRelease<VkVertexInputAttributeDescription> attributeDescriptions = StackAlloc(m_Spec.Layout.GetCount() * sizeof(VkVertexInputAttributeDescription));

		for (uint32_t i = 0; i < m_Spec.Layout.GetCount(); i++)
		{
			attributeDescriptions[i] = {};
			attributeDescriptions[i].location = i;
			attributeDescriptions[i].binding = 0;
			attributeDescriptions[i].format = ShaderDataTypeToVulkan(m_Spec.Layout[i].Type);
			attributeDescriptions[i].offset = (uint32_t)m_Spec.Layout[i].Offset;
		}

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

		auto* vulkanShader = static_cast<VulkanShader*>(m_Spec.Shader);
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
		pipelineInfo.renderPass = static_cast<VulkanRenderPass*>(m_Spec.RenderPass)->GetVkHandle();
		pipelineInfo.subpass = 0;

		VkResult vkRes = vkCreateGraphicsPipelines(m_Device->GetVkHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not create the graphics pipeline");
	}

	VulkanPipeline::~VulkanPipeline()
	{
		QK_PROFILE_FUNCTION();

		vkDeviceWaitIdle(m_Device->GetVkHandle());
		vkDestroyPipeline(m_Device->GetVkHandle(), m_Pipeline, nullptr);
		vkDestroyPipelineLayout(m_Device->GetVkHandle(), m_PipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_Device->GetVkHandle(), m_DescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(m_Device->GetVkHandle(), m_DescriptorPool, nullptr);
	}

	VkDescriptorSet VulkanPipeline::GetDescriptorSet() const
	{
		return m_DescriptorSets[VulkanContext::Get()->GetCurrentFrameIndex()];
	}

	void VulkanPipeline::UpdateDescriptors()
	{
		// Uniform Buffers
		for (auto* uniformBuffer : m_Spec.UniformBuffers)
		{
			auto* ub = static_cast<VulkanUniformBuffer*>(uniformBuffer);

			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = ub->GetVkHandle();
			bufferInfo.offset = 0;
			bufferInfo.range = ub->GetSize();

			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstBinding = ub->GetBinding();
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

		// Samplers
		for (auto* sampler : m_Spec.Samplers)
		{
			auto* sp = static_cast<VulkanSampler2D*>(sampler);

			// TODO: set
			VkDescriptorImageInfo imageInfo{};
			imageInfo.sampler = sp->GetVkHandle();
			imageInfo.imageView = nullptr;
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstBinding = sp->GetBinding();
			writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSet.descriptorCount = sp->GetSpecification().SamplerCount;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescriptorSet.pImageInfo = &imageInfo;

			for (uint32_t i = 0; i < VulkanContext::FramesInFlight; i++)
			{
				writeDescriptorSet.dstSet = m_DescriptorSets[i];
				vkUpdateDescriptorSets(m_Device->GetVkHandle(), 1, &writeDescriptorSet, 0, nullptr);
			}
		}
	}

	void VulkanPipeline::CreateDescriptorSetLayout()
	{
		QK_PROFILE_FUNCTION();

		const auto& shaderResources = m_Spec.Shader->GetShaderResources();

		uint32_t bindingCount = m_Spec.Shader->GetBindingCount();
		AutoRelease<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = StackAlloc(bindingCount * sizeof(VkDescriptorSetLayoutBinding));
		VkDescriptorSetLayoutBinding* descriptorSetLayoutbindingPtr = descriptorSetLayoutBindings;

		QK_CORE_ASSERT(shaderResources.UniformBuffers.size() == m_Spec.UniformBuffers.size(), "Mismatch number of uniform buffer objects!");

		for (auto& uniformBuffer : shaderResources.UniformBuffers)
		{
			*descriptorSetLayoutbindingPtr = {};
			descriptorSetLayoutbindingPtr->binding = uniformBuffer.Binding;
			descriptorSetLayoutbindingPtr->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorSetLayoutbindingPtr->descriptorCount = 1;
			descriptorSetLayoutbindingPtr->stageFlags = Utils::ShaderStageToVulkan(uniformBuffer.Stage);
			descriptorSetLayoutbindingPtr++;
		}

		QK_CORE_ASSERT(shaderResources.CombinedSamplers.size() == m_Spec.Samplers.size(), "Mismatch number of sampler objects!");

		for (auto& sampler : shaderResources.CombinedSamplers)
		{
			*descriptorSetLayoutbindingPtr = {};
			descriptorSetLayoutbindingPtr->binding = sampler.Binding;
			descriptorSetLayoutbindingPtr->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorSetLayoutbindingPtr->descriptorCount = sampler.SamplerCount;
			descriptorSetLayoutbindingPtr->stageFlags = Utils::ShaderStageToVulkan(sampler.Stage);
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

		// Descriptor pool shared for all frames in flight
		{
			VkDescriptorPoolSize poolSize{};
			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = m_Spec.Shader->GetDescriptorCount() * VulkanContext::FramesInFlight;

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = 1;
			poolInfo.pPoolSizes = &poolSize;
			poolInfo.maxSets = VulkanContext::FramesInFlight;

			vkCreateDescriptorPool(m_Device->GetVkHandle(), &poolInfo, nullptr, &m_DescriptorPool);
		}

		// Descriptor sets
		{
			// Copy each layout for each frame in flight
			AutoRelease<VkDescriptorSetLayout> layouts = StackAlloc(VulkanContext::FramesInFlight * sizeof(VkDescriptorSetLayout));
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
}
