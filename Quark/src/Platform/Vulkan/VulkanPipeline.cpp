#include "qkpch.h"
#include "VulkanPipeline.h"

#include "Quark/Renderer/Renderer.h"

#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"
#include "VulkanFormats.h"
#include "VulkanRenderer.h"
#include "VulkanRenderPass.h"
#include "VulkanUniformBuffer.h"
#include "VulkanUtils.h"

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

	VulkanPipeline::VulkanPipeline(const PipelineSpecification& spec) : Pipeline(spec)
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

		m_DescriptorSets.resize(m_Spec.FramesInFlight);
		m_CameraUniformBuffers.resize(m_Spec.FramesInFlight);

		for (uint32_t i = 0; i < m_Spec.FramesInFlight; i++)
		{
			m_CameraUniformBuffers[i] = UniformBuffer::Create(m_Spec.CameraUniformBufferSize, 0);
		}

		// Descriptor pool
		{
			VkDescriptorPoolSize poolSize{};
			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = m_Spec.FramesInFlight;

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = 1;
			poolInfo.pPoolSizes = &poolSize;
			poolInfo.maxSets = m_Spec.FramesInFlight;

			vkCreateDescriptorPool(vkDevice, &poolInfo, nullptr, &m_DescriptorPool);
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

			vkCreateDescriptorSetLayout(vkDevice, &layoutInfo, nullptr, &m_DescriptorSetLayout);
		}

		// Descriptor sets
		{
			// Copy each layout for each frame in flight
			auto* layouts = static_cast<VkDescriptorSetLayout*>(alloca(m_Spec.FramesInFlight * sizeof(VkDescriptorSetLayout)));
			for (uint32_t i = 0; i < m_Spec.FramesInFlight; i++)
				layouts[i] = m_DescriptorSetLayout;

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorPool;
			allocInfo.descriptorSetCount = m_Spec.FramesInFlight;
			allocInfo.pSetLayouts = layouts;

			vkAllocateDescriptorSets(vkDevice, &allocInfo, m_DescriptorSets.data());

			for (uint32_t i = 0; i < m_Spec.FramesInFlight; i++)
			{
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = static_cast<VulkanUniformBuffer*>(m_CameraUniformBuffers[i].get())->GetVkHandle();
				bufferInfo.offset = 0;
				bufferInfo.range = m_Spec.CameraUniformBufferSize;

				VkWriteDescriptorSet descriptorWrite{};
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = m_DescriptorSets[i];
				descriptorWrite.dstBinding = 0;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pBufferInfo = &bufferInfo;

				vkUpdateDescriptorSets(vkDevice, 1, &descriptorWrite, 0, nullptr);
			}
		}

		Invalidate();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		{
			Timer t;
			vkDeviceWaitIdle(vkDevice);
			QK_CORE_INFO("Waiting for device to finish: {0}ms", t.Milliseconds().count());
		}

		vkDestroyPipeline(vkDevice, m_Pipeline, nullptr);
		vkDestroyPipelineLayout(vkDevice, m_PipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(vkDevice, m_DescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(vkDevice, m_DescriptorPool, nullptr);
	}

	void VulkanPipeline::Bind(const Ref<CommandBuffer>& commandBuffer)
	{
		auto vkCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer.get())->GetVkHandle();
		vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
		vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[VulkanRenderer::GetCurrentFrameIndex()], 0, nullptr);
	}

	void VulkanPipeline::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_Spec.ViewportWidth = viewportWidth;
		m_Spec.ViewportHeight = viewportHeight;

		Invalidate();
	}

	const Ref<UniformBuffer>& VulkanPipeline::GetUniformBuffer() const
	{
		return m_CameraUniformBuffers[VulkanRenderer::GetCurrentFrameIndex()];
	}

	void VulkanPipeline::Invalidate()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

		if (m_Pipeline)
		{
			vkDestroyPipeline(vkDevice, m_Pipeline, nullptr);
			vkDestroyPipelineLayout(vkDevice, m_PipelineLayout, nullptr);
		}

		// Vertex input 
		auto bindingDescription = Utils::GetBindingDescription(m_Spec.Layout);

		QK_CORE_ASSERT(m_Spec.Layout.GetCount() > 0, "A vertex layout must be set for the pipeline");
		auto* attributeDescriptions = static_cast<VkVertexInputAttributeDescription*>(alloca(m_Spec.Layout.GetCount() * sizeof(VkVertexInputAttributeDescription)));

		for (uint32_t i = 0; i < m_Spec.Layout.GetCount(); i++)
		{
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

		// Viewport
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_Spec.ViewportWidth;
		viewport.height = (float)m_Spec.ViewportHeight;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = VkExtent2D{ m_Spec.ViewportWidth, m_Spec.ViewportHeight };

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

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

#if 0
		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(sizeof(dynamicStates));
		dynamicState.pDynamicStates = dynamicStates;
#endif

		// Pipeline creation
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;

		vkCreatePipelineLayout(vkDevice, &pipelineLayoutInfo, nullptr, &m_PipelineLayout);

		VkPipelineShaderStageCreateInfo shaderStages[2]{};
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = static_cast<VulkanShader*>(m_Spec.Shader.get())->GetVertexVkHandle();
		shaderStages[0].pName = "main";

		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = static_cast<VulkanShader*>(m_Spec.Shader.get())->GetFragmentVkHandle();
		shaderStages[1].pName = "main";

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;

		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = static_cast<VulkanRenderPass*>(m_Spec.RenderPass.get())->GetVkHandle();
		pipelineInfo.subpass = 0;

		VkResult vkRes = vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not create the graphics pipeline");
	}
}
