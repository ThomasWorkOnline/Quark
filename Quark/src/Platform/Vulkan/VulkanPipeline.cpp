#include "qkpch.h"
#include "VulkanPipeline.h"

#include "VulkanEnums.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "VulkanUniformBuffer.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>

namespace Quark {

	VulkanPipeline::VulkanPipeline(VulkanDevice* device, const PipelineSpecification& spec)
		: Pipeline(spec)
		, m_Device(device)
		, m_DescriptorSets(spec.DescriptorSetCount)
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_ASSERT(spec.RenderPass, "RenderPass must be a valid pointer to a render pass");

		CreateDescriptorSetLayout();
		CreateDescriptorPoolAndSets();
		CreatePipeline();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		QK_PROFILE_FUNCTION();

		vkDestroyPipeline(m_Device->GetVkHandle(), m_Pipeline, nullptr);
		vkDestroyPipelineLayout(m_Device->GetVkHandle(), m_PipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_Device->GetVkHandle(), m_DescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(m_Device->GetVkHandle(), m_DescriptorPool, nullptr);
	}

	bool VulkanPipeline::operator==(const Pipeline& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_Pipeline == o->m_Pipeline;

		return false;
	}

	VkDescriptorSet VulkanPipeline::GetDescriptorSet(uint32_t frameIndex) const
	{
		return m_DescriptorSets[frameIndex];
	}

	void VulkanPipeline::CreateDescriptorSetLayout()
	{
		QK_PROFILE_FUNCTION();

		const auto& shaderResources = m_Spec.Shader->GetShaderResources();

		AutoRelease<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = StackAlloc(shaderResources.BindingCount * sizeof(VkDescriptorSetLayoutBinding));
		VkDescriptorSetLayoutBinding* descriptorSetLayoutbindingPtr = descriptorSetLayoutBindings;

		for (auto& samplerArray : shaderResources.SamplerArrays)
		{
			*descriptorSetLayoutbindingPtr = {};
			descriptorSetLayoutbindingPtr->binding            = samplerArray.Decorators.Binding;
			descriptorSetLayoutbindingPtr->descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorSetLayoutbindingPtr->descriptorCount    = samplerArray.SamplerCount;
			descriptorSetLayoutbindingPtr->stageFlags         = ShaderStageToVulkan(samplerArray.Stage);
			descriptorSetLayoutbindingPtr->pImmutableSamplers = nullptr;
			descriptorSetLayoutbindingPtr++;
		}

		for (auto& uniformBuffer : shaderResources.UniformBuffers)
		{
			*descriptorSetLayoutbindingPtr = {};
			descriptorSetLayoutbindingPtr->binding            = uniformBuffer.Decorators.Binding;
			descriptorSetLayoutbindingPtr->descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorSetLayoutbindingPtr->descriptorCount    = 1;
			descriptorSetLayoutbindingPtr->stageFlags         = ShaderStageToVulkan(uniformBuffer.Stage);
			descriptorSetLayoutbindingPtr++;
		}

		QK_CORE_ASSERT(descriptorSetLayoutbindingPtr - descriptorSetLayoutBindings == shaderResources.BindingCount, "Shader binding count and resources do not match!");

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = shaderResources.BindingCount;
		layoutInfo.pBindings    = descriptorSetLayoutBindings;

		vkCreateDescriptorSetLayout(m_Device->GetVkHandle(), &layoutInfo, nullptr, &m_DescriptorSetLayout);
	}

	void VulkanPipeline::CreateDescriptorPoolAndSets()
	{
		QK_PROFILE_FUNCTION();

		// Descriptor pool shared for all frames in flight
		{
			const auto& shaderResources = m_Spec.Shader->GetShaderResources();

			uint32_t descriptorPoolCount = 0;
			descriptorPoolCount += shaderResources.UniformBuffers.size() > 0;
			descriptorPoolCount += shaderResources.SamplerArrays.size() > 0;

			AutoRelease<VkDescriptorPoolSize> poolSizes = StackAlloc(descriptorPoolCount * sizeof(VkDescriptorPoolSize));
			VkDescriptorPoolSize* poolSizesPtr = poolSizes;

			// - type is the type of descriptor.
			// - descriptorCount is the number of descriptors of that type to allocate.
			//     If type is VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK then descriptorCount is the number of bytes to allocate for descriptors of this type.

			// Uniform Buffers
			if (uint32_t count = (uint32_t)shaderResources.UniformBuffers.size())
			{
				poolSizesPtr->type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				poolSizesPtr->descriptorCount = count * m_Spec.DescriptorSetCount;
				poolSizesPtr++;
			}
			
			// Samplers
			if (uint32_t count = (uint32_t)shaderResources.SamplerArrays.size())
			{
				poolSizesPtr->type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				poolSizesPtr->descriptorCount = count * m_Spec.DescriptorSetCount;
				poolSizesPtr++;
			}

			QK_CORE_ASSERT(poolSizesPtr - poolSizes == descriptorPoolCount, "Descriptor pool count and shader resources do not match!");

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.maxSets       = m_Spec.DescriptorSetCount;
			poolInfo.poolSizeCount = descriptorPoolCount;
			poolInfo.pPoolSizes    = poolSizes;

			vkCreateDescriptorPool(m_Device->GetVkHandle(), &poolInfo, nullptr, &m_DescriptorPool);
		}

		// Descriptor sets
		{
			// Copy each layout for each frame in flight
			AutoRelease<VkDescriptorSetLayout> layouts = StackAlloc(m_Spec.DescriptorSetCount * sizeof(VkDescriptorSetLayout));
			for (uint32_t i = 0; i < m_Spec.DescriptorSetCount; i++)
				layouts[i] = m_DescriptorSetLayout;

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool     = m_DescriptorPool;
			allocInfo.descriptorSetCount = 1 * m_Spec.DescriptorSetCount;
			allocInfo.pSetLayouts        = layouts;

			vkAllocateDescriptorSets(m_Device->GetVkHandle(), &allocInfo, m_DescriptorSets.data());
		}
	}

	void VulkanPipeline::CreatePipeline()
	{
		const auto& shaderResources = m_Spec.Shader->GetShaderResources();

		AutoRelease<VkVertexInputAttributeDescription> attributeDescriptions = StackAlloc(m_Spec.Layout.GetCount() * sizeof(VkVertexInputAttributeDescription));

		for (uint32_t i = 0; i < m_Spec.Layout.GetCount(); i++)
		{
			attributeDescriptions[i] = {};
			attributeDescriptions[i].location     = i;
			attributeDescriptions[i].binding      = 0;
			attributeDescriptions[i].format       = ShaderDataTypeToVulkanBaseType(m_Spec.Layout[i].Type);
			attributeDescriptions[i].offset       = (uint32_t)m_Spec.Layout[i].Offset;
		}

		// Vertex input 
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding                = 0;
		bindingDescription.stride                 = (uint32_t)m_Spec.Layout.GetStride();
		bindingDescription.inputRate              = VK_VERTEX_INPUT_RATE_VERTEX;

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount   = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = m_Spec.Layout.GetCount();
		vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType                       = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology                    = PrimitiveTopologyToVulkan(m_Spec.Topology);
		inputAssembly.primitiveRestartEnable      = VK_FALSE;

		// Viewport
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType                       = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount               = 1;
		viewportState.scissorCount                = 1;

		// Rasterization
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType                          = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthBiasClamp                 = VK_FALSE;
		rasterizer.rasterizerDiscardEnable        = VK_FALSE;
		rasterizer.polygonMode                    = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth                      = 1.0f;
		rasterizer.cullMode                       = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace                      = VK_FRONT_FACE_CLOCKWISE; // <-- Vulkan uses right-handed coordinate system so vertex winding is flipped
		rasterizer.depthBiasEnable                = VK_FALSE;

		// Multisampling
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType                       = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable         = VK_FALSE;
		multisampling.rasterizationSamples        = SampleCountToVulkan(m_Spec.RenderPass->GetSpecification().Samples);
		multisampling.minSampleShading            = 1.0f;
		multisampling.alphaToCoverageEnable       = VK_FALSE;
		multisampling.alphaToOneEnable            = VK_FALSE;

		// Depth stencil
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable              = VK_TRUE;
		depthStencil.depthWriteEnable             = VK_TRUE;
		depthStencil.depthCompareOp               = VK_COMPARE_OP_LESS;
		depthStencil.stencilTestEnable            = VK_FALSE;
		depthStencil.minDepthBounds               = 0.0f;
		depthStencil.maxDepthBounds               = 1.0f;

		// Blending
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask       = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable          = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor  = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor  = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp         = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor  = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor  = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp         = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType                       = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable               = VK_FALSE;
		colorBlending.logicOp                     = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount             = 1;
		colorBlending.pAttachments                = &colorBlendAttachment;

		// Dynamic states
		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_LINE_WIDTH,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_VIEWPORT
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType                        = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount            = sizeof_array(dynamicStates);
		dynamicState.pDynamicStates               = dynamicStates;

		// Push constants
		uint32_t pushConstantRangeCount = (uint32_t)shaderResources.PushConstants.size();

		AutoRelease<VkPushConstantRange> pushConstantRanges = StackAlloc(pushConstantRangeCount * sizeof(VkPushConstantRange));
		VkPushConstantRange* pushConstantRangePtr = pushConstantRanges;

		for (auto& pushConstant : shaderResources.PushConstants)
		{
			*pushConstantRangePtr = {};
			pushConstantRangePtr->offset     = 0;
			pushConstantRangePtr->size       = (uint32_t)pushConstant.Size;
			pushConstantRangePtr->stageFlags = ShaderStageToVulkan(pushConstant.Stage);
			pushConstantRangePtr++;
		}

		// Pipeline creation
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = pushConstantRangeCount;
		pipelineLayoutInfo.pPushConstantRanges    = pushConstantRanges;
		pipelineLayoutInfo.setLayoutCount         = 1;
		pipelineLayoutInfo.pSetLayouts            = &m_DescriptorSetLayout;

		vkCreatePipelineLayout(m_Device->GetVkHandle(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout);

		auto* vulkanShader = static_cast<const VulkanShader*>(m_Spec.Shader);
		auto& shaderStages = vulkanShader->GetShaderStages();
		uint32_t stageCount = (uint32_t)shaderStages.size();

		AutoRelease<VkPipelineShaderStageCreateInfo> stages = StackAlloc(stageCount * sizeof(VkPipelineShaderStageCreateInfo));

		size_t stageIndex = 0;
		for (auto& [shaderStage, shaderModule] : shaderStages)
		{
			stages[stageIndex] = {};
			stages[stageIndex].sType     = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stages[stageIndex].stage     = shaderStage;
			stages[stageIndex].module    = shaderModule.Module;
			stages[stageIndex].pName     = shaderModule.EntryPoint.c_str();
			stages[stageIndex].pSpecializationInfo = nullptr;
			stageIndex++;
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount          = stageCount;
		pipelineInfo.pStages             = stages;

		pipelineInfo.pVertexInputState   = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;

		pipelineInfo.pViewportState      = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState   = &multisampling;
		pipelineInfo.pDepthStencilState  = &depthStencil;
		pipelineInfo.pColorBlendState    = &colorBlending;
		pipelineInfo.pDynamicState       = &dynamicState;

		pipelineInfo.layout              = m_PipelineLayout;
		pipelineInfo.renderPass          = static_cast<const VulkanRenderPass*>(m_Spec.RenderPass)->GetVkHandle();
		pipelineInfo.subpass             = 0;

		vkCreateGraphicsPipelines(m_Device->GetVkHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline);
	}
}
