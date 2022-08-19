#include "qkpch.h"
#include "VulkanPipeline.h"

#include "VulkanCommandBuffer.h"
#include "VulkanFormats.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "VulkanUniformBuffer.h"

// disable alloca failure warning
// since variable size stack arrays are not supported by all compilers
#pragma warning(disable : 6255)

namespace Quark {

	namespace Utils {

		static QK_CONSTEXPR20 VkVertexInputBindingDescription GetBindingDescription(const BufferLayout& layout)
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = static_cast<uint32_t>(layout.GetStride());
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}
	}

	VulkanPipeline::VulkanPipeline(VulkanDevice* device, const PipelineSpecification& spec) : Pipeline(spec),
		m_Device(device)
	{
		QK_PROFILE_FUNCTION();

		// Descriptor pool
		{
			VkDescriptorPoolSize poolSize{};
			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = VulkanContext::FramesInFlight;

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = 1;
			poolInfo.pPoolSizes = &poolSize;
			poolInfo.maxSets = VulkanContext::FramesInFlight;

			vkCreateDescriptorPool(m_Device->GetVkHandle(), &poolInfo, nullptr, &m_DescriptorPool);
		}

		// Descriptor set layout
		{
			VkDescriptorSetLayoutBinding uboLayoutBinding{};
			uboLayoutBinding.binding = 0;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = 1;
			layoutInfo.pBindings = &uboLayoutBinding;

			vkCreateDescriptorSetLayout(m_Device->GetVkHandle(), &layoutInfo, nullptr, &m_DescriptorSetLayout);
		}

		// Descriptor sets
		{
			// Copy each layout for each frame in flight
			auto* layouts = static_cast<VkDescriptorSetLayout*>(alloca(VulkanContext::FramesInFlight * sizeof(VkDescriptorSetLayout)));
			for (uint32_t i = 0; i < VulkanContext::FramesInFlight; i++)
				layouts[i] = m_DescriptorSetLayout;

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorPool;
			allocInfo.descriptorSetCount = VulkanContext::FramesInFlight;
			allocInfo.pSetLayouts = layouts;

			vkAllocateDescriptorSets(m_Device->GetVkHandle(), &allocInfo, m_DescriptorSets);
			
			if (m_Spec.UniformBuffer)
			{
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = static_cast<VulkanUniformBuffer*>(m_Spec.UniformBuffer)->GetVkHandle();
				bufferInfo.offset = 0;
				bufferInfo.range = m_Spec.UniformBuffer->GetSize();

				VkWriteDescriptorSet descriptorWrite{};
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstBinding = 0;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite.pBufferInfo = &bufferInfo;

				for (uint32_t i = 0; i < VulkanContext::FramesInFlight; i++)
				{
					descriptorWrite.dstSet = m_DescriptorSets[i];
					vkUpdateDescriptorSets(m_Device->GetVkHandle(), 1, &descriptorWrite, 0, nullptr);
				}
			}
		}

		// Vertex input 
		auto bindingDescription = Utils::GetBindingDescription(m_Spec.Layout);

		QK_CORE_ASSERT(m_Spec.Layout.GetCount() > 0, "A vertex layout must be set for the pipeline");
		auto* attributeDescriptions = static_cast<VkVertexInputAttributeDescription*>(alloca(m_Spec.Layout.GetCount() * sizeof(VkVertexInputAttributeDescription)));

		for (uint32_t i = 0; i < m_Spec.Layout.GetCount(); i++)
		{
			attributeDescriptions[i] = {};
			attributeDescriptions[i].location = i;
			attributeDescriptions[i].binding = 0;
			attributeDescriptions[i].format = ShaderDataTypeToVulkan(m_Spec.Layout[i].Type);
			attributeDescriptions[i].offset = static_cast<uint32_t>(m_Spec.Layout[i].Offset);
		}

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = m_Spec.Layout.GetCount();
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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

		auto* vulkanShader = reinterpret_cast<VulkanShader*>(m_Spec.Shader);
		auto& shaderStages = vulkanShader->GetShaderStages();
		size_t stageCount = shaderStages.size();

		auto* stages = static_cast<VkPipelineShaderStageCreateInfo*>(alloca(stageCount * sizeof(VkPipelineShaderStageCreateInfo)));

		size_t stageIndex = 0;
		for (auto& [shaderStage, shaderModule] : shaderStages)
		{
			stages[stageIndex] = {};
			stages[stageIndex].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stages[stageIndex].stage = shaderStage;
			stages[stageIndex].module = shaderModule;
			stages[stageIndex].pName = "main";
			stages[stageIndex].pSpecializationInfo = nullptr;
			stageIndex++;
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(stageCount);
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
}
