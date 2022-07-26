#include "qkpch.h"
#include "VulkanPipeline.h"

#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanUniformBuffer.h"
#include "VulkanUtils.h"

// disable alloca failure warning
// since variable size stack arrays are not supported by all compilers
#pragma warning(disable : 6255)

namespace Quark {

	namespace Utils {

		static constexpr VkFormat ShaderDataTypeToVulkan(ShaderDataType type)
		{
			switch (type)
			{
				case ShaderDataType::Float:   return VK_FORMAT_R32_SFLOAT;
				case ShaderDataType::Float2:  return VK_FORMAT_R32G32_SFLOAT;
				case ShaderDataType::Float3:  return VK_FORMAT_R32G32B32_SFLOAT;
				case ShaderDataType::Float4:  return VK_FORMAT_R32G32B32A32_SFLOAT;
				case ShaderDataType::Double:  return VK_FORMAT_R64_SFLOAT;
				case ShaderDataType::Double2: return VK_FORMAT_R64G64_SFLOAT;
				case ShaderDataType::Double3: return VK_FORMAT_R64G64B64_SFLOAT;
				case ShaderDataType::Double4: return VK_FORMAT_R64G64B64A64_SFLOAT;

				// TODO: map matrices, int and bool types
				default:
					QK_CORE_FATAL("Unknown ShaderDataType");
					return VK_FORMAT_UNDEFINED;
			}
		}

		static constexpr VkCullModeFlagBits RenderCullModeToVulkan(RenderCullMode mode)
		{
			switch (mode)
			{
				case RenderCullMode::None:         return VK_CULL_MODE_NONE;
				case RenderCullMode::Front:        return VK_CULL_MODE_FRONT_BIT;
				case RenderCullMode::Back:         return VK_CULL_MODE_BACK_BIT;
				case RenderCullMode::FrontAndBack: return VK_CULL_MODE_FRONT_AND_BACK;
				default:
					QK_CORE_FATAL("Unknown culling mode");
					return VK_CULL_MODE_NONE;
			}
		}

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

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		m_DescriptorSets.resize(m_Spec.FramesInFlight);
		m_ImageAvailableSemaphores.resize(m_Spec.FramesInFlight);
		m_CameraUniformBuffers.resize(m_Spec.FramesInFlight);
		m_CommandBuffers.resize(m_Spec.FramesInFlight);

		vk::SemaphoreCreateInfo semaphoreInfo;
		for (uint32_t i = 0; i < m_Spec.FramesInFlight; i++)
		{
			m_ImageAvailableSemaphores[i] = vkDevice.createSemaphore(semaphoreInfo);
			m_CameraUniformBuffers[i] = UniformBuffer::Create(m_Spec.CameraUniformBufferSize, 0);
			m_CommandBuffers[i] = CommandBuffer::Create();
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
			QK_CORE_ASSERT(m_Spec.FramesInFlight > 0, "Number of frames in flight must be greater than zero");
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

		RecreateGraphicsPipeline();
		RecreateFramebuffers();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		{
			Timer t;
			vkDeviceWaitIdle(vkDevice);
			QK_CORE_INFO("Waiting for device to finish: {0}ms", t.Milliseconds().count());
		}

		for (auto& framebuffer : m_SwapChainFramebuffers)
			vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);

		for (auto& semaphore : m_ImageAvailableSemaphores)
			vkDestroySemaphore(vkDevice, semaphore, nullptr);

		vkDestroyPipeline(vkDevice, m_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(vkDevice, m_PipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(vkDevice, m_DescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(vkDevice, m_DescriptorPool, nullptr);
	}

	void VulkanPipeline::BeginFrame()
	{
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_Spec.FramesInFlight;

		auto& swapChain = VulkanContext::GetSwapChain();
		m_NextImageIndex = swapChain.AcquireNextImageIndex(m_ImageAvailableSemaphores[m_CurrentFrameIndex]);

		m_ActiveCommandBuffer = static_cast<VulkanCommandBuffer*>(m_CommandBuffers[m_CurrentFrameIndex].get())->GetVkHandle();
		m_CommandBuffers[m_CurrentFrameIndex]->Reset();
		m_CommandBuffers[m_CurrentFrameIndex]->Begin();

		vkCmdBindPipeline(m_ActiveCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
		vkCmdBindDescriptorSets(m_ActiveCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[m_CurrentFrameIndex], 0, nullptr);
	}

	void VulkanPipeline::EndFrame()
	{
		m_CommandBuffers[m_CurrentFrameIndex]->End();

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore waitSemaphores = m_ImageAvailableSemaphores[m_CurrentFrameIndex];

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_ActiveCommandBuffer;

		auto& swapChain = VulkanContext::GetSwapChain();
		VkSemaphore signalSemaphore = swapChain.GetRenderFinishedSemaphore();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore;

		vkQueueSubmit(VulkanContext::GetCurrentDevice().GetGraphicsQueue(), 1, &submitInfo, swapChain.GetFence());
	}

	void VulkanPipeline::BeginRenderPass(const Ref<RenderPass>& renderPass)
	{
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = static_cast<VulkanRenderPass*>(renderPass.get())->GetVkHandle();
		renderPassInfo.framebuffer = m_SwapChainFramebuffers[m_NextImageIndex];

		// must be size of framebuffer
		renderPassInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		renderPassInfo.renderArea.extent = VkExtent2D{ m_Spec.ViewportWidth, m_Spec.ViewportHeight };

		if (renderPass->GetSpecification().Clears)
		{
			VkClearValue clearColor = { 0.01f, 0.01f, 0.01f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;
		}

		vkCmdBeginRenderPass(m_ActiveCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanPipeline::EndRenderPass()
	{
		vkCmdEndRenderPass(m_ActiveCommandBuffer);
	}

	void VulkanPipeline::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_Spec.ViewportWidth = viewportWidth;
		m_Spec.ViewportHeight = viewportHeight;

		RecreateGraphicsPipeline();
		RecreateFramebuffers();
	}

	void VulkanPipeline::RecreateFramebuffers()
	{
		QK_PROFILE_FUNCTION();

		auto& swapChain = VulkanContext::GetSwapChain();
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		if (m_SwapChainFramebuffers.size() == 0)
		{
			m_SwapChainFramebuffers.resize(swapChain.GetImageCount());
		}
		else
		{
			for (auto& framebuffer : m_SwapChainFramebuffers)
				vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
		}

		for (uint32_t i = 0; i < swapChain.GetImageCount(); i++)
		{
			vk::ImageView attachments[] = {
				swapChain.GetImageView(i)
			};

			vk::FramebufferCreateInfo framebufferInfo;
			framebufferInfo.setRenderPass(static_cast<VulkanRenderPass*>(m_Spec.RenderPass.get())->GetVkHandle());
			framebufferInfo.setAttachmentCount(1);
			framebufferInfo.setPAttachments(attachments);
			framebufferInfo.setWidth(swapChain.GetWidth());
			framebufferInfo.setHeight(swapChain.GetHeight());
			framebufferInfo.setLayers(1);

			m_SwapChainFramebuffers[i] = vkDevice.createFramebuffer(framebufferInfo);
		}
	}

	void VulkanPipeline::RecreateGraphicsPipeline()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		if (m_GraphicsPipeline)
		{
			vkDevice.destroyPipeline(m_GraphicsPipeline);
			vkDevice.destroyPipelineLayout(m_PipelineLayout);
		}

		// Vertex input 
		auto bindingDescription = Utils::GetBindingDescription(m_Spec.Layout);

		QK_CORE_ASSERT(m_Spec.Layout.GetCount() > 0, "A vertex layout must be set for the pipeline");
		auto* attributeDescriptions = static_cast<VkVertexInputAttributeDescription*>(alloca(m_Spec.Layout.GetCount() * sizeof(VkVertexInputAttributeDescription)));

		for (uint32_t i = 0; i < m_Spec.Layout.GetCount(); i++)
		{
			attributeDescriptions[i].location = i;
			attributeDescriptions[i].binding = 0;
			attributeDescriptions[i].format = Utils::ShaderDataTypeToVulkan(m_Spec.Layout[i].Type);
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
		rasterizer.cullMode = Utils::RenderCullModeToVulkan(m_Spec.CullMode);
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
		vk::DynamicState dynamicStates[] = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eLineWidth
		};

		vk::PipelineDynamicStateCreateInfo dynamicState;
		dynamicState.setDynamicStateCount(static_cast<uint32_t>(sizeof(dynamicStates)));
		dynamicState.setPDynamicStates(dynamicStates);
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

		VkResult vkRes = vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not create the graphics pipeline");
	}
}
